// This file is part of LinVstManager.

#include "vstbucket.h"

VstBucket::VstBucket(QString t_name,
                     QString t_vstPath,
                     QByteArray t_hash,
                     VstStatus t_status,
                     VstBridge t_bridge,
                     VstType t_type,
                     bool t_newlyAdded)
{
    name = t_name;
    vstPath = t_vstPath;
    hash = t_hash;
    status = t_status;
    bridge = t_bridge;
    vstType = t_type;
    newlyAdded = t_newlyAdded;
}
