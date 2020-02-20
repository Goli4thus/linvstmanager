// This file is part of LinVstManager.

#ifndef SCANHANDLER_H
#define SCANHANDLER_H

#include <QObject>
#include "enums.h"
#include <QMap>
#include <QVector>
#include "scanresult.h"
#include "vstbucket.h"
#include "scanworker.h"

class ScanHandler : public QObject
{
    Q_OBJECT
public:
    explicit ScanHandler(const QVector<VstBucket> &pVstBuckets,
                         QString pScanFolder,
                         bool pUseCheckBasic,
                         QObject *parent = nullptr);
//    static ArchType checkArch(const QString &findingAbsPath);

private:
    QVector<VstBucket> mVstBuckets;
    QString mScanFolder;
    bool mUseCheckBasic;
    QVector<ScanWorker *> mScanWorkers;
    QVector<QThread *> mScanWorkerThreads;
    QVector<QVector<ScanResult>> mScanResults;
    QVector<bool> mWorkerDone;

signals:
    void signalScanFinished(bool wasCanceled, QVector<ScanResult> scanResults);
    void signalFoundVst3();
    void signalFoundVst2();
    void signalFoundDll();

public slots:
    void slotPerformScan();
    void slotScanWorkerFinished(const quint8 pWorkerID, bool pWasCanceled,
                                QVector<ScanResult> pScanResults);

};

#endif // SCANHANDLER_H
