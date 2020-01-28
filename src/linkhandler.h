#ifndef LINKHANDLER_H
#define LINKHANDLER_H

#include <QObject>
#include "enums.h"
#include "vstbucket.h"
#include <QStringList>
#include <QMap>
class Preferences;

class LinkHandler : public QObject
{
    Q_OBJECT
public:
    explicit LinkHandler(const Preferences &t_prf, QList<VstBucket>*pVstBuckets, QObject *parent = nullptr);
    RvLinkHandler refreshStatus(bool refreshSingle = false, int singleIndex = 0);
    RvLinkHandler updateVsts();
    RvLinkHandler enableVst(int idx);
    RvLinkHandler disableVst(int idx);
    RvLinkHandler blacklistVst(int idx);
    RvLinkHandler changeBridge(int idx, VstBridge newBridgeType);
    QStringList checkForOrphans();
    RvLinkHandler removeOrphans(QStringList filePathsOrphans);

private:
    const Preferences &prf;
    bool checkSoFileMatch(QString filePathA, QString filePathB);
    QMap<VstType, int> mMapVstExtLen;
    QList<VstBucket>*mVstBuckets;

signals:

public slots:

};

#endif // LINKHANDLER_H
