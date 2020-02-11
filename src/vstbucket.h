// This file is part of LinVstManager.

#ifndef VSTBUCKET_H
#define VSTBUCKET_H

#include "enums.h"
#include <QString>

class VstBucket
{
public:
    VstBucket(QString t_name,
              QString t_vstPath,
              QByteArray t_pathHash,
              QByteArray t_soFileHash,
              VstStatus t_status,
              VstBridge t_bridge,
              VstType t_type,
              bool t_newlyAdded);
    VstBucket();

    QString name;
    QString vstPath;
    QByteArray pathHash;
    QByteArray soFileHash;
    VstStatus status;
    VstBridge bridge;
    VstType vstType;
    bool newlyAdded;
};

#endif // VSTBUCKET_H
