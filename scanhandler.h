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

signals:
    void signalScanDone();

public slots:
    void slotPerformScan(QString scanFolder, VstType vstType, QList<ScanResult> *scanResults);

};

#endif // SCANHANDLER_H
