#include "scanhandler.h"
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDir>
#include "vstbucket.h"
#include "scanresult.h"

ScanHandler::ScanHandler(const QList<VstBucket> *pVstBuckets, const QList<ScanResult> *pScanResults, QObject *parent)
    : QObject(parent), mVstBuckets(pVstBuckets), mScanResults(pScanResults)
{
    mHasher = new QCryptographicHash(QCryptographicHash::Sha1);

}

ScanHandler::~ScanHandler()
{
    delete mHasher;
}

QByteArray ScanHandler::calcFilepathHash(QString filepath)
{
    // Calculate sha1-hash of filepath_VstDll
    mHasher->reset();
    mHasher->addData(filepath.toUtf8());
    return mHasher->result();
}

void ScanHandler::slotPerformScan()
{

}
