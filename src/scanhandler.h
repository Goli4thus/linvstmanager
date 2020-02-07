// This file is part of LinVstManager.

#ifndef SCANHANDLER_H
#define SCANHANDLER_H

#include <QObject>
#include "enums.h"
#include <QMap>
#include "scanresult.h"
#include "vstbucket.h"

class ScanHandler : public QObject
{
    Q_OBJECT
public:
    explicit ScanHandler(const QVector<VstBucket> &pVstBuckets,
                         QString pScanFolder,
                         QString pPathCheckTool,
                         bool pUseCheckTool,
                         QObject *parent = nullptr);

private:
    QVector<VstBucket> mVstBuckets;
    QString mScanFolder;
    QString mPathCheckTool;
    bool mUseCheckTool;
    QMap<VstType, QString> mapVstExtension;
    bool checkDll(QString &pathCheckTool, const QString &findingAbsPath);

signals:
    void signalScanFinished(bool wasCanceled, QVector<ScanResult> scanResults);
    void signalFoundVst3();
    void signalFoundVst2();
    void signalFoundDll();

public slots:
    void slotPerformScan();

};

#endif // SCANHANDLER_H
