// This file is part of LinVstManager.

#include "modelvstbuckets.h"
#include <QtDebug>
#include <QFont>
#include <QColor>
#include <QFileInfo>
#include <QBrush>
#include <QDir>
#include "preferences.h"
#include "enums.h"
#include "linkhandler.h"
#include "scanresult.h"

ModelVstBuckets::ModelVstBuckets(QObject *parent, QVector<VstBucket> &pVstBuckets, Preferences *pPrf, DataHasher &pDataHasher)
    : dataHasher(pDataHasher)
{
    this->setParent(parent);
    mUpdateView = true;
    prf = pPrf;

    // Fill model with data if available
    if (!pVstBuckets.empty()) {
        mVstBuckets.append(pVstBuckets);
        slotUpdateHashes();
    }

    lh = new LinkHandler(*prf, &mVstBuckets, dataHasher);
    lh->refreshStatus(false, 0, true);
}

ModelVstBuckets::~ModelVstBuckets()
{
    delete lh;
}

int ModelVstBuckets::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mVstBuckets.size();
}

int ModelVstBuckets::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    // Columns:
    // | New | Status | Name | Type | Bridge | Path | Index |
    return 7;
}

QVariant ModelVstBuckets::data(const QModelIndex &index, int role) const
{
    if (mUpdateView) {
        switch(role) {
            case Qt::DisplayRole: {
                switch(index.column()) {
                    case nsMW::TableColumnPosType::NewlyAdded: {
                        if (mVstBuckets.at(index.row()).newlyAdded) {
                            return QString("*");
                        } else {
                            return QString(" ");
                        }

                    }
                    case nsMW::TableColumnPosType::Status: {
                        switch (mVstBuckets.at(index.row()).status) {
                            case Enabled: {
                                return QString("Enabled");
                            }
                            case Disabled: {
                                return QString("Disabled");
                            }
                            case Mismatch: {
                                return QString("Mismatch");
                            }
                            case No_So: {
                                return QString("No_So");
                            }
                            case NotFound: {
                                return QString("NotFound");
                            }
                            case Orphan: {
                                return QString("Orphan");
                            }
                            case NoBridge: {
                                return QString("NoBridge");
                            }
                            case NA: {
                                return QString("NA");
                            }
                            case Blacklisted: {
                                return QString("Blacklisted");
                            }
                        }
                    }
                    break;
                    case nsMW::TableColumnPosType::Type: {
                        switch (mVstBuckets.at(index.row()).vstType) {
                            case VstType::VST2: {
                                return QString("VST2");
                            }
                            case VstType::VST3: {
                                return QString("VST3");
                            }
                        }
                    }
                    break;
                    case nsMW::TableColumnPosType::Bridge: {
                        switch (mVstBuckets.at(index.row()).bridge) {
                            case VstBridge::LinVst: {
                                return QString("LinVst");
                            }
                            case VstBridge::LinVstX: {
                                return QString("LinVst-X");
                            }
                            case VstBridge::LinVst3: {
                                return QString("LinVst3");
                            }
                            case VstBridge::LinVst3X: {
                                return QString("LinVst3-X");
                            }
                        }
                    }
                    break;
                    case nsMW::TableColumnPosType::Name: {
                        return mVstBuckets.at(index.row()).name;
                    }
                    case nsMW::TableColumnPosType::Path: {
//                        // Show in '~/' notation
//                        int lenHome = QDir::homePath().length();
//                        int lenFile = mVstBuckets.at(index.row()).vstPath.length();
//                        return mVstBuckets.at(index.row()).vstPath.right(lenFile - lenHome);
                        return mVstBuckets.at(index.row()).vstPath;
                    }
                    case nsMW::TableColumnPosType::Index: {   // original index
                        return index.row();
                    }
                }
            }
            break;
//            case Qt::BackgroundRole: {
//                if (index.column() == TableColumnPosType::NewlyAdded) {
//                    QColor color(22, 22, 22);
//                    return QBrush(color, Qt::SolidPattern);
//                }
//                if (index.column() == 2) { // bridge
//                    QColor color;
//                    bool bridgeEnabledInPrf;

//                    if ((mVstBuckets.at(index.row()).bridge == VstBridge::LinVst)
//                            && (prf->getEnabledLinVst() == false)) {
//                        bridgeEnabledInPrf = false;
//                    }
//                    else if ((mVstBuckets.at(index.row()).bridge == VstBridge::LinVstX)
//                            && (prf->getEnabledLinVstX() == false)) {
//                        bridgeEnabledInPrf = false;
//                    }
//                    else if ((mVstBuckets.at(index.row()).bridge == VstBridge::LinVst3)
//                            && (prf->getEnabledLinVst3() == false)) {
//                        bridgeEnabledInPrf = false;
//                    }
//                    else if ((mVstBuckets.at(index.row()).bridge == VstBridge::LinVst3X)
//                            && (prf->getEnabledLinVst3X() == false)) {
//                        bridgeEnabledInPrf = false;
//                    } else {
//                        bridgeEnabledInPrf = true;
//                    }

//                    if (bridgeEnabledInPrf == true) {
//                        color.setRgb(255, 255, 255);  // White
//                    } else {
//                        color.setRgb(226, 85, 37);  // Red
//                    }

//                    return QBrush(color, Qt::SolidPattern);
//                }
//            }
//            break;
            case Qt::ForegroundRole: {
//                if (index.column() == 0) {
//                    QColor color;
//                    switch (mVstBuckets.at(index.row()).status) {
//                    case VstStatus::Enabled:
//                        color.setRgb(87, 196, 25); // Green
//                        break;
//                    case VstStatus::Disabled:
//                        color.setRgb(255, 212, 41); // Yellow
//                        break;
//                    case VstStatus::Mismatch:
//                        color.setRgb(226, 85, 37);  // Red
//                        break;
//                    case VstStatus::No_So:
//                        color.setRgb(91, 160, 255);  // Blue
//                        break;
//                    case VstStatus::NotFound:
//                        color.setRgb(226, 85, 37);  // Red
//                    case VstStatus::NA:
//                        color.setRgb(220, 220, 220);  // light grey
//                        break;
//                    case VstStatus::Blacklisted:
//                        color.setRgb(22, 22, 22);  // Black
//                        break;
//                    default:
//                        break;
//                    }
//                    return QBrush(color, Qt::Dense1Pattern);
//                }
                if (index.column() == nsMW::TableColumnPosType::Bridge) {
                    QColor color;

                    if (!prf->bridgeEnabled(mVstBuckets.at(index.row()).bridge)) {
                        color.setRgb(226, 85, 37);  // Red
                    } else {
                        return QVariant();
                    }

                    return QBrush(color, Qt::Dense1Pattern);
                }
            }
            break;
            case Qt::DecorationRole: {
                if (index.column() == nsMW::TableColumnPosType::Status) {
                    QColor color;
                    switch (mVstBuckets.at(index.row()).status) {
                    case VstStatus::Enabled:
                        color.setRgb(87, 196, 25); // Green
                        break;
                    case VstStatus::Disabled:
                        color.setRgb(255, 212, 41); // Yellow
                        break;
                    case VstStatus::Mismatch:
                        color.setRgb(226, 85, 37);  // Red
                        break;
                    case VstStatus::No_So:
                        color.setRgb(91, 160, 255);  // Blue
                        break;
                    case VstStatus::NotFound:
                        color.setRgb(182, 0, 231);  // Violette
                        break;
                    case VstStatus::NoBridge:
                        color.setRgb(226, 85, 37);  // Red
                        break;
                    case VstStatus::NA:
                        color.setRgb(220, 220, 220);  // light grey
                        break;
                    case VstStatus::Blacklisted:
                        color.setRgb(51, 51, 51);  // Black
                        break;
                    default:
                        break;
                    }
                    return color;
                }
            }
            break;
            case Qt::ToolTipRole: {
                if (index.column() == nsMW::TableColumnPosType::NewlyAdded) {
                    return QString("*\t: Newly added");
                }
            }
            break;
            case Qt::TextAlignmentRole: {
                if (index.column() == nsMW::TableColumnPosType::NewlyAdded && role == Qt::TextAlignmentRole) {
                    return Qt::AlignCenter;
                } else {
                    return QVariant();
                }
            }
        }
    }
    return QVariant();
}

