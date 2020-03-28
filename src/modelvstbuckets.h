// This file is part of LinVstManager.

#ifndef MODELVSTBUCKETS_H
#define MODELVSTBUCKETS_H

#include <QObject>
#include <QAbstractTableModel>
#include "vstbucket.h"
#include "scanresult.h"
#include "datahasher.h"
class Preferences;
class LinkHandler;

class ModelVstBuckets : public QAbstractTableModel
{
    Q_OBJECT
public:
    ModelVstBuckets(QObject *parent, QVector<VstBucket> &pVstBuckets, Preferences *pPrf, DataHasher &pDataHasher);
    ~ModelVstBuckets() override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

    void addVstBucket(const QStringList &filepaths_VstDll);
    void removeVstBucket(QVector<int>indexOfVstBuckets);
    void renameVstBucket(const int &indexOfVstBucket, const QString &nameNew);
    void renameVstBucket(const QVector<int> &indices, const bool modeAdd, const bool atEnd, const QString &phrase);
    void enableVstBucket(const QVector<int> &indexOfVstBuckets);
    void disableVstBucket(const QVector<int> &indexOfVstBuckets);
    void blacklistVstBucket(const QVector<int> &indexOfVstBuckets);
    void unblacklistVstBucket(const QVector<int> &indexOfVstBuckets);
    void updateVsts();
    void refreshStatus();
    void addScanSelection(QVector<ScanResult> *scanSelection);

    QVector<int> changeBridges(const QVector<int> &indexOfVstBuckets, VstBridge reqBridgeType);
    bool mUpdateView;
    QStringList checkForOrphans();
    bool removeOrphans(const QStringList &filePathsOrphans);
    QVector<VstBucket> *getBufferVstBuckets();
    int getSizeVstBuckets();

private:
    Preferences *prf;
    LinkHandler *lh;
    QVector<VstBucket>mVstBuckets;
    DataHasher &dataHasher;

signals:
    void signalTableOperationFinished();
    void signalConfigDataChanged(bool needsRefresh = false, QVector<VstBridge> changedBridges = {});
    void signalFeedbackLogOutput(QString msg, bool isVerbose = false);
    void signalFeedbackUpdateDone();

public slots:
    void slotUpdateHashes();
};

#endif // MODELVSTBUCKETS_H
