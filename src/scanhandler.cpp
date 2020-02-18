// This file is part of LinVstManager.

#include "scanhandler.h"
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include "vstbucket.h"
#include "scanresult.h"
#include <QDebug>
#include <QProcess>
#include <QThread>
#include "datahasher.h"
#include <QDataStream>


ScanHandler::ScanHandler(const QVector<VstBucket> &pVstBuckets,
                         QString pScanFolder,
                         bool pUseCheckBasic,
                         QObject *parent)
{
    Q_UNUSED(parent)
    mVstBuckets = pVstBuckets;
    mScanFolder = std::move(pScanFolder);
    mUseCheckBasic = pUseCheckBasic;

    mapVstExtension.insert(VstType::VST2, "*.dll");
    mapVstExtension.insert(VstType::VST3, "*.vst3");
}

void ScanHandler::verifyDll(bool &verified,
                            VstType &vstType,
                            VstProbabilityType &vstProbability,
                            const QString &finding)
{
    /*- checks that seems to make the most sense (overall no delimiting of search pattern)
     *    - probability: 100%
     *        - VSTPluginMain: case insensitive
     *        - PtsV: case sensitive
     *    - probability: 75%
     *        - PtsV: case insensitive
     *        - VstP: case insensitive
     *    - probability: NA (not available) (if no verification was applied) */

    if (mUseCheckBasic) {
        bool C1 = checkDllBasic(finding, false, "VSTPluginMain");
        bool C2 = checkDllBasic(finding, true,  "PtsV");
        bool C3 = checkDllBasic(finding, false, "PtsV");
        bool C4 = checkDllBasic(finding, false, "VstP");
        verified = true;

        if (C1 || C2) {
            vstType = VstType::VST2;
            vstProbability = VstProbabilityType::p100;
            emit (signalFoundVst2());
        } else if (C3 || C4) {
            vstType = VstType::VST2;
            vstProbability = VstProbabilityType::p75;
            emit (signalFoundVst2());
        } else {
            vstType = VstType::NoVST;
            vstProbability = VstProbabilityType::pNA;
            emit (signalFoundDll());
        }
    } else {
        // Without verification being active, blindly consider it an unverified VST2.
        vstType = VstType::VST2;
        vstProbability = VstProbabilityType::pNA;
        verified = false;
        emit (signalFoundVst2());
    }
}

bool ScanHandler::checkDllBasic(const QString &findingAbsPath, bool caseSensitive, const QString &pattern)
{
    bool retVal;
    QProcess process;
    QString strCaseSens;
    QString pathSanitized = findingAbsPath;

    pathSanitized.replace(QString(" "), QString("\\ "));

    if (caseSensitive) {
        strCaseSens = "--no-ignore-case";
    } else {
        strCaseSens = "--ignore-case";
    }

    QString cmd = (QStringList() << "bash -c \"strings " << pathSanitized << " | grep " << strCaseSens << " " << pattern << "\"").join("");
    process.start(cmd);
    process.waitForFinished();
    QString retStr(process.readAllStandardOutput());

    if (retStr.contains(pattern, Qt::CaseInsensitive)) {
        retVal = true;
    } else {
        retVal = false;
    }
    return retVal;
}

