// This file is part of LinVstManager.

#ifndef MODELSCAN_H
#define MODELSCAN_H

#include <QObject>
#include <QAbstractTableModel>
#include "vstbucket.h"
#include "scanresult.h"
//#include <QReadWriteLock>
#include <QThread>
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
    void triggerScan(QString scanFolder, QString pathCheckTool, bool useCheckTool);
    bool isModelEmpty();
    QList<ScanResult> getScanSelection();
    void emptyModel();

private:
    ScanHandler *mScanHandler;
    const QList<VstBucket> *mVstBuckets;
    QList<ScanResult> mScanResults;
    QList<ScanResult> mScanResultsTmp;
    void fillModel();
    QThread mScanThread;

signals:
    void signalTableOperationFinished();
    void signalPerformScan(QString scanFolder, QList<ScanResult> *scanResults, QString pathCheckTool, bool useCheckTool);
    void signalScanDone(bool findings);
    void signalScanCanceled();
    void signalFoundVst3();
    void signalFoundVst2();
    void signalFoundDll();

public slots:
    void slotScanDone();
    void slotScanCancel();
    void slotScanCanceled();
    void slotSelectEntry(QList<int> selectionIndices);
    void slotUnselectEntry(QList<int> selectionIndices);
};

#endif // MODELSCAN_H
