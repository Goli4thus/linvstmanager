// This file is part of LinVstManager.

#include "scanhandler.h"
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include "vstbucket.h"
#include "scanresult.h"
#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include "datahasher.h"
#include <QDataStream>
#include <QElapsedTimer>
#include <QDebug>


ScanHandler::ScanHandler(const QVector<VstBucket> &pVstBuckets,
                         QString pScanFolder,
                         bool pUseCheckBasic,
                         QObject *parent)
{
    Q_UNUSED(parent)
    mVstBuckets = pVstBuckets;
    mScanFolder = std::move(pScanFolder);
    mUseCheckBasic = pUseCheckBasic;
    mIdealThreadCount = QThread::idealThreadCount();
    mWorkerDone.resize(mIdealThreadCount);
    for (auto &i : mWorkerDone) {
        i = true;
    }
}


void ScanHandler::slotPerformScan()
{
    QElapsedTimer eTimer;
    eTimer.start();
    qDebug() << "Scan start: ";
    QVector<ScanResult> scanResults;
    QByteArrayList existingPathHashes;
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

//    qDebug() << "============================================";
//    qDebug() << "========== NEW SCAN ========================";
//    qDebug() << "============================================";
    QStringList findings;
    while (it.hasNext()) {
        // Collect all filepaths first
        findings.append(it.next());
    }

    // Depending on amount, split up QStringList into multiple segments (i.e. 2 or 4)
    QVector<QStringList> findingsWorkerSublist;
    int amount = findings.size();
    int numberOfWorkers;
    if (amount < 20) {
        // Prepare for two worker threads
        findingsWorkerSublist.resize(2);
        findingsWorkerSublist[0].append(findings.mid(0, amount / 2));
        findingsWorkerSublist[1].append(findings.mid(amount / 2));
        numberOfWorkers = 2;
    } else if (amount <= 128) {  // 128 is the max threadcount of a AMD Threadripper CPU atm.
        // Prepare for four worker threads
        int sub = amount / 4;
        findingsWorkerSublist.resize(4);
        findingsWorkerSublist[0].append(findings.mid(0, sub));
        findingsWorkerSublist[1].append(findings.mid(sub, sub));
        findingsWorkerSublist[2].append(findings.mid(sub * 2, sub));
        findingsWorkerSublist[3].append(findings.mid(sub * 3));
        numberOfWorkers = 4;
    } else {
        // Prepare for ideal number of worker threads
        int sub = amount / mIdealThreadCount;
        numberOfWorkers = mIdealThreadCount;
        findingsWorkerSublist.resize(numberOfWorkers);
        for (int i=0; i < (numberOfWorkers - 1); ++i) {
            findingsWorkerSublist[i].append(findings.mid(sub * i, sub));
        }
        findingsWorkerSublist[numberOfWorkers-1].append(findings.mid(sub * (numberOfWorkers - 1)));
    }

    qDebug() << "Scanning with " << numberOfWorkers << "threads.";

    // Create required amount of worker threads, make connections, and start them.
    mScanWorkers.resize(numberOfWorkers);
    mScanWorkerThreads.resize(numberOfWorkers);
    mScanResults.resize(numberOfWorkers);
    for (int i=0; i < numberOfWorkers; ++i) {
        // ----------- worker --------------------------------------
        mScanWorkers[i] = new ScanWorker(i,
                                         findingsWorkerSublist[i],
                                         mUseCheckBasic,
                                         existingPathHashes);
        mScanWorkerThreads[i] = new QThread(this);
        mScanWorkers[i]->moveToThread(mScanWorkerThreads[i]);

        // Signal chain for scan start
        connect(mScanWorkerThreads[i], &QThread::started, mScanWorkers[i], &ScanWorker::slotPerformScan);

        // Signal chain for scan finish/cancel
        connect(mScanWorkers[i], &ScanWorker::signalScanWorkerFinished, this, &ScanHandler::slotScanWorkerFinished);
        connect(mScanWorkers[i], &ScanWorker::signalScanWorkerFinished, mScanWorkers[i], &ScanWorker::deleteLater);
        connect(mScanWorkerThreads[i], &QThread::finished, mScanWorkerThreads[i], &QThread::deleteLater);

        // Scan progress reporting
        connect(mScanWorkers[i], &ScanWorker::signalFoundVst2, this, &ScanHandler::signalFoundVst2);
        connect(mScanWorkers[i], &ScanWorker::signalFoundVst3, this, &ScanHandler::signalFoundVst3);
        connect(mScanWorkers[i], &ScanWorker::signalFoundDll, this, &ScanHandler::signalFoundDll);
    }

    // Start worker threads which will their portion of the scan
    for (int i=0; i < numberOfWorkers; ++i) {
        mWorkerDone[i] = false;
        mScanWorkerThreads[i]->start();
    }


    forever {
        QCoreApplication::processEvents();
        // Wait for worker threads to finish, while:
        //   - polling "user interruption request"
        if (QThread::currentThread()->isInterruptionRequested()) {
            scanCanceledByUser = true;
            for (int i=0; i < numberOfWorkers; ++i) {
                mScanWorkerThreads[i]->requestInterruption();
            }
        }

        // If user cancels: Trigger cancellation of worker threads, wait for them
        // to finish and finish ScanThread itself

        //   - polling worker threads to finish (poll flags that are set by slots)
        if (!mWorkerDone.contains(false)) {
//        if (mWorkerDone[0] && mWorkerDone[1] && mWorkerDone[2] && mWorkerDone[3]) {
            if (!scanCanceledByUser) {
                // If worker finish normally: Combine results and finish ScanThread
                for (int i=0; i < numberOfWorkers; ++i) {
                    scanResults.append(mScanResults[i]);
                }
            }
            break;
        }

    }
//    qDebug() << "--------------------------------------------";

    if (scanCanceledByUser) {
        emit(signalScanFinished(true, scanResults));
    } else {
        emit(signalScanFinished(false, scanResults));
    }
    qDebug() << "Scan end: " << eTimer.restart() << " ms ";
}

void ScanHandler::slotScanWorkerFinished(const quint8 pWorkerID,
                                         bool pWasCanceled,
                                         QVector<ScanResult> pScanResults)
{
    mWorkerDone[pWorkerID] = true;
    if (!pWasCanceled) {
        mScanResults[pWorkerID] = pScanResults;
    }
    mScanWorkerThreads[pWorkerID]->quit();
}
