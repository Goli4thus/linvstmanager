#include "scanhandler.h"
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include "vstbucket.h"
#include "scanresult.h"
#include <QDebug>

ScanHandler::ScanHandler(const QList<VstBucket> *pVstBuckets, QObject *parent)
    : QObject(parent), mVstBuckets(pVstBuckets)
{
    mHasher = new QCryptographicHash(QCryptographicHash::Sha1);
    mapVstExtension.insert(VstType::VST2, "*.dll");
    mapVstExtension.insert(VstType::VST3, "*.vst3");
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

void ScanHandler::slotPerformScan(QString scanFolder, VstType vstType, QList<ScanResult> *scanResults)
{
    /* TODO: slotPerformScan(): Basically:
     * 1) Perform the iterative scan
     *    X filter based on VST type (.dll or .vst3)
     *    - calculate path hash on the fly and check via QList::contains, if part of mVstBuckets
     *        X if so: skip
     *        X if not: add to mScanResults
     *        - (later: if ".dll", consider checking if really VST (see TestVst app))
     */

    QByteArrayList existingPathHashes;
    QByteArray hash;
    for (int i=0; i < mVstBuckets->size(); i++) {
        existingPathHashes.append(mVstBuckets->at(i).hash);
    }

    QDirIterator it(scanFolder,
//                    QStringList() << mapVstExtension.value(vstType),
                    QStringList() << "*.dll" << "*.vst3",
                    QDir::Files,
                    QDirIterator::Subdirectories);

    qDebug() << "============================================";
    qDebug() << "========== NEW SCAN ========================";
    qDebug() << "============================================";
    QString finding;
    while (it.hasNext()) {
        finding = it.next();
//        qDebug() << "Found: " << finding;

        // Skip findings that are already part of tracked VstBuckets
        hash = calcFilepathHash(finding);
        if (!existingPathHashes.contains(hash)) {
//            qDebug() << "Already exists: YES";
//        } else {
//            qDebug() << "Already exists: NO (-->> new)";

            // TODO: Check if "*.dll" and actually a VST (needs wine?!?)
            qDebug() << "New finding: " << finding;


            QFileInfo fileType(finding);
            if ((fileType.suffix() == "dll")
                    || (fileType.suffix() == "Dll")
                    || (fileType.suffix() == "DLL")) {
                vstType = VstType::VST2;
            } else {  // ".vst3"
                vstType = VstType::VST3;
            }
            scanResults->append(ScanResult(QFileInfo(finding).baseName(),
                                           finding,
                                           vstType,
                                           hash,
                                           false));
        }
    }
    qDebug() << "--------------------------------------------";

    emit(signalScanDone());
}