QVariant ModelVstBuckets::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
        case Qt::DisplayRole: {
            if (orientation == Qt::Horizontal) {
                switch (section)
                {
                case nsMW::TableColumnPosType::NewlyAdded:
                    return QString("N");
                case nsMW::TableColumnPosType::Status:
                    return QString("Status");
                case nsMW::TableColumnPosType::Bridge:
                    return QString("Bridge");
                case nsMW::TableColumnPosType::Name:
                    return QString("Name");
                case nsMW::TableColumnPosType::Type:
                    return QString("Type");
                case nsMW::TableColumnPosType::Path:
                    return QString("Path");
                case nsMW::TableColumnPosType::Index:
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
            if (section == nsMW::TableColumnPosType::NewlyAdded) {
                return QString("Newly added");
            } else if(section == nsMW::TableColumnPosType::Status) {
                return QString("Enabled,     // VST is enabled via active softlink\n"
                               "Disabled,    // VST is disabled due to missing softlink\n"
                               "Mismatch,    // Mismatch between linvst.so and *.so file associated with VST-dll\n"
                               "No_So,       // VST-dll has no associated VST-so file\n"
                               "NotFound,    // VST-dll can't be found using the specified config path\n"
                               "NoBridge,    // No suitable bridge has been enabled in preferences.\n"
                               "Orphan,      // The so-file seems orphaned as it doesn't refer to an existing VST-dll\n"
                               "NA,          // Initial state\n"
                               "Blacklisted  // VST is blacklisted from being handled");
            } else if (section == nsMW::TableColumnPosType::Bridge) {
                return QString("A bridge colored in red means\n"
                               "the bridge is disabled in preferences");
            }
        }
        break;
    }
    return QVariant();
}

