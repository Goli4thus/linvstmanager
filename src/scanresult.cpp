// This file is part of LinVstManager.

#include "scanresult.h"

ScanResult::ScanResult(QString pName,
                       QString pVstPath,
                       VstType pVstType,
                       ArchType pArchType,
                       VstProbabilityType pProbType,
                       bool pVerified,
                       QByteArray pPathHash,
                       QByteArray pSoFileHash,
                       bool pSelected)
{
    name = std::move(pName);
    vstPath = std::move(pVstPath);
    vstType = pVstType;
    archType = pArchType;
    probType = pProbType;
    verified = pVerified;
    pathHash = std::move(pPathHash);
    soFileHash = std::move(pSoFileHash);
    selected = pSelected;
}
