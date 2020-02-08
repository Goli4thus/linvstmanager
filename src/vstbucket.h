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
              QString pLinkSalt,
              VstStatus pStatus,
              VstBridge pBridge,
              VstType pType,
              bool pNewlyAdded);

    QString name;
    QString vstPath;
    QByteArray pathHash;
    QByteArray soFileHash;
    QString linkSalt;
    VstStatus status;
    VstBridge bridge;
    VstType vstType;
    bool newlyAdded;
};

#endif // VSTBUCKET_H
