// This file is part of LinVstManager.

#ifndef LINKHANDLER_H
#define LINKHANDLER_H

#include <QObject>
#include "enums.h"
#include "vstbucket.h"
#include <QStringList>
#include <QMap>
class Preferences;
class DataHasher;

class LinkHandler : public QObject
{
    Q_OBJECT
public:
    explicit LinkHandler(const Preferences &t_prf, QVector<VstBucket>*pVstBuckets, DataHasher &pDataHasher, QObject *parent = nullptr);
    ~LinkHandler();
    RvLinkHandler refreshStatus(bool refreshSingle = false, int singleIndex = 0, bool updateSoFileHash = false, bool checkExistingForConflict = false);
    RvLinkHandler updateVsts();
    RvLinkHandler enableVst(const QVector<int> &indexOfVstBuckets);
    RvLinkHandler disableVst(const QVector<int> &indexOfVstBuckets);
    RvLinkHandler renameVst(const int &indexOfVstBucket, const QString &nameNew);
    RvLinkHandler blacklistVst(const QVector<int> &indexOfVstBuckets);
    RvLinkHandler changeBridge(int idx, VstBridge newBridgeType);
    QStringList checkForOrphans();
    RvLinkHandler removeOrphans(const QStringList &filePathsOrphans);
    ArchType checkArch(const QString &findingAbsPath);
    void updateArch();

private:
    const Preferences &prf;
    bool checkSoHashMatch(const QByteArray &soFileHash, const VstBridge vstBridge);
    QVector<VstBucket>*mVstBuckets;
    DataHasher &dataHasher;
    void updateConflicts();

signals:

public slots:

};

#endif // LINKHANDLER_H
