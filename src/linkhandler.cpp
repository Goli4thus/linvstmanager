// This file is part of LinVstManager.

#include "linkhandler.h"
#include "preferences.h"
#include <qdebug.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStringList>
#include "pathhasher.h"

LinkHandler::LinkHandler(const Preferences &t_prf, QList<VstBucket> *pVstBuckets, QObject *parent) : QObject(parent), prf(t_prf)
{
    mMapVstExtLen.insert(VstType::VST2, 4); // ".dll"
    mMapVstExtLen.insert(VstType::VST3, 5); // ".vst3"
    mVstBuckets = pVstBuckets;

    pathHasher = new PathHasher();
}

LinkHandler::~LinkHandler()
{
    delete pathHasher;
}

RvLinkHandler LinkHandler::refreshStatus(bool refreshSingle, int singleIndex)
{
    QFileInfo fileInfoVst;
    QFileInfo fileInfoSo;
    QFileInfo fileInfoLink;
    QString filePathSoTmpl;

    for (int i=0; i < mVstBuckets->size(); i++) {
        if (refreshSingle && (i != singleIndex)) {
            continue;
        } else {
            if ((*mVstBuckets).at(i).status == VstStatus::Blacklisted) {
                continue;
            } else {
                fileInfoVst.setFile((*mVstBuckets).at(i).vstPath);

                if (fileInfoVst.exists()) {
                    fileInfoSo.setFile(fileInfoVst.filePath().left(
                                           fileInfoVst.filePath().size() - mMapVstExtLen.value((*mVstBuckets).at(i).vstType)) + ".so");

                    if (!prf.bridgeEnabled((*mVstBuckets).at(i).bridge)) {
                        (*mVstBuckets)[i].status = VstStatus::NoBridge;
                        continue;
                    } else {
                        if (fileInfoSo.exists()) {
                            fileInfoLink.setFile(prf.getPathLinkFolder() + "/" + (*mVstBuckets).at(i).name + ".so");


                            filePathSoTmpl = prf.getPathSoTmplBridge((*mVstBuckets).at(i).bridge);

                            if (checkSoFileMatch(filePathSoTmpl, fileInfoSo.filePath())) {
                                // Qt docs re QFileInfo::exists():
                                // "Note: If the file is a symlink that points to a non-existing file, false is returned."
                                // -->> No problem in this case, as we already made sure the source so-file exists.
                                if (fileInfoLink.exists()) {
                                    (*mVstBuckets)[i].status = VstStatus::Enabled;
                                } else {
                                    (*mVstBuckets)[i].status = VstStatus::Disabled;
                                }
                            } else {
                                (*mVstBuckets)[i].status = VstStatus::Mismatch;
                            }
                        } else {
                            (*mVstBuckets)[i].status = VstStatus::No_So;
                        }
                    }
                } else {
                    (*mVstBuckets)[i].status = VstStatus::NotFound;
                }
            }
        }
    }

    return RvLinkHandler::LH_OK;
}

RvLinkHandler LinkHandler::updateVsts()
{
    QString filePathSoTmpl;
    QString filePathSoDest;

    for (auto vstBucket : *mVstBuckets) {
        if (   (vstBucket.status == VstStatus::No_So)
            || (vstBucket.status == VstStatus::Mismatch)
            || (vstBucket.status == VstStatus::NoBridge)) {
            if (prf.bridgeEnabled(vstBucket.bridge)) {
                filePathSoTmpl = prf.getPathSoTmplBridge(vstBucket.bridge);
                filePathSoDest = vstBucket.vstPath.left(
                            vstBucket.vstPath.size() - mMapVstExtLen.value(vstBucket.vstType)) + ".so";
                if (!QFile::remove(filePathSoDest)) {
                    qDebug() << "(LH): updateVsts(): remove file failed";
                }
                if (!QFile::copy(filePathSoTmpl, filePathSoDest)) {
                    qDebug() << "(LH): updateVsts(): copy file failed";
                }
            } else {
                vstBucket.status = VstStatus::NoBridge;
                continue;
            }
        }
    }

    /* Refresh everything even if selectively would strictly be enough.
     * Reason: From a user perspective it makes more sense that if, for instance,
     *         a VST is 'NotFound' and the user fixes it, then after performing
     *         an update action, the status should be "fixed" as well
     *         (meaning being updated to what is actually the case).
     */
    refreshStatus();

    return RvLinkHandler::LH_OK;
}

