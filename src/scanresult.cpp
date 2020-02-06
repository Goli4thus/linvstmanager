// This file is part of LinVstManager.

#include "scanresult.h"

ScanResult::ScanResult(QString t_name,
                       QString t_vstPath,
                       VstType t_vstType,
                       bool t_verified,
                       QByteArray t_hash,
                       bool t_selected)
{
    name = std::move(t_name);
    vstPath = std::move(t_vstPath);
    vstType = t_vstType;
    verified = t_verified;
    hash = std::move(t_hash);
    selected = t_selected;
}
