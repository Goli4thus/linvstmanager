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
    ~ModelScan() override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    bool mUpdateView;
    void triggerScan(const QString &scanFolder, const QString &pathCheckTool, bool useCheckTool);
    bool isModelEmpty();
    QList<ScanResult> getScanSelection();
    void emptyModel();

private:
    const QList<VstBucket> *mVstBuckets;
    QList<ScanResult> mScanResults;
    void fillModel(QList<ScanResult> &scanResults);
    ScanHandler *mScanHandler;
    QThread *mScanThread;

signals:
    void signalTableOperationFinished();
//    void signalPerformScan(QString scanFolder, QList<ScanResult> scanResults, QString pathCheckTool, bool useCheckTool);
    void signalScanCanceled();
    void signalFoundVst3();
    void signalFoundVst2();
    void signalFoundDll();
    void signalScanFinished(bool findings);

public slots:
    void slotScanCancel();
    void slotScanFinished(bool wasCanceled, QList<ScanResult> scanResults);
    void slotSelectEntry(const QList<int> &selectionIndices);
    void slotUnselectEntry(const QList<int> &selectionIndices);
};

#endif // MODELSCAN_H
