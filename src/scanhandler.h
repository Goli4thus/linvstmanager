// This file is part of LinVstManager.

#ifndef SCANHANDLER_H
#define SCANHANDLER_H

#include <QObject>
#include "enums.h"
#include <QMap>
#include <QVector>
#include "scanresult.h"
#include "vstbucket.h"

class ScanHandler : public QObject
{
    Q_OBJECT
public:
    explicit ScanHandler(const QVector<VstBucket> &pVstBuckets,
                         QString pScanFolder,
                         bool pUseCheckBasic,
                         QObject *parent = nullptr);
    static ArchType checkArch(const QString &findingAbsPath);

private:
    QVector<VstBucket> mVstBuckets;
    QString mScanFolder;
    bool mUseCheckBasic;
    QMap<VstType, QString> mapVstExtension;
    void verifyDll(bool &verified, VstType &vstType, VstProbabilityType &vstProbability, const QString &finding);
    bool checkDllBasic(const QString &findingAbsPath, bool caseSensitive, const QString &pattern);

signals:
    void signalScanFinished(bool wasCanceled, QVector<ScanResult> scanResults);
    void signalFoundVst3();
    void signalFoundVst2();
    void signalFoundDll();

public slots:
    void slotPerformScan();

};

#endif // SCANHANDLER_H
