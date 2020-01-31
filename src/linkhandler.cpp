#include "linkhandler.h"
#include "preferences.h"
#include <qdebug.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStringList>

LinkHandler::LinkHandler(const Preferences &t_prf, QList<VstBucket> *pVstBuckets, QObject *parent) : QObject(parent), prf(t_prf)
{
    mMapVstExtLen.insert(VstType::VST2, 4); // ".dll"
    mMapVstExtLen.insert(VstType::VST3, 5); // ".vst3"
    mVstBuckets = pVstBuckets;
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

    for (int i=0; i < mVstBuckets->size(); i++) {
        if (   ((*mVstBuckets).at(i).status == VstStatus::No_So)
            || ((*mVstBuckets).at(i).status == VstStatus::Mismatch)
            || ((*mVstBuckets).at(i).status == VstStatus::NoBridge)) {
            if (prf.bridgeEnabled((*mVstBuckets).at(i).bridge)) {
                filePathSoTmpl = prf.getPathSoTmplBridge((*mVstBuckets).at(i).bridge);
                filePathSoDest = (*mVstBuckets).at(i).vstPath.left(
                            (*mVstBuckets).at(i).vstPath.size() - mMapVstExtLen.value((*mVstBuckets).at(i).vstType)) + ".so";
                if (!QFile::remove(filePathSoDest)) {
                    qDebug() << "(LH): updateVsts(): remove file failed";
                }
                if (!QFile::copy(filePathSoTmpl, filePathSoDest)) {
                    qDebug() << "(LH): updateVsts(): copy file failed";
                }
            } else {
                (*mVstBuckets)[i].status = VstStatus::NoBridge;
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
    linkFolder.setNameFilters(QStringList() << "*.so");
    linkFolder.setFilter(QDir::AllEntries | QDir::System);
    QStringList strListSoFiles = linkFolder.entryList();
    QFileInfo fileInfo;
    for (int i=0; i < strListSoFiles.size(); i++) {
        fileInfo.setFile(linkFolder.path() + "/" + strListSoFiles.at(i));
        // Qt docs: "Note: If the file is a symlink that points to a non-existing file, false is returned."
        // -->> Exactly what we want.
        if (!fileInfo.exists()) {
            // symlink seems to be pointing nowhere
            // Cross check if it doesn't relate to any entry in vst list
            bool isOrphan = true;
            QString imaginarySoFile;
            for (int k=0; k < mVstBuckets->size(); k++) {
                // Actually check if symlink points at an imaginary ".so" file alongside any of the tracked vstPaths
                imaginarySoFile = (*mVstBuckets).at(k).vstPath.left(
                            (*mVstBuckets).at(k).vstPath.size() - mMapVstExtLen.value((*mVstBuckets).at(k).vstType)) + ".so";
                if (imaginarySoFile == fileInfo.symLinkTarget()) {
                    isOrphan = false;
                    break;
                }
            }

            if (isOrphan) {
                filePathsOrphans.append(fileInfo.filePath());
            }
        }
    }
    return filePathsOrphans;
}

RvLinkHandler LinkHandler::removeOrphans(QStringList filePathsOrphans)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;

    for (int i=0; i < filePathsOrphans.size(); i++) {
        if (!QFile::remove(filePathsOrphans.at(i))) {
            qDebug() << "(LH): removeOrphans(): link delete failed for: '" << filePathsOrphans.at(i) << "'";
            retVal = RvLinkHandler::LH_NOT_OK;
        }
    }

    return retVal;
}

bool LinkHandler::checkSoFileMatch(QString filePathA, QString filePathB)
{
    bool match;

    if (QProcess::execute("diff", QStringList() << filePathA << filePathB) == 0)
    {
        match = true;
    } else {
        match = false;
    }
    return match;
}

