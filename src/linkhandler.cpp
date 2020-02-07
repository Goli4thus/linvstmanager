// This file is part of LinVstManager.

#include "linkhandler.h"
#include "preferences.h"
#include <qdebug.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStringList>
#include "datahasher.h"
#include <QElapsedTimer>

LinkHandler::LinkHandler(const Preferences &t_prf, QVector<VstBucket> *pVstBuckets, DataHasher &pDataHasher, QObject *parent)
    : QObject(parent), prf(t_prf), dataHasher(pDataHasher)
{
    mVstBuckets = pVstBuckets;
}

LinkHandler::~LinkHandler()
{
}

RvLinkHandler LinkHandler::refreshStatus(bool refreshSingle, int singleIndex, bool updateSoFileHash)
{
    QFileInfo fileInfoVst;
    QFileInfo fileInfoSo;
    QFileInfo fileInfoLink;
    QString filePathSoTmpl;
    QVector<int> indexOfVstBuckets;

    if (refreshSingle) {
        indexOfVstBuckets.append(singleIndex);
    } else {
        for (int i=0; i < mVstBuckets->size(); i++) {
            indexOfVstBuckets.append(i);
        }
    }

    for(const auto &index : indexOfVstBuckets) {
        auto &vstBucket = (*mVstBuckets)[index];
        if (vstBucket.status == VstStatus::Blacklisted) {
            continue;
        } else {
            fileInfoVst.setFile(vstBucket.vstPath);

            if (fileInfoVst.exists()) {
                QString filePathSo;
                if (vstBucket.vstType == VstType::VST2) {
                    filePathSo = vstBucket.vstPath.chopped(3) + "so"; // Replace "dll"
                } else { // VST3
                    filePathSo = vstBucket.vstPath.chopped(4) + "so"; // Replace "vst3"
                }
                fileInfoSo.setFile(filePathSo);

                if (!prf.bridgeEnabled(vstBucket.bridge)) {
                    vstBucket.status = VstStatus::NoBridge;
                    continue;
                } else {
                    if (fileInfoSo.exists()) {
                        fileInfoLink.setFile(prf.getPathLinkFolder() + "/" + vstBucket.name + ".so");

                        // Calculate soFileHash, if actually needed (i.e. after startup)
                        if (updateSoFileHash) {
                            vstBucket.soFileHash = dataHasher.calcFiledataHash(filePathSo);
                        }

                        filePathSoTmpl = prf.getPathSoTmplBridge(vstBucket.bridge);

                        if (checkSoHashMatch(vstBucket.soFileHash, vstBucket.bridge)) {
                            // Qt docs re QFileInfo::exists():
                            // "Note: If the file is a symlink that points to a non-existing file, false is returned."
                            // -->> No problem in this case, as we already made sure the source so-file exists.
                            if (fileInfoLink.exists()) {
                                vstBucket.status = VstStatus::Enabled;
                            } else {
                                vstBucket.status = VstStatus::Disabled;
                            }
                        } else {
                            vstBucket.status = VstStatus::Mismatch;
                        }
                    } else {
                        vstBucket.status = VstStatus::No_So;
                    }
                }
            } else {
                vstBucket.status = VstStatus::NotFound;
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
                if (vstBucket.vstType == VstType::VST2) {
                    filePathSoDest = vstBucket.vstPath.chopped(3) + "so"; // Replace "dll"
                } else { // VST3
                    filePathSoDest = vstBucket.vstPath.chopped(4) + "so"; // Replace "vst3"
                }
                if (QFile::exists(filePathSoDest)) {
                    if (!QFile::remove(filePathSoDest)) {
                        qDebug() << "(LH): updateVsts(): remove file failed";
                    }
                }
                if (!QFile::copy(filePathSoTmpl, filePathSoDest)) {
                    qDebug() << "(LH): updateVsts(): copy file failed";
                } else {
                    // Update soFile hash by simple copy of template hash
                    vstBucket.soFileHash = dataHasher.getHashSoTmplBridge(vstBucket.bridge);
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
    refreshStatus(false, 0, true);

    return RvLinkHandler::LH_OK;
}

RvLinkHandler LinkHandler::enableVst(const QVector<int> &indexOfVstBuckets)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QString filePathSoSrc;
    QString filePathLinkDest;
    QDir linkFolder(prf.getPathLinkFolder());

    // Sanity check: Ensure that the link folder actually exists before attempting to make a softlink
    // (Should never be the case unless the folder got deleted after it was selected in preferences dialog)
    if (!linkFolder.exists()) {
        qDebug() << "(LH): enableVst(): link folder doesn't exist";
        retVal = RvLinkHandler::LH_NOT_OK;
    }

    // Enable a currently disabled VST
    for(const auto &index : indexOfVstBuckets) {
//        const auto &vstBucket = (*mVstBuckets).at(index);
        auto &vstBucket = (*mVstBuckets)[index];
        if (vstBucket.status == VstStatus::Disabled) {
            if (vstBucket.vstType == VstType::VST2) {
                filePathSoSrc = vstBucket.vstPath.chopped(3) + "so"; // Replace "dll"
            } else { // VST3
                filePathSoSrc = vstBucket.vstPath.chopped(4) + "so"; // Replace "vst3"
            }

            filePathLinkDest = prf.getPathLinkFolder() + "/" + vstBucket.name + ".so";

            if (!QFile::link(filePathSoSrc, filePathLinkDest)) {
                qDebug() << "(LH): enableVst(): making link failed (errno: " << strerror(errno) << ") (index: " << index << ")";
                retVal = RvLinkHandler::LH_NOT_OK;
            } else {
                vstBucket.status = VstStatus::Enabled;
            }
        }
    }

    return retVal;
}

RvLinkHandler LinkHandler::disableVst(const QVector<int> &indexOfVstBuckets)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QFileInfo filePathLinkDest;

    // Disable a currently enabled VST
    for(const auto &index : indexOfVstBuckets) {
        auto &vstBucket = (*mVstBuckets)[index];
        if (vstBucket.status == VstStatus::Enabled) {
            filePathLinkDest.setFile(prf.getPathLinkFolder() + "/" + vstBucket.name + ".so");

            if (!filePathLinkDest.isSymLink()) {
                qDebug() << "(LH): disableVst(): not a symlink";
                retVal = RvLinkHandler::LH_NOT_OK;
            } else {
                if (!QFile::remove(filePathLinkDest.filePath())) {
                    qDebug() << "(LH): disableVst(): link delete failed";
                    retVal = RvLinkHandler::LH_NOT_OK;
                } else {
                    vstBucket.status = VstStatus::Disabled;
                }
            }
        }
    }

    return retVal;
}

RvLinkHandler LinkHandler::blacklistVst(const QVector<int> &indexOfVstBuckets)
{
    RvLinkHandler retVal = RvLinkHandler::LH_OK;
    QFileInfo filePathSoSrc;
    QFileInfo filePathLinkDest;

    // Blacklisting means removing both the softlink in link folder and the so-file alongside the VST.
    for(const auto &index : indexOfVstBuckets) {
        auto &vstBucket = (*mVstBuckets)[index];
        if (vstBucket.status != VstStatus::Blacklisted) {
            if (vstBucket.vstType == VstType::VST2) {
                filePathSoSrc = vstBucket.vstPath.chopped(3) + "so"; // Replace "dll"
            } else { // VST3
                filePathSoSrc = vstBucket.vstPath.chopped(4) + "so"; // Replace "vst3"
            }

            filePathLinkDest.setFile(prf.getPathLinkFolder() + "/" + vstBucket.name + ".so");

            if (filePathLinkDest.exists()) {
                if (!filePathLinkDest.isSymLink()) {
                    qDebug() << "(LH): blacklisteVst(): not a symlink";
                } else {
                    if (!QFile::remove(filePathLinkDest.filePath())) {
                        qDebug() << "(LH): blacklisteVst(): link delete failed";
                        retVal = RvLinkHandler::LH_NOT_OK;
                    }
                }
            }

            if (filePathSoSrc.exists()) {
                if (!QFile::remove(filePathSoSrc.filePath())) {
                    qDebug() << "(LH): blacklisteVst(): filePathSoSrc delete failed";
                    retVal = RvLinkHandler::LH_NOT_OK;
                } else {
                    vstBucket.status = VstStatus::Blacklisted;
                }
            }
        }
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

    auto &vstBucket = (*mVstBuckets)[idx];
    // Attempt to change bridge only in certain states that "make sense".
    if (   vstBucket.status == VstStatus::Enabled
        || vstBucket.status == VstStatus::Disabled
        || vstBucket.status == VstStatus::NoBridge
        || vstBucket.status == VstStatus::No_So) {

        if (vstBucket.vstType == VstType::VST2) {
            filePathSoDest = vstBucket.vstPath.chopped(3) + "so"; // Replace "dll"
        } else { // VST3
            filePathSoDest = vstBucket.vstPath.chopped(4) + "so"; // Replace "vst3"
        }

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
        } else {
            // Update soFile hash by simple copy of template hash
            vstBucket.soFileHash = dataHasher.getHashSoTmplBridge(vstBucket.bridge);
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
            for (const auto &vstBucket : *mVstBuckets) {
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

bool LinkHandler::checkSoHashMatch(const QByteArray &soFileHash, const VstBridge vstBridge)
{
    // Compare hash values of soFile and soTmplFile
    return soFileHash.compare(dataHasher.getHashSoTmplBridge(vstBridge)) == 0;
}