RvLinkHandler LinkHandler::enableVst(int idx)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QString filePathSoSrc;
    QString filePathLinkDest;
    QDir linkFolder(prf.getPathLinkFolder());

    // Enable a currently disabled VST
    if ((*mVstBuckets).at(idx).status == VstStatus::Disabled) {
        filePathSoSrc = (*mVstBuckets).at(idx).vstPath.left(
                    (*mVstBuckets).at(idx).vstPath.size() - mMapVstExtLen.value((*mVstBuckets).at(idx).vstType)) + ".so";
        filePathLinkDest = prf.getPathLinkFolder() + "/" + (*mVstBuckets).at(idx).name + ".so";

        // Sanity check: Ensure that the link folder actually exists before attempting to make a softlink
        // (Should never be the case unless the folder got deleted after it was selected in preferences dialog)
        if (!linkFolder.exists()) {
            qDebug() << "(LH): enableVst(): link folder doesn't exist";
            retVal = RvLinkHandler::LH_NOT_OK;
        } else {
            if (!QFile::link(filePathSoSrc, filePathLinkDest)) {
                qDebug() << "(LH): enableVst(): making link failed";
                retVal = RvLinkHandler::LH_NOT_OK;
            }
        }
    } else {
        retVal = RvLinkHandler::LH_IGNORED;
    }

    if (retVal == RvLinkHandler::LH_OK) {
        (*mVstBuckets)[idx].status = VstStatus::Enabled;
    }

    return retVal;
}

RvLinkHandler LinkHandler::disableVst(int idx)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QFileInfo filePathLinkDest;

    // Disable a currently enabled VST
    if ((*mVstBuckets).at(idx).status == VstStatus::Enabled) {
        filePathLinkDest.setFile(prf.getPathLinkFolder() + "/" + (*mVstBuckets).at(idx).name + ".so");

        if (!filePathLinkDest.isSymLink()) {
            qDebug() << "(LH): disableVst(): not a symlink";
            retVal = RvLinkHandler::LH_NOT_OK;
        } else {
            if (!QFile::remove(filePathLinkDest.filePath())) {
                qDebug() << "(LH): disableVst(): link delete failed";
                retVal = RvLinkHandler::LH_NOT_OK;
            }
        }
    } else {
        retVal = RvLinkHandler::LH_IGNORED;
    }

    if (retVal == RvLinkHandler::LH_OK) {
        (*mVstBuckets)[idx].status = VstStatus::Disabled;
    }

    return retVal;
}

RvLinkHandler LinkHandler::blacklistVst(int idx)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QFileInfo filePathSoSrc;
    QFileInfo filePathLinkDest;

    // Blacklisting means removing both the softlink in link folder and the so-file alongside the VST.
    if ((*mVstBuckets).at(idx).status != VstStatus::Blacklisted) {
        filePathSoSrc = (*mVstBuckets).at(idx).vstPath.left(
                    (*mVstBuckets).at(idx).vstPath.size() - mMapVstExtLen.value((*mVstBuckets).at(idx).vstType)) + ".so";
        filePathLinkDest.setFile(prf.getPathLinkFolder() + "/" + (*mVstBuckets).at(idx).name + ".so");

        if (!filePathLinkDest.isSymLink()) {
            qDebug() << "(LH): blacklisteVst(): not a symlink";
        } else {
            if (!QFile::remove(filePathLinkDest.filePath())) {
                qDebug() << "(LH): blacklisteVst(): link delete failed";
                retVal = RvLinkHandler::LH_NOT_OK;
            }
        }

        if (!filePathSoSrc.exists()) {
            qDebug() << "(LH): blacklisteVst(): filePathSoSrc doesn't exist";
        } else {
            if (!QFile::remove(filePathSoSrc.filePath())) {
                qDebug() << "(LH): blacklisteVst(): filePathSoSrc delete failed";
                retVal = RvLinkHandler::LH_NOT_OK;
            }
        }
    } else {
        retVal = RvLinkHandler::LH_IGNORED;
    }

    if (retVal == RvLinkHandler::LH_OK) {
        (*mVstBuckets)[idx].status = VstStatus::Blacklisted;
    }

    return retVal;
}

