#include "scanresult.h"

ScanResult::ScanResult(QString t_name,
                       QString t_vstPath,
                       VstType t_vstType,
                       QByteArray t_hash,
                       bool t_selected)
{
    name = t_name;
    vstPath = t_vstPath;
    vstType = t_vstType;
    hash = t_hash;
    selected = t_selected;
}
