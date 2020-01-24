#ifndef LINKHANDLER_H
#define LINKHANDLER_H

#include <QObject>
#include "enums.h"
#include "vstbucket.h"
#include <QMap>
class Preferences;

class LinkHandler : public QObject
{
    Q_OBJECT
public:
    explicit LinkHandler(const Preferences &t_prf, QObject *parent = nullptr);
    RvLinkHandler refreshStatus(QList<VstBucket>&pVstBuckets, bool refreshSingle = false, int singleIndex = 0);
    RvLinkHandler updateVsts(QList<VstBucket>&pVstBuckets);
    RvLinkHandler enableVst(VstBucket &pVstBucket);
    RvLinkHandler disableVst(VstBucket &pVstBucket);
    RvLinkHandler blacklistVst(VstBucket &pVstBucket);
    RvLinkHandler changeBridge(VstBucket &pVstBucket, VstBridge newBridgeType);
    RvLinkHandler checkForOrphans(QList<VstBucket>&pVstBuckets);
    RvLinkHandler removeOrphans();

private:
    const Preferences &prf;
    bool checkSoFileMatch(QString filePathA, QString filePathB);
    QMap<VstType, int> mMapVstExtLen;

signals:

public slots:

};

#endif // LINKHANDLER_H
