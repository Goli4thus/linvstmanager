// This file is part of LinVstManager.

#include "modelscan.h"
#include <QtDebug>
#include <QFont>
#include <QColor>
#include <QBrush>
#include "enums.h"
#include "scanhandler.h"
#include <QByteArray>
#include <QMetaType>
#include "colors.h"

ModelScan::ModelScan(const QVector<VstBucket> *pVstBuckets, QObject *parent) : mVstBuckets(pVstBuckets)
{
    this->setParent(parent);

    mUpdateView = true;
}

ModelScan::~ModelScan()
{
    delete mScanHandler;
}

int ModelScan::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mScanResults.size();
}

int ModelScan::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    // Columns:
    // | Selected | Name | Type | Path | (Index) |
    return 4;
}

QVariant ModelScan::data(const QModelIndex &index, int role) const
{
    if (mUpdateView) {
        switch(role) {
            case Qt::DisplayRole: {
                switch(index.column()) {
                    case nsDS::TableColumnPosType::Status: {
                        if (mScanResults.at(index.row()).selected) {
                            return "-S-";
                        } else {
                            return "---";
                        }
                    }
                    case nsDS::TableColumnPosType::Name: {
                        return mScanResults.at(index.row()).name;
                    }
                    case nsDS::TableColumnPosType::Type: {
                        switch (mScanResults.at(index.row()).vstType) {
                            case VstType::VST2: {
                                if (mScanResults.at(index.row()).verified) {
                                    return QString("VST2");
                                } else {
                                    return QString("VST2 (?)");
                                }
                            }
                            case VstType::VST3: {
                                return QString("VST3");
                            }
                        }
                    }
                    break;
                    case nsDS::TableColumnPosType::Path: {
                        return mScanResults.at(index.row()).vstPath;
                    }
                    case nsDS::TableColumnPosType::Index: {   // original index
                        return index.row();
                    }
                }
            }
            break;
            case Qt::DecorationRole: {
                if (index.column() == nsDS::TableColumnPosType::Status) {
                    QColor color;
                    if (mScanResults.at(index.row()).selected) {
                        color = Colors::getColorYellow();
                    } else {
                        color = Colors::getColorLightGrey();
                    }
                    return color;
                }
            }
            break;
            case Qt::ToolTipRole: {
                if (index.column() == nsDS::TableColumnPosType::Status) {
                    return QString("-S-\t: Selected\n"
                                   "---\t: Unselected\n");
                } else if (index.column() == nsDS::TableColumnPosType::Type) {
                    return QString("VST2 (?)\t: Unverified dll-file\n");
                }
            }
        }
    }
    return QVariant();
}

QVariant ModelScan::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
        case Qt::DisplayRole: {
            if (orientation == Qt::Horizontal) {
                switch (section)
                {
                case 0:
                    return QString("S");
                case 1:
                    return QString("Name");
                case 2:
                    return QString("Type");
                case 3:
                    return QString("Path");
                case 4:
                    return QString("Index");
                }
            }
        }
        break;
        case Qt::FontRole: {
            QFont boldFont;
            boldFont.setBold(false);
            return boldFont;
        }
        case Qt::TextAlignmentRole: {
            return Qt::AlignLeft + Qt::AlignVCenter;
        }
        case Qt::ToolTipRole: {
            if (section == nsDS::TableColumnPosType::Status) {
                return QString("-S-\t: Selected\n"
                               "---\t: Unselected\n");
            } else if (section == nsDS::TableColumnPosType::Type) {
                return QString("VST2 (?)\t: Unverified dll-file\n");
            }
        }
        break;
    }
    return QVariant();
}

void ModelScan::triggerScan(const QString &scanFolder,
                            const QString &pathCheckTool64,
                            bool useCheckTool64,
                            const QString &pathCheckTool32,
                            bool useCheckTool32)
{
    emptyModel();

    /* Move ScanHandler to dedicated thread to circumvent
     * obvious UI lock-up during long scans. */
    mScanHandler = new ScanHandler(*mVstBuckets, scanFolder, pathCheckTool64, useCheckTool64, pathCheckTool32, useCheckTool32);
    mScanThread = new QThread(this);
    mScanHandler->moveToThread(mScanThread);

    // Signal chain for scan start
    connect(mScanThread, &QThread::started, mScanHandler, &ScanHandler::slotPerformScan);

    // Signal chain for scan finish/cancel
    connect(mScanHandler, &ScanHandler::signalScanFinished, this, &ModelScan::slotScanFinished);
    connect(mScanHandler, &ScanHandler::signalScanFinished, mScanHandler, &ScanHandler::deleteLater);
    connect(mScanThread, &QThread::finished, mScanThread, &QThread::deleteLater);

    // Scan progress reporting
    connect(mScanHandler, &ScanHandler::signalFoundVst2, this, &ModelScan::signalFoundVst2);
    connect(mScanHandler, &ScanHandler::signalFoundVst3, this, &ModelScan::signalFoundVst3);
    connect(mScanHandler, &ScanHandler::signalFoundDll, this, &ModelScan::signalFoundDll);

    // Start thread which will start scan
    mScanThread->start();
}

void ModelScan::emptyModel()
{
    beginRemoveRows(QModelIndex(), 0, mScanResults.size() - 1);
    mScanResults.clear();
    endRemoveRows();
}

void ModelScan::fillModel(QVector<ScanResult> &scanResults)
{
    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount() + scanResults.size() - 1);
    mScanResults.append(scanResults);
    endInsertRows();
}

void ModelScan::slotScanFinished(bool wasCanceled, QVector<ScanResult> scanResults)
{
    if (wasCanceled) {
        emit(signalScanCanceled());
    } else {
        fillModel(scanResults);
        emit(signalScanFinished(!scanResults.isEmpty()));
    }

    mScanThread->quit();
}

void ModelScan::slotScanCancel()
{
    mScanThread->requestInterruption();
}

bool ModelScan::isModelEmpty()
{
    return mScanResults.isEmpty();
}

QVector<ScanResult> ModelScan::getScanSelection()
{
    QVector<ScanResult> scanSelection;
    for (const auto &scanResult : mScanResults) {
        if (scanResult.selected) {
            scanSelection.append(scanResult);
        }
    }

    return scanSelection;
}

void ModelScan::slotSelectEntry(const QVector<int> &selectionIndices)
{
    for(int i = selectionIndices.size() - 1; i >= 0; i--) {
        mScanResults[selectionIndices.at(i)].selected = true;
    }
}

void ModelScan::slotUnselectEntry(const QVector<int> &selectionIndices)
{
    for(int i = selectionIndices.size() - 1; i >= 0; i--) {
        mScanResults[selectionIndices.at(i)].selected = false;
    }
}