ArchType ScanHandler::checkArch(const QString &findingAbsPath)
{
    // adapted from: http://stackoverflow.com/a/495305/1338797
    ArchType result;
    QFile file(findingAbsPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return ArchType::ArchNA;
    }
    QDataStream in(&file);
    const quint8 lengthMSDosHeader = 64;
    const quint8 paddingMSDosHeader = 58;
    const quint8 paddingPEHeader = 2;
    const quint16 magicMSDoxHeader = 0x4D5A; // 'MZ'
    const quint16 magicPEHeader = 0x5045;    // 'PE'
    const quint16 IMAGE_FILE_MACHINE_I386 = 0x014c;
    const quint16 IMAGE_FILE_MACHINE_AMD64 =0x8664;

    /* format string (fitting for 64 input bytes):
     * - magic:   2x char (2 byte)
     * - padding: 58x char (58 byte)
     * - offset:  1x int (4 byte)
     * Read and check the MS-DOS-header */
    quint16 magic;
    quint32 offset;
    in.setByteOrder(QDataStream::BigEndian);
    in >> magic;
    in.skipRawData(paddingMSDosHeader);
    in.setByteOrder(QDataStream::LittleEndian);
    in >> offset;

    if (magic == magicMSDoxHeader) {
        in.skipRawData(offset - lengthMSDosHeader);
        /* format string (fitting for 6 input bytes):
         * - 2s:  2x char (2 byte)
         * - 2s:  2x char (2 byte)
         * - H:   1x unsigned short (2 byte) */
        quint16 machine;
        in.setByteOrder(QDataStream::BigEndian);
        in >> magic;
        in.skipRawData(paddingPEHeader);
        in.setByteOrder(QDataStream::LittleEndian);
        in >> machine;

        if (magic != magicPEHeader) {
            result = ArchType::ArchNA;
        } else if (machine == IMAGE_FILE_MACHINE_I386) {
            result = ArchType::Arch32;
        } else if (machine == IMAGE_FILE_MACHINE_AMD64) {
            result = ArchType::Arch64;
        } else {
            result = ArchType::ArchNA;
        }
    } else {
        result = ArchType::ArchNA;
    }

    file.close();
    return result;
}

void ScanHandler::slotPerformScan()
{
//#define D_TEST_PROGRESSBAR_FIXED_SCAN_DURATION
#ifndef D_TEST_PROGRESSBAR_FIXED_SCAN_DURATION
    QVector<ScanResult> scanResults;
    QByteArrayList existingPathHashes;
    QByteArray pathHash;
    QByteArray soFileHash;
    VstType vstType;
    ArchType archType;
    VstProbabilityType vstProbability;
    bool verified;
    bool scanCanceledByUser = false;
    /* No shared instance use by other objects is needed here,
     * because no soTmplHash updates are being done.
     */
    DataHasher pathHasher;

    for (const auto &vstBucket : mVstBuckets) {
        existingPathHashes.append(vstBucket.pathHash);
    }

    QDirIterator it(mScanFolder,
                    QStringList() << "*.dll" << "*.vst3",
                    QDir::Files,
                    QDirIterator::Subdirectories);

    qDebug() << "============================================";
    qDebug() << "========== NEW SCAN ========================";
    qDebug() << "============================================";
    QString finding;

    while (it.hasNext()) {
        finding = it.next();

        // Check if scan operation has been canceled by user
        if (QThread::currentThread()->isInterruptionRequested()) {
            scanCanceledByUser = true;
            break;
        }

        // Skip findings that are already part of tracked VstBuckets
        pathHash = pathHasher.calcFilepathHash(finding);
        if (!existingPathHashes.contains(pathHash)) {
            qDebug() << "New finding: " << finding;

            QFileInfo fileType(finding);
            if ((fileType.suffix() == "dll")
                    || (fileType.suffix() == "Dll")
                    || (fileType.suffix() == "DLL")) {
                verifyDll(verified, vstType, vstProbability, finding);
                if (vstType == VstType::NoVST) {
                    continue;
                }
            } else {  // ".vst3"
                vstType = VstType::VST3;
                vstProbability = VstProbabilityType::p100;
                verified = true;
                emit (signalFoundVst3());
            }

            archType = checkArch(finding);
            if (archType == ArchType::ArchNA) {
                // If we can't determine the architecture, then there's no
                // point in being optimistic about the probability.
                vstProbability = VstProbabilityType::pNA;
            }

            QString fileName = QFileInfo(finding).fileName();
            QString suffix = QFileInfo(finding).suffix();
            // subtract suffix including period from filename
            scanResults.append(ScanResult(fileName.chopped(suffix.size() + 1),
                                          finding,
                                          vstType,
                                          archType,
                                          vstProbability,
                                          verified,
                                          pathHash,
                                          soFileHash, // empty for now
                                          false));
        }
    }
#else
    // Test progressbar mechanism by simulating a long scan (fixed time using timer)
    for (int i=0; i < 55; i++) {
        // Check if scan operation has been canceled by user
        if (QThread::currentThread()->isInterruptionRequested()) {
            scanCanceledByUser = true;
            break;
        }
        QThread::sleep(1);
    }
#endif
    qDebug() << "--------------------------------------------";

    if (scanCanceledByUser) {
        emit(signalScanFinished(true, scanResults));
    } else {
        emit(signalScanFinished(false, scanResults));
    }
}
