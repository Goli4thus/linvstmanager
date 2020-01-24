#include "vstbucket.h"

VstBucket::VstBucket(QString t_name,
                     QString t_dllPath,
                     QByteArray t_hash,
                     VstStatus t_status,
                     VstBridge t_bridge,
                     VstType t_type,
                     bool t_newlyAdded)
{
    name = t_name;
    dllPath = t_dllPath;
    hash = t_hash;
    status = t_status;
    bridge = t_bridge;
    vstType = t_type;
    newlyAdded = t_newlyAdded;
}