void ModelVstBuckets::addVstBucket(const QStringList &filepaths_VstDll)
{
    // Clear 'newlyAdded' flags
    for (auto vstBucket : mVstBuckets) {
        vstBucket.newlyAdded = false;
    }

    // Start processing selection
    for (const auto &filepath : filepaths_VstDll) {
        QString fileName = QFileInfo(filepath).fileName();
        QString suffix = QFileInfo(filepath).suffix();
        QByteArray filepath_Hash = dataHasher.calcFilepathHash(filepath);
        QByteArray soFile_Hash = dataHasher.calcFilepathHash(filepath);

        // Check if VST dll is already part of model and skip if so
        bool newVstFound = true;
        for (const auto & vstBucket : mVstBuckets) {
            if (vstBucket.pathHash.compare(filepath_Hash) == 0) {
               newVstFound = false;
               break;
            }
            /* Q: Would skipping based on filename alone instead of full path be more robust?
             * -->> Depends...
             *    - If we assume that the same VST is installed in different folders,
             *      then the hash would be different and both would be added as of now.
             *    - Using a 'filename only' approach would fix this, but if two vendors create
             *      a VST with the same name (i.e. EQ.dll), then only one would be added.
             *      Whereas with a 'full path' approach both would.
             */
        }

        if (newVstFound) {
            // Check if VST2 or VST3 file and set 'bridge' type accordingly
            VstType vstType;
            VstBridge bridgeType;
            QFileInfo fileType(filepath);
            if ((fileType.suffix() == "dll")
                    || (fileType.suffix() == "Dll")
                    || (fileType.suffix() == "DLL")) {
                vstType = VstType::VST2;

                if (prf->getBridgeDefaultVst2IsX()) {
                    bridgeType = VstBridge::LinVstX;
                } else {
                    bridgeType = VstBridge::LinVst;
                }
            } else {
                vstType = VstType::VST3;

                if (prf->getBridgeDefaultVst3IsX()) {
                    bridgeType = VstBridge::LinVst3X;
                } else {
                    bridgeType = VstBridge::LinVst3;
                }
            }
            /* One could invoke linkhandler here to get newly added
             * VSTs to state "Disabled" right away. But if we consider that the user might
             * want to change the bridge of a VST right after the add, getting to "Disabled"
             * (meaning creating a so-file alongside the added VST file) beforehand would have
             * been for nothing.
             * So we leave the decision of when to perform an 'update' for the user.
             */

            beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());
            // subtrace suffix including period fro filename
            mVstBuckets.append(VstBucket(fileName.chopped(suffix.size() + 1),
                                         filepath,
                                         filepath_Hash,
                                         soFile_Hash,
                                         VstStatus::NA,
                                         bridgeType,
                                         vstType,
                                         true));
            endInsertRows();
        }
        else
        {
            emit (signalFeedbackLogOutput("- Skipped VST (already there): " + filepath));
        }
    }
    //    qDebug() << "filepath_Hash: " << filepath_Hash;
    emit(signalConfigDataChanged(true));
}

