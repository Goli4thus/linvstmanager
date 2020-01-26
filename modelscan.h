#ifndef MODELSCAN_H
#define MODELSCAN_H

#include <QObject>
#include <QAbstractTableModel>
#include "vstbucket.h"
#include "scanresult.h"
class ScanHandler;

class ModelScan : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ModelScan(const QList<VstBucket> *pVstBuckets, QObject *parent = nullptr);
    ~ModelScan();
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    bool mUpdateView;
    void triggerScan();
    void emptyModel();
    bool isModelEmpty();

private:
    ScanHandler *scanHandler;
    const QList<VstBucket> *mVstBuckets;
    QList<ScanResult> mScanResults;

signals:
    void signalTableOperationFinished();

public slots:
    void slotFillModel();
    void slotSelectEntry(QList<int> selectionIndices);
    void slotUnselectEntry(QList<int> selectionIndices);
};

#endif // MODELSCAN_H
