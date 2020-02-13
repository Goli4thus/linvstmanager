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
    explicit ModelScan(const QVector<VstBucket> *pVstBuckets, QObject *parent = nullptr);
    ~ModelScan() override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    bool mUpdateView;
    void triggerScan(const QString &scanFolder,
                     const QString &pathCheckTool64,
                     bool useCheckTool64,
                     const QString &pathCheckTool32,
                     bool useCheckTool32,
                     bool useCheckBasic);
    bool isModelEmpty();
    QVector<ScanResult> getScanSelection();
    void emptyModel();
    int getNumModelEntries();

private:
    const QVector<VstBucket> *mVstBuckets;
    QVector<ScanResult> mScanResults;
    void fillModel(QVector<ScanResult> &scanResults);
    ScanHandler *mScanHandler;
    QThread *mScanThread;

signals:
    void signalTableOperationFinished();
//    void signalPerformScan(QString scanFolder, QVector<ScanResult> scanResults, QString pathCheckTool, bool useCheckTool);
    void signalScanCanceled();
    void signalFoundVst3();
    void signalFoundVst2();
    void signalFoundDll();
    void signalScanFinished(bool findings);

public slots:
    void slotScanCancel();
    void slotScanFinished(bool wasCanceled, QVector<ScanResult> scanResults);
    void slotSelectEntry(const QVector<int> &selectionIndices);
    void slotUnselectEntry(const QVector<int> &selectionIndices);
};

#endif // MODELSCAN_H
