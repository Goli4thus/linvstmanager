// This file is part of LinVstManager.

#include "modelscan.h"
#include <QtDebug>
#include <QFont>
#include <QColor>
#include <QBrush>
#include "enums.h"
#include "scanhandler.h"
#include <QByteArray>

ModelScan::ModelScan(const QList<VstBucket> *pVstBuckets, QObject *parent) : mVstBuckets(pVstBuckets)
{
    this->setParent(parent);

    mUpdateView = true;
    mScanHandler = new ScanHandler(mVstBuckets);

    /* Move ScanHandler to dedicated thread to circumvent
     * obvious UI lock-up during long scans. */
    mScanHandler->moveToThread(&mScanThread);
    mScanThread.start();

    connect(this, &ModelScan::signalPerformScan, mScanHandler, &ScanHandler::slotPerformScan);
    connect(mScanHandler, &ScanHandler::signalScanDone, this, &ModelScan::slotScanDone);
    connect(mScanHandler, &ScanHandler::signalScanCanceled, this, &ModelScan::slotScanCanceled);
    connect(mScanHandler, &ScanHandler::signalFoundVst2, this, &ModelScan::signalFoundVst2);
    connect(mScanHandler, &ScanHandler::signalFoundVst3, this, &ModelScan::signalFoundVst3);
    connect(mScanHandler, &ScanHandler::signalFoundDll, this, &ModelScan::signalFoundDll);
}

ModelScan::~ModelScan()
{
    delete mScanHandler;
    mScanThread.quit();
    mScanThread.wait();
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
                        color.setRgb(255, 216, 59);  // Blue
                    } else {
                        color.setRgb(220, 220, 220);  // light grey
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

void ModelScan::triggerScan(QString scanFolder, QString pathCheckTool, bool useCheckTool)
{
    emptyModel();

    // Pass temporary buffer. Buffer associated with model will be filled after scan is finished.
    mScanResultsTmp.clear();
    emit(signalPerformScan(scanFolder, &mScanResultsTmp, pathCheckTool, useCheckTool));
}

void ModelScan::emptyModel()
{
    beginRemoveRows(QModelIndex(), 0, mScanResults.size() - 1);
    mScanResults.clear();
    endRemoveRows();
}

void ModelScan::fillModel()
{
    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount() + mScanResultsTmp.size() - 1);
    mScanResults.append(mScanResultsTmp);
    endInsertRows();
}

void ModelScan::slotScanDone()
{
    fillModel();
    emit(signalScanDone(!mScanResults.isEmpty()));
}

void ModelScan::slotScanCancel()
{
    mScanThread.requestInterruption();
}

void ModelScan::slotScanCanceled()
{
    emit(signalScanCanceled());
}

bool ModelScan::isModelEmpty()
{
    return mScanResults.isEmpty();
}

QList<ScanResult> ModelScan::getScanSelection()
{
    QList<ScanResult> scanSelection;
    for (int i=0; i < mScanResults.size(); i++) {
        if (mScanResults.at(i).selected) {
            scanSelection.append(mScanResults.at(i));
        }
    }

    return scanSelection;
}

void ModelScan::slotSelectEntry(QList<int> selectionIndices)
{
    for(int i = selectionIndices.size() - 1; i >= 0; i--) {
        mScanResults[selectionIndices.at(i)].selected = true;
    }
}

void ModelScan::slotUnselectEntry(QList<int> selectionIndices)
{
    for(int i = selectionIndices.size() - 1; i >= 0; i--) {
        mScanResults[selectionIndices.at(i)].selected = false;
    }
}
