#include "linkhandler.h"
#include "preferences.h"
#include <qdebug.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStringList>

LinkHandler::LinkHandler(const Preferences &t_prf, QObject *parent) : QObject(parent), prf(t_prf)
{
    mMapVstExtLen.insert(VstType::VST2, 4); // ".dll"
    mMapVstExtLen.insert(VstType::VST3, 5); // ".vst3"
}

RvLinkHandler LinkHandler::refreshStatus(QList<VstBucket> &pVstBuckets, bool refreshSingle, int singleIndex)
{
    QFileInfo fileInfoVst;
    QFileInfo fileInfoSo;
    QFileInfo fileInfoLink;
    QString filePathSoTmpl;

    for (int i=0; i < pVstBuckets.size(); i++) {
        if (refreshSingle && (i != singleIndex)) {
            continue;
        } else {
            if (pVstBuckets.at(i).status == VstStatus::Blacklisted) {
                continue;
            } else {
                fileInfoVst.setFile(pVstBuckets.at(i).vstPath);

                if (fileInfoVst.exists()) {
                    fileInfoSo.setFile(fileInfoVst.filePath().left(fileInfoVst.filePath().size() - mMapVstExtLen.value(pVstBuckets.at(i).vstType)) + ".so");

                    if (!prf.bridgeEnabled(pVstBuckets.at(i).bridge)) {
                        pVstBuckets[i].status = VstStatus::NoBridge;
                        continue;
                    } else {
                        if (fileInfoSo.exists()) {
                            fileInfoLink.setFile(prf.getPathLinkFolder() + "/" + pVstBuckets.at(i).name + ".so");


                            filePathSoTmpl = prf.getPathSoTmplBridge(pVstBuckets.at(i).bridge);

                            if (checkSoFileMatch(filePathSoTmpl, fileInfoSo.filePath())) {
                                // Qt docs re QFileInfo::exists():
                                // "Note: If the file is a symlink that points to a non-existing file, false is returned."
                                // -->> No problem in this case, as we already made sure the source so-file exists.
                                if (fileInfoLink.exists()) {
                                    pVstBuckets[i].status = VstStatus::Enabled;
                                } else {
                                    pVstBuckets[i].status = VstStatus::Disabled;
                                }
                            } else {
                                pVstBuckets[i].status = VstStatus::Mismatch;
                            }
                        } else {
                            pVstBuckets[i].status = VstStatus::No_So;
                        }
                    }
                } else {
                    pVstBuckets[i].status = VstStatus::NotFound;
                }
            }
        }
    }

    return RvLinkHandler::LH_OK;
}

RvLinkHandler LinkHandler::updateVsts(QList<VstBucket> &pVstBuckets)
{
    QString filePathSoTmpl;
    QString filePathSoDest;

    for (int i=0; i < pVstBuckets.size(); i++) {
        if (   (pVstBuckets.at(i).status == VstStatus::No_So)
            || (pVstBuckets.at(i).status == VstStatus::Mismatch)
            || (pVstBuckets.at(i).status == VstStatus::NoBridge)) {
            if (prf.bridgeEnabled(pVstBuckets.at(i).bridge)) {
                filePathSoTmpl = prf.getPathSoTmplBridge(pVstBuckets.at(i).bridge);
                filePathSoDest = pVstBuckets.at(i).vstPath.left(pVstBuckets.at(i).vstPath.size() - mMapVstExtLen.value(pVstBuckets.at(i).vstType)) + ".so";
                if (!QFile::remove(filePathSoDest)) {
                    qDebug() << "(LH): updateVsts(): remove file failed";
                }
                if (!QFile::copy(filePathSoTmpl, filePathSoDest)) {
                    qDebug() << "(LH): updateVsts(): copy file failed";
                }
            } else {
                pVstBuckets[i].status = VstStatus::NoBridge;
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
    refreshStatus(pVstBuckets);

    return RvLinkHandler::LH_OK;
}

RvLinkHandler LinkHandler::enableVst(VstBucket &pVstBucket)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QString filePathSoSrc;
    QString filePathLinkDest;
    QDir linkFolder(prf.getPathLinkFolder());

    // Enable a currently disabled VST
    if (pVstBucket.status == VstStatus::Disabled) {
        filePathSoSrc = pVstBucket.vstPath.left(pVstBucket.vstPath.size() - mMapVstExtLen.value(pVstBucket.vstType)) + ".so";
        filePathLinkDest = prf.getPathLinkFolder() + "/" + pVstBucket.name + ".so";

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
        pVstBucket.status = VstStatus::Enabled;
    }

    return retVal;
}

RvLinkHandler LinkHandler::disableVst(VstBucket &pVstBucket)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QFileInfo filePathLinkDest;

    // Disable a currently enabled VST
    if (pVstBucket.status == VstStatus::Enabled) {
        filePathLinkDest.setFile(prf.getPathLinkFolder() + "/" + pVstBucket.name + ".so");

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
        pVstBucket.status = VstStatus::Disabled;
    }

    return retVal;
}

RvLinkHandler LinkHandler::blacklistVst(VstBucket &pVstBucket)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QFileInfo filePathSoSrc;
    QFileInfo filePathLinkDest;

    // Blacklisting means removing both the softlink in link folder and the so-file alongside the VST.
    if (pVstBucket.status != VstStatus::Blacklisted) {
        filePathSoSrc = pVstBucket.vstPath.left(pVstBucket.vstPath.size() - mMapVstExtLen.value(pVstBucket.vstType)) + ".so";
        filePathLinkDest.setFile(prf.getPathLinkFolder() + "/" + pVstBucket.name + ".so");

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
        pVstBucket.status = VstStatus::Blacklisted;
    }

    return retVal;
}

RvLinkHandler LinkHandler::changeBridge(VstBucket &pVstBucket, VstBridge newBridgeType)
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
    if (   pVstBucket.status == VstStatus::Enabled
        || pVstBucket.status == VstStatus::Disabled
        || pVstBucket.status == VstStatus::NoBridge
        || pVstBucket.status == VstStatus::No_So) {

        filePathSoDest = pVstBucket.vstPath.left(pVstBucket.vstPath.size() - mMapVstExtLen.value(pVstBucket.vstType)) + ".so";

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
        pVstBucket.bridge = newBridgeType;
    }

    return retVal;
}

RvLinkHandler LinkHandler::checkForOrphans(QList<VstBucket> &pVstBuckets)
{
    /* When to call:
     * - once at startup
     *    -->> possibly inform user about possibility of cleanup
     */

    /* Basically make use of:
     * - QFileInfo::isSymLink(linkPath): to detect all symlinks in link folder
     * - QFileInfo::exists(linkPath): returns false if target doesn't exists; meaining the link is an orphan
     */
//    QFileInfo fileInfoLink;

//    for (int i=0; i < pVstBuckets.size(); i++) {
//        fileInfoLink.setFile(pVstBuckets.at(i).vstPath);
    //    }
}

RvLinkHandler LinkHandler::removeOrphans()
{

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

