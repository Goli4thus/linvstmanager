#ifndef MODELVSTBUCKETS_H
#define MODELVSTBUCKETS_H

#include <QObject>
#include <QAbstractTableModel>
#include "vstbucket.h"
class QCryptographicHash;
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

    // TODO: Consider adding return values to these methods in conjunctionw with LinkHandler implementation
    void addVstBucket(QStringList filepaths_VstDll);
    void removeVstBucket(QList<int>indexOfVstBuckets);
    void enableVstBucket(QList<int>indexOfVstBuckets);
    void disableVstBucket(QList<int>indexOfVstBuckets);
    void blacklistVstBucket(QList<int>indexOfVstBuckets);
    void unblacklistVstBucket(QList<int>indexOfVstBuckets);
    void updateVsts();
    void refreshStatus();

    QList<int> changeBridges(QList<int>indexOfVstBuckets, VstBridge reqBridgeType);
    bool mUpdateView;
    QStringList checkForOrphans();
    bool removeOrphans(QStringList filePathsOrphans);
    QList<VstBucket> *getBufferVstBuckets();

private:
    QCryptographicHash *mHasher;
    QByteArray calcFilepathHash(QString filepath);
    Preferences *prf;
    LinkHandler *lh;
    QList<VstBucket>mVstBuckets;

signals:
    void signalTableOperationFinished();
    void signalConfigDataChanged();

public slots:
    void slotUpdateHashes();
};

#endif // MODELVSTBUCKETS_H
