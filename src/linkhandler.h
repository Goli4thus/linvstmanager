// This file is part of LinVstManager.

#ifndef LINKHANDLER_H
#define LINKHANDLER_H

#include <QObject>
#include "enums.h"
#include "vstbucket.h"
#include <QStringList>
#include <QMap>
class Preferences;
class PathHasher;

class LinkHandler : public QObject
{
    Q_OBJECT
public:
    explicit LinkHandler(const Preferences &t_prf, QList<VstBucket>*pVstBuckets, QObject *parent = nullptr);
    ~LinkHandler();
    RvLinkHandler refreshStatus(bool refreshSingle = false, int singleIndex = 0);
    RvLinkHandler updateVsts();
    RvLinkHandler enableVst(int idx);
    RvLinkHandler disableVst(int idx);
    RvLinkHandler blacklistVst(int idx);
    RvLinkHandler changeBridge(int idx, VstBridge newBridgeType);
    QStringList checkForOrphans();
    RvLinkHandler removeOrphans(const QStringList &filePathsOrphans);

private:
    const Preferences &prf;
    bool checkSoFileMatch(const QString &filePathA, const QString &filePathB);
    QMap<VstType, int> mMapVstExtLen;
    QList<VstBucket>*mVstBuckets;
    PathHasher *pathHasher;

signals:

public slots:

};

#endif // LINKHANDLER_H
