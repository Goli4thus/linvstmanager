#ifndef SCANHANDLER_H
#define SCANHANDLER_H

#include <QObject>
#include "enums.h"
#include <QMap>
class QCryptographicHash;
class VstBucket;
class ScanResult;

class ScanHandler : public QObject
{
    Q_OBJECT
public:
    explicit ScanHandler(const QList<VstBucket> *pVstBuckets, QObject *parent = nullptr);
    ~ScanHandler();

private:
    QCryptographicHash *mHasher;
    QByteArray calcFilepathHash(QString filepath);
    const QList<VstBucket> *mVstBuckets;
    QMap<VstType, QString> mapVstExtension;
    bool checkDll(QString &pathCheckTool, QString findingAbsPath);

signals:
    void signalScanDone();
    void signalScanCanceled();
    void signalFoundVst3();
    void signalFoundVst2();
    void signalFoundDll();

public slots:
    void slotPerformScan(QString scanFolder, QList<ScanResult> *scanResults, QString pathCheckTool, bool useCheckTool);

};

#endif // SCANHANDLER_H
