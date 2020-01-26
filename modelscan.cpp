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
    scanHandler = new ScanHandler(mVstBuckets, &mScanResults);

    // TODO: Remove dummy data later on
    mScanResults.append(ScanResult("dummy-1",
                                   "/some/path/to/somewhere/dummy-1",
                                   QByteArray(),
                                   false));
    mScanResults.append(ScanResult("dummy-2",
                                   "/some/path/to/somewhere/dummy-2",
                                   QByteArray(),
                                   false));
    mScanResults.append(ScanResult("dummy-3",
                                   "/some/path/to/somewhere/dummy-3",
                                   QByteArray(),
                                   false));
    mScanResults.append(ScanResult("dummy-4",
                                   "/some/path/to/somewhere/dummy-4",
                                   QByteArray(),
                                   false));
}

ModelScan::~ModelScan()
{
    delete scanHandler;
}

int ModelScan::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mScanResults.size();
}

int ModelScan::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    // Columns:
    // | Selected | Name | Path | Index |
    return 4;
}

QVariant ModelScan::data(const QModelIndex &index, int role) const
{
    if (mUpdateView) {
        switch(role) {
            case Qt::DisplayRole: {
                switch(index.column()) {
                    case 0: {
                        if (mScanResults.at(index.row()).selected) {
                            return "-S-";
                        } else {
                            return "---";
                        }
                    }
                    break;
                    case 1: {
                        return mScanResults.at(index.row()).name;
                    }
                    break;
                    case 2: {
                        return mScanResults.at(index.row()).vstPath;
                    }
                    break;
                    case 3: {   // original index
                        return index.row();
                    }
                    break;
                }
            }
            break;
            case Qt::DecorationRole: {
                if (index.column() == 0) {
                    QColor color;
                    if (mScanResults.at(index.row()).selected) {
                        color.setRgb(91, 160, 255);  // Blue
                    } else {
                        color.setRgb(220, 220, 220);  // light grey
                    }
                    return color;
                }
            }
            break;
            case Qt::ToolTipRole: {
                if (index.column() == 0) {
                    return QString("-S-\t: Selected\n"
                                   "---\t: Unselected\n");
                }
            }
        }
    }
    return QVariant();}

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
                    return QString("Path");
                case 3:
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
        break;
        case Qt::TextAlignmentRole: {
            return Qt::AlignLeft + Qt::AlignVCenter;
        }
        break;
        case Qt::ToolTipRole: {
            if (section == 0) {
                return QString("-S-\t: Selected\n"
                               "---\t: Unselected\n");
            }
        }
        break;
    }
    return QVariant();
}

void ModelScan::triggerScan()
{
    /*
     * Q: The actual scan should probably be done in a separate thread, shouldn't it?
     */
}

void ModelScan::emptyModel()
{

}

bool ModelScan::isModelEmpty()
{
    return mScanResults.isEmpty();
}

void ModelScan::slotFillModel()
{

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
