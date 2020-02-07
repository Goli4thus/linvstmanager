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
                         QString pPathCheckTool,
                         bool pUseCheckTool,
                         QObject *parent)
{
    Q_UNUSED(parent)
    mVstBuckets = pVstBuckets;
    mScanFolder = std::move(pScanFolder);
    mPathCheckTool = std::move(pPathCheckTool);
    mUseCheckTool = pUseCheckTool;

    mapVstExtension.insert(VstType::VST2, "*.dll");
    mapVstExtension.insert(VstType::VST3, "*.vst3");
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

                if (mUseCheckTool) {
                    if (!checkDll(mPathCheckTool, finding)) {
                        // Not a VST; therefore skip.
                        emit (signalFoundDll());
                        continue;
                    }
                    verified = true;
                } else {
                    verified = false;
                }
                vstType = VstType::VST2;
                emit (signalFoundVst2());
            } else {  // ".vst3"
                vstType = VstType::VST3;
                verified = true;
                emit (signalFoundVst3());
            }
            // TODO: fix baseName problem here as well
            QString fileName = QFileInfo(finding).fileName();
            QString suffix = QFileInfo(finding).suffix();
            // subtrace suffix including period fro filename
            scanResults.append(ScanResult(fileName.chopped(suffix.size() + 1),
                                          finding,
                                          vstType,
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
