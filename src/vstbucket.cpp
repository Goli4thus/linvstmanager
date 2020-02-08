// This file is part of LinVstManager.

#include "vstbucket.h"

VstBucket::VstBucket(QString pName,
                     QString pVstPath,
                     QByteArray pPathHash,
                     QByteArray pSoFileHash,
                     QString pLinkSalt,
                     VstStatus pStatus,
                     VstBridge pBridge,
                     VstType pType,
                     bool pNewlyAdded)
{
    name = std::move(pName);
    vstPath = std::move(pVstPath);
    pathHash = std::move(pPathHash);
    linkSalt = std::move(pLinkSalt);
    soFileHash = std::move(pSoFileHash);
    status = pStatus;
    bridge = pBridge;
    vstType = pType;
    newlyAdded = pNewlyAdded;
}