void ModelVstBuckets::removeVstBucket(QVector<int>indexOfVstBuckets)
{
    /* Sort the original indices so that we can start removal with
     * the highest index and avoid running into issues of indices
     * becoming 'outdated'. */
    std::sort(indexOfVstBuckets.begin(), indexOfVstBuckets.end());
    int index = 0;

    lh->blacklistVst(indexOfVstBuckets);

    for(int i = indexOfVstBuckets.size() - 1; i >= 0; i--) {
        index = indexOfVstBuckets.at(i);
        beginRemoveRows(QModelIndex(), index, index);
        // Removing a VstBucket is the same as blacklisting it with
        // the addition of removing it from the model.
//        QVector<int> tmp;
//        tmp.append(index);
//        lh->blacklistVst(tmp);
        mVstBuckets.removeAt(index);
        endRemoveRows();
    }

    emit(signalConfigDataChanged());
}

void ModelVstBuckets::enableVstBucket(const QVector<int> &indexOfVstBuckets)
{
    if (lh->enableVst(indexOfVstBuckets) == RvLinkHandler::LH_OK) {
        emit(signalConfigDataChanged());
    } else {
        //            QString msg("(MVB): enableVstBucket: not LH_OK for index: " + QString::number(index));
        //            qDebug() << msg;
        //            emit(signalFeedbackLogOutput(msg, true));
    }
}

void ModelVstBuckets::disableVstBucket(const QVector<int> &indexOfVstBuckets)
{
    if (lh->disableVst(indexOfVstBuckets) == RvLinkHandler::LH_OK) {
        emit(signalConfigDataChanged());
    } else {
//		QString msg("(MVB): disableVstBucket: not LH_OK for index: " + QString::number(index));
//		qDebug() << msg;
//		emit(signalFeedbackLogOutput(msg, true));
    }
}

void ModelVstBuckets::blacklistVstBucket(const QVector<int> &indexOfVstBuckets)
{
    if (lh->blacklistVst(indexOfVstBuckets) == RvLinkHandler::LH_OK) {
        emit(signalConfigDataChanged());
    } else {
        //            QString msg("(MVB): blacklistVstBucket: not LH_OK for index: " + QString::number(index));
        //            qDebug() << msg;
        //            emit(signalFeedbackLogOutput(msg, true));
    }
}

void ModelVstBuckets::unblacklistVstBucket(const QVector<int> &indexOfVstBuckets)
{
    for(const auto &index : indexOfVstBuckets) {
        mVstBuckets[index].status = VstStatus::NA;

        if (lh->refreshStatus(true, index) == RvLinkHandler::LH_OK) {
            emit(signalConfigDataChanged());
        } else {
            QString msg("(MVB): unblacklistVstBucket: not LH_OK for index: " + QString::number(index));
            qDebug() << msg;
            emit(signalFeedbackLogOutput(msg, true));
        }
    }
}