RvLinkHandler LinkHandler::changeBridge(int idx, VstBridge newBridgeType)
{
    /* Basically:
     * 1) Remove old so-file alongside VST
     * 2) Add new so-file alongside VST
     * The symlink should not be affected by this and should still point
     * to the location of the new so-file afterwards (due to same name).
     */
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QFileInfo filePathSoDest;
    QString filePathSoTmpl;

    // Attempt to change bridge only in certain states that "make sense".
    if (   (*mVstBuckets).at(idx).status == VstStatus::Enabled
        || (*mVstBuckets).at(idx).status == VstStatus::Disabled
        || (*mVstBuckets).at(idx).status == VstStatus::NoBridge
        || (*mVstBuckets).at(idx).status == VstStatus::No_So) {

        filePathSoDest = (*mVstBuckets).at(idx).vstPath.left(
                    (*mVstBuckets).at(idx).vstPath.size() - mMapVstExtLen.value((*mVstBuckets).at(idx).vstType)) + ".so";

        // Could be that the "old so-file" doesn't exist
        if (filePathSoDest.exists()) {
            if (!QFile::remove(filePathSoDest.filePath())) {
                qDebug() << "(LH): changeBridge(): old filePathSoDest delete failed";
                retVal = RvLinkHandler::LH_NOT_OK;
            }
        }

        // Create the new so-file based on the new bridge type
        filePathSoTmpl = prf.getPathSoTmplBridge(newBridgeType);
        if (!QFile::copy(filePathSoTmpl, filePathSoDest.filePath())) {
            qDebug() << "(LH): changeBridge(): new filePathSoDest copy failed";
            retVal = RvLinkHandler::LH_NOT_OK;
        }
    } else {
        retVal = RvLinkHandler::LH_IGNORED;
    }

    if (retVal == RvLinkHandler::LH_OK) {
        (*mVstBuckets)[idx].bridge = newBridgeType;
    }

    return retVal;
}


QStringList LinkHandler::checkForOrphans()
{
    QStringList filePathsOrphans;
    QDir linkFolder(prf.getPathLinkFolder());

    // Filter for file "*.so" files including broken softlinks
    linkFolder.setNameFilters(QStringList() << "*.so");
    linkFolder.setFilter(QDir::AllEntries | QDir::System);
    QStringList strListSoFiles = linkFolder.entryList();

    QFileInfo fileInfo;
    for (int i=0; i < strListSoFiles.size(); i++) {
        fileInfo.setFile(linkFolder.path() + "/" + strListSoFiles.at(i));
        // Qt docs: "Note: If the file is a symlink that points to a non-existing file, false is returned."
        // -->> Exactly what we want.
        bool isOrphan = false;
        if (!fileInfo.exists()) {
            // softlink is pointing nowhere
            isOrphan = true;
        } else {
            // softlink is pointing somewhere
            /* Furthermore: Any softlinks that are present in link folder
             * but aren't "tracked" by LinVstManager should be cleaned up as well
             * in order to avoid confusing situations. (i.e. DAW loading a potentially
             * version mismatching VST that actually isn't showing up in LinVstManager) */

            // Contruct "*.so"-paths of all currently tracked VSTs
            QStringList targetSoPaths;
            for (auto vstBucket : *mVstBuckets) {
                if (vstBucket.vstType == VstType::VST2) {
                    // Chop extension "dll"
                    targetSoPaths.append(vstBucket.vstPath.chopped(3) + "so");
                } else { // VST3
                    // Chop extension "vst3"
                    targetSoPaths.append(vstBucket.vstPath.chopped(4) + "so");
                }
            }

            // Check if the current softlink points to any one of them
            isOrphan = true;
            QString softlinkTarget = fileInfo.symLinkTarget();
            for (int i=0; i < targetSoPaths.size(); i++) {
                if (targetSoPaths.at(i) == softlinkTarget) {
                    // Softlink actually refers to tracked VST. Therefore no orphan
                    isOrphan = false;
                    break;
                }
            }
        }

        if (isOrphan) {
            filePathsOrphans.append(fileInfo.filePath());
        }
    }
    return filePathsOrphans;
}

RvLinkHandler LinkHandler::removeOrphans(const QStringList &filePathsOrphans)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;

    for (const auto &orphanPath : filePathsOrphans) {
        if (!QFile::remove(orphanPath)) {
            qDebug() << "(LH): removeOrphans(): link delete failed for: '" << orphanPath << "'";
            retVal = RvLinkHandler::LH_NOT_OK;
        }
    }

    return retVal;
}

bool LinkHandler::checkSoFileMatch(const QString &filePathA, const QString &filePathB)
{
    return QProcess::execute("diff", QStringList() << filePathA << filePathB) == 0;
}

