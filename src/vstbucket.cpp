// This file is part of LinVstManager.

#include "vstbucket.h"

VstBucket::VstBucket(QString pName,
                     QString pVstPath,
                     QByteArray pPathHash,
                     QByteArray pSoFileHash,
                     VstStatus pStatus,
                     VstBridge pBridge,
                     VstType pType,
                     BitType pBitType,
                     bool pNewlyAdded)
{
    name = std::move(pName);
    vstPath = std::move(pVstPath);
    pathHash = std::move(pPathHash);
    soFileHash = std::move(pSoFileHash);
    status = pStatus;
    bridge = pBridge;
    vstType = pType;
    bitType = pBitType;
    newlyAdded = pNewlyAdded;
}

VstBucket::VstBucket()
{
    name = "tmp";
    vstPath = "tmpPath";
    pathHash = QByteArray();
    soFileHash = QByteArray();
    status = VstStatus::NA;
    bridge = VstBridge::LinVst;
    vstType = VstType::VST2;
    bitType = BitType::BitsNA;
    newlyAdded = false;
}