void ModelVstBuckets::updateVsts()
{
    if (lh->updateVsts() == RvLinkHandler::LH_OK) {
        emit(signalFeedbackUpdateDone());
    } else {
        QString msg("(MVB): updateVsts: not LH_OK");
        qDebug() << msg;
        emit(signalFeedbackLogOutput(msg, true));
    }
}

void ModelVstBuckets::refreshStatus()
{
    lh->refreshStatus();
}

void ModelVstBuckets::addScanSelection(QVector<ScanResult> *scanSelection)
{
    VstBridge bridgeType;

    // Clear 'newlyAdded' flags
    for (auto &vstBucket : mVstBuckets) {
        vstBucket.newlyAdded = false;
    }

    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount() + scanSelection->size() - 1);
    for (const auto &i : *scanSelection) {
        if (i.vstType == VstType::VST2) {
            if (prf->getBridgeDefaultVst2IsX()) {
                bridgeType = VstBridge::LinVstX;
            } else {
                bridgeType = VstBridge::LinVst;
            }
        } else {
            if (prf->getBridgeDefaultVst3IsX()) {
                bridgeType = VstBridge::LinVst3X;
            } else {
                bridgeType = VstBridge::LinVst3;
            }
        }

        mVstBuckets.append(VstBucket(i.name,
                                     i.vstPath,
                                     i.pathHash,
                                     i.soFileHash, // empty for now (no_so yet)
                                     VstStatus::NA,
                                     bridgeType,
                                     i.vstType,
                                     true));
    }
    endInsertRows();

    emit(signalConfigDataChanged());
}

QVector<int> ModelVstBuckets::changeBridges(const QVector<int> &indexOfVstBuckets, VstBridge reqBridgeType)
{
    int index;
    bool configDataChanged = false;
    QVector<int> skippedIndices;

    for(int i = indexOfVstBuckets.size() - 1; i >= 0; i--) {
        index = indexOfVstBuckets.at(i);

        if ((mVstBuckets[index].vstType == VstType::VST2)
                && (   (reqBridgeType == VstBridge::LinVst)
                    || (reqBridgeType == VstBridge::LinVstX))) {
            if (prf->bridgeEnabled(reqBridgeType)) {
                if(lh->changeBridge(index, reqBridgeType) == RvLinkHandler::LH_OK) {
                    // Refresh to make sure it actually worked with preserving the initial status
                    lh->refreshStatus(true, index);
                    configDataChanged = true;
                }
            } else {
                // Request bridge type is not enabled in preferences
                skippedIndices.append(index);
            }
        } else if ((mVstBuckets[index].vstType == VstType::VST3) // VST3
                && (   (reqBridgeType == VstBridge::LinVst3)
                    || (reqBridgeType == VstBridge::LinVst3X))) {
            if (prf->bridgeEnabled(reqBridgeType)) {
                if(lh->changeBridge(index, reqBridgeType) == RvLinkHandler::LH_OK) {
                    // Refresh to make sure it actually worked with preserving the initial status
                    lh->refreshStatus(true, index);
                    configDataChanged = true;
                }
            } else {
                // Request bridge type is not enabled in preferences
                skippedIndices.append(index);
            }
        } else {
            // Request bridge type doesn't fit the selected VST's type.
            skippedIndices.append(index);
        }
    }

    if (configDataChanged) {
        emit(signalConfigDataChanged());
    }
    return skippedIndices;
}

QStringList ModelVstBuckets::checkForOrphans()
{
    return lh->checkForOrphans();
}

bool ModelVstBuckets::removeOrphans(const QStringList &filePathsOrphans)
{
    return (lh->removeOrphans(filePathsOrphans) == RvLinkHandler::LH_OK);
}

QVector<VstBucket> *ModelVstBuckets::getBufferVstBuckets()
{
    return &mVstBuckets;
}

void ModelVstBuckets::slotUpdateHashes()
{
    for (auto &vstBucket : mVstBuckets) {
        vstBucket.pathHash = dataHasher.calcFilepathHash(vstBucket.vstPath);
    }
}

