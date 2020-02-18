// This file is part of LinVstManager.

#ifndef VSTBUCKET_H
#define VSTBUCKET_H

#include "enums.h"
#include <QString>

class VstBucket
{
public:
    VstBucket(QString pName,
              QString pVstPath,
              QByteArray pPathHash,
              QByteArray pSoFileHash,
              VstStatus pStatus,
              VstBridge pBridge,
              VstType pType,
              ArchType pBitType,
              bool pNewlyAdded);
    VstBucket();

    QString name;
    QString vstPath;
    QByteArray pathHash;
    QByteArray soFileHash;
    VstStatus status;
    VstBridge bridge;
    VstType vstType;
    ArchType archType;
    bool newlyAdded;
};

#endif // VSTBUCKET_H
