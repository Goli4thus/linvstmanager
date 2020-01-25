#include "scanresult.h"

ScanResult::ScanResult(QString t_name,
                       QString t_vstPath,
                       QByteArray t_hash,
                       bool t_selected)
{
    name = t_name;
    vstPath = t_vstPath;
    hash = t_hash;
    selected = t_selected;
}
