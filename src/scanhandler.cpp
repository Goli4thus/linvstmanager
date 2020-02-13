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
#include "VstDllCheck/vstdllcheck.h"
#include "datahasher.h"


ScanHandler::ScanHandler(const QVector<VstBucket> &pVstBuckets,
                         QString pScanFolder,
                         QString pPathCheckTool64,
                         bool pUseCheckTool64,
                         QString pPathCheckTool32,
                         bool pUseCheckTool32,
                         bool pUseCheckBasic,
                         QObject *parent)
{
    Q_UNUSED(parent)
    mVstBuckets = pVstBuckets;
    mScanFolder = std::move(pScanFolder);
    mPathCheckTool64 = std::move(pPathCheckTool64);
    mUseCheckTool64 = pUseCheckTool64;
    mPathCheckTool32 = std::move(pPathCheckTool32);
    mUseCheckTool32 = pUseCheckTool32;
    mUseCheckBasic = pUseCheckBasic;

    mapVstExtension.insert(VstType::VST2, "*.dll");
    mapVstExtension.insert(VstType::VST3, "*.vst3");
}

void ScanHandler::verifyDll(bool &verified,
                            VstType &vstType,
                            BitType &bitType,
                            const QString &finding)
{
    verified = false;
    vstType = VstType::VST2;
    bitType = BitType::BitsNA;

    if (mUseCheckBasic) {
        if (checkDllBasic(finding)) {
            verified = true;
            emit (signalFoundVst2());
        } else {
            vstType = VstType::NoVST;
            emit (signalFoundDll());
        }
    } else {
        if (mUseCheckTool64) {
            if (checkDll(mPathCheckTool64, finding)) {
                bitType = BitType::Bits64;
            }
            verified = true;
        }

        if ((bitType == BitType::BitsNA) && mUseCheckTool32) {
            if (checkDll(mPathCheckTool32, finding)) {
                bitType = BitType::Bits32;
            }
            verified = true;
        }

        if ( (   (mUseCheckTool64 && (bitType == BitType::BitsNA))
                 && (mUseCheckTool32 && (bitType == BitType::BitsNA)))
             ||
             (   (!mUseCheckTool64)
                 && (mUseCheckTool32 && (bitType == BitType::BitsNA)))
             ||
             (   (mUseCheckTool64 && (bitType == BitType::BitsNA))
                 && (!mUseCheckTool32)) ) {
            /* Neither a verified 64 bit, nor a verified 32 bit VST
             * (despite verification being active); therefore skip. */
            vstType = VstType::NoVST;
            emit (signalFoundDll());
        } else {
            /* Found a VST. Either due to successful verification or
             * no verification being active (indicated by 'verified'
             * not being set). */
            emit (signalFoundVst2());
        }
    }
}

bool ScanHandler::checkDll(QString &pathCheckTool, const QString &findingAbsPath)
{
    /*
     * Call the external utility "vstdllcheck" and evaluate its exit code
     * to determine if the specified dll (via absPath) is a VST or not.
     */
    bool retVal;
    int exitCode;

    // Enclose 'finding' path in quotes to counteract possible spaces in path
    exitCode = QProcess::execute(pathCheckTool + " \"" + findingAbsPath + "\"");
    switch (exitCode) {
        case D_CHECK_PASSED:
            qDebug() << "checkDll(): " << "D_CHECK_PASSED" << "( " << findingAbsPath << " )";
            retVal = true;
        break;
        case D_CHECK_FAILED_LIBRARYLOADERROR:
            qDebug() << "checkDll(): " << "D_CHECK_FAILED_LIBRARYLOADERROR" << "( " << findingAbsPath << " )";
            retVal = false;
        break;
        case D_CHECK_FAILED_ENTRYPOINT_NOT_FOUND:
            qDebug() << "checkDll(): " << "D_CHECK_FAILED_ENTRYPOINT_NOT_FOUND" << "( " << findingAbsPath << " )";
            retVal = false;
        break;
        case D_CHECK_FAILED_AEFFECTERROR:
            qDebug() << "checkDll(): " << "D_CHECK_FAILED_AEFFECTERROR" << "( " << findingAbsPath << " )";
            retVal = false;
        break;
        case D_CHECK_FAILED_NO_KEFFECTMAGIC_NO_VST:
            qDebug() << "checkDll(): " << "D_CHECK_FAILED_NO_KEFFECTMAGIC_NO_VST" << "( " << findingAbsPath << " )";
            retVal = false;
        break;
        case D_CHECK_FAILED_NO_PROCESSREPLACING:
            qDebug() << "checkDll(): " << "D_CHECK_FAILED_NO_PROCESSREPLACING" << "( " << findingAbsPath << " )";
            retVal = false;
        break;
        case D_CHECK_FAILED_NO_EDITOR:
            qDebug() << "checkDll(): " << "D_CHECK_FAILED_NO_EDITOR" << "( " << findingAbsPath << " )";
            retVal = false;
        break;
        default:
            qDebug() << "checkDll(): " << "default case (shouldn't happen at all";
            retVal = false;
        break;
    }

    return retVal;
}

bool ScanHandler::checkDllBasic(const QString &findingAbsPath)
{
    bool retVal;
    QProcess process;
    QString pathSanitized = findingAbsPath;

    pathSanitized.replace(QString(" "), QString("\\ "));

    QString cmd = (QStringList() << "bash -c \"strings " << pathSanitized << " | grep --no-ignore-case ^VSTPluginMain$\"").join("");
    process.start(cmd);
    process.waitForFinished();
    QString retStr1(process.readAllStandardOutput());

    cmd = (QStringList() << "bash -c \"strings " << pathSanitized << " | grep --no-ignore-case ^VstP$\"").join("");
    process.start(cmd);
    process.waitForFinished();
    QString retStr2(process.readAllStandardOutput());

    if ((retStr1 == "VSTPluginMain\n") || (retStr2 == "VstP\n")) {
        retVal = true;
    } else {
        retVal = false;
    }
    return retVal;
}

void ScanHandler::slotPerformScan()
{
    /* Basically:
     * 1) Perform the iterative scan
     *    X filter based on VST type (.dll or .vst3)
     *    X calculate path hash on the fly and check via QVector::contains, if part of mVstBuckets
     *        X if so: skip
     *        X if not: add to mScanResults
     *    O (later: if ".dll", consider checking if really VST (see TestVst app))
     */


//#define D_TEST_PROGRESSBAR_FIXED_SCAN_DURATION
#ifndef D_TEST_PROGRESSBAR_FIXED_SCAN_DURATION
    QVector<ScanResult> scanResults;
    QByteArrayList existingPathHashes;
    QByteArray pathHash;
    QByteArray soFileHash;
    VstType vstType;
    BitType bitType;
    bool verified;
    bool scanCanceledByUser = false;
    /* No shared instance with other object needed here,
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
                verifyDll(verified, vstType, bitType, finding);
                if (vstType == VstType::NoVST) {
                    continue;
                }
            } else {  // ".vst3"
                vstType = VstType::VST3;
                bitType = BitType::BitsNA;
                verified = true;
                emit (signalFoundVst3());
            }

            QString fileName = QFileInfo(finding).fileName();
            QString suffix = QFileInfo(finding).suffix();
            // subtract suffix including period from filename
            scanResults.append(ScanResult(fileName.chopped(suffix.size() + 1),
                                          finding,
                                          vstType,
                                          bitType,
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
