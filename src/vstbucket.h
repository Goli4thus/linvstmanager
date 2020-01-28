#ifndef VSTBUCKET_H
#define VSTBUCKET_H

#include "enums.h"
#include <QString>

class VstBucket
{
public:
    VstBucket(QString t_name,
              QString t_vstPath,
              QByteArray t_hash,
              VstStatus t_status,
              VstBridge t_bridge,
              VstType t_type,
              bool t_newlyAdded);

    QString name;
    QString vstPath;
    QByteArray hash;
    VstStatus status;
    VstBridge bridge;
    VstType vstType;
    bool newlyAdded;
};

#endif // VSTBUCKET_H
