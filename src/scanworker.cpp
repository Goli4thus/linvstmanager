// This file is part of LinVstManager.

#include "scanworker.h"
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

ScanWorker::ScanWorker(const quint8 pWorkerID,
                       const QStringList pFindingPaths,
                       bool pUseCheckBasic,
                       const QByteArrayList pExistingPathHashes,
                       QObject *parent)
    : mWorkerID(pWorkerID),
      mFindingPaths(pFindingPaths),
      mUseCheckBasic(pUseCheckBasic),
      mExistingPathHashes(pExistingPathHashes)
{
    Q_UNUSED(parent)
}

void ScanWorker::verifyDll(bool &verified,
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

bool ScanWorker::checkDllBasic(const QString &findingAbsPath,
                               bool caseSensitive,
                               const QString &pattern)
{
    bool retVal;
    QProcess process;
    QString strCaseSens;
    QString pathSanitized = findingAbsPath;

    /* Sanitize path by putting into single quotes and escaping
     * any single quotes already with the path */
    pathSanitized.replace(QString("\'"), QString("\'\\\'\'"));
    pathSanitized.prepend("'");
    pathSanitized.append("'");

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

ArchType ScanWorker::checkArch(const QString &findingAbsPath)
{
    // adapted from: http://stackoverflow.com/a/495305/1338797
    ArchType result;
    QFile file(findingAbsPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "checkArch(): " << "NA: file open problem (" << findingAbsPath << ")";
        return ArchType::ArchNA;
    }
    QDataStream in(&file);
    const quint8 lengthMSDosHeader = 64;
    const quint8 paddingMSDosHeader = 58;
    const quint8 paddingPEHeader = 2;
    const quint16 magicMSDosHeader = 0x4D5A; // 'MZ'
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

    if (magic == magicMSDosHeader) {
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
            qDebug() << "checkArch(): " << "NA: magicPEHeader not there (" << findingAbsPath << ")";
        } else if (machine == IMAGE_FILE_MACHINE_I386) {
            result = ArchType::Arch32;
        } else if (machine == IMAGE_FILE_MACHINE_AMD64) {
            result = ArchType::Arch64;
        } else {
            result = ArchType::ArchNA;
            qDebug() << "checkArch(): " << "NA: Unknown arch: " << machine << " (" << findingAbsPath << ")";
        }
    } else {
        result = ArchType::ArchNA;
            qDebug() << "checkArch(): " << "NA: magicMSDosHeader not there (" << findingAbsPath << ")";
    }

    file.close();
    return result;
}

void ScanWorker::slotPerformScan()
{
//#define D_TEST_PROGRESSBAR_FIXED_SCAN_DURATION
#ifndef D_TEST_PROGRESSBAR_FIXED_SCAN_DURATION
    QVector<ScanResult> scanResults;
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


    for (const auto &finding : mFindingPaths) {
        // Check if scan operation has been canceled by user
        if (QThread::currentThread()->isInterruptionRequested()) {
            scanCanceledByUser = true;
            break;
        }

        // Skip findings that are already part of tracked VstBuckets
        pathHash = pathHasher.calcFilepathHash(finding);
        if (!mExistingPathHashes.contains(pathHash)) {
//            qDebug() << "New finding: " << finding;

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
    for (int i=0; i < 55; ++i) {
        // Check if scan operation has been canceled by user
        if (QThread::currentThread()->isInterruptionRequested()) {
            scanCanceledByUser = true;
            break;
        }
        QThread::sleep(1);
    }
#endif
//    qDebug() << "--------------------------------------------";

    if (scanCanceledByUser) {
        emit(signalScanWorkerFinished(mWorkerID, true, scanResults));
    } else {
        emit(signalScanWorkerFinished(mWorkerID, false, scanResults));
    }
}
