// This file is part of LinVstManager.

#ifndef MODELVSTBUCKETS_H
#define MODELVSTBUCKETS_H

#include <QObject>
#include <QAbstractTableModel>
#include "vstbucket.h"
#include "scanresult.h"
#include "pathhasher.h"
class Preferences;
class LinkHandler;

class ModelVstBuckets : public QAbstractTableModel
{
    Q_OBJECT
public:
    ModelVstBuckets(QObject *parent, QList<VstBucket> &pVstBuckets, Preferences *pPrf);
    ~ModelVstBuckets();
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

    void addVstBucket(const QStringList &filepaths_VstDll);
    void removeVstBucket(QList<int>indexOfVstBuckets);
    void enableVstBucket(const QList<int> &indexOfVstBuckets);
    void disableVstBucket(const QList<int> &indexOfVstBuckets);
    void blacklistVstBucket(const QList<int> &indexOfVstBuckets);
    void unblacklistVstBucket(const QList<int> &indexOfVstBuckets);
    void updateVsts();
    void refreshStatus();
    void addScanSelection(QList<ScanResult> *scanSelection);

    QList<int> changeBridges(const QList<int> &indexOfVstBuckets, VstBridge reqBridgeType);
    bool mUpdateView;
    QStringList checkForOrphans();
    bool removeOrphans(const QStringList &filePathsOrphans);
    QList<VstBucket> *getBufferVstBuckets();

private:
    Preferences *prf;
    LinkHandler *lh;
    QList<VstBucket>mVstBuckets;
    PathHasher *pathHasher;

signals:
    void signalTableOperationFinished();
    void signalConfigDataChanged();
    void signalFeedbackLogOutput(QString msg, bool isVerbose = false);

public slots:
    void slotUpdateHashes();
};

#endif // MODELVSTBUCKETS_H
