// This file is part of LinVstManager.

#include "vstbucket.h"

VstBucket::VstBucket(QString t_name,
                     QString t_vstPath,
                     QByteArray t_pathHash,
                     QByteArray t_soFileHash,
                     VstStatus t_status,
                     VstBridge t_bridge,
                     VstType t_type,
                     bool t_newlyAdded)
{
    name = std::move(t_name);
    vstPath = std::move(t_vstPath);
    pathHash = std::move(t_pathHash);
    soFileHash = std::move(t_soFileHash);
    status = t_status;
    bridge = t_bridge;
    vstType = t_type;
    newlyAdded = t_newlyAdded;
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
    newlyAdded = false;
}
