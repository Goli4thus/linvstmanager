#ifndef SCANHANDLER_H
#define SCANHANDLER_H

#include <QObject>
class QCryptographicHash;
class VstBucket;
class ScanResult;

class ScanHandler : public QObject
{
    Q_OBJECT
public:
    explicit ScanHandler(const QList<VstBucket> *pVstBuckets, const QList<ScanResult> *pScanResults, QObject *parent = nullptr);

private:
    QCryptographicHash *mHasher;
    QByteArray calcFilepathHash(QString filepath);
    const QList<VstBucket> *mVstBuckets;
    const QList<ScanResult> *mScanResults;

signals:
    void signalScanDone();

public slots:
    void slotPerformScan();

};

#endif // SCANHANDLER_H
