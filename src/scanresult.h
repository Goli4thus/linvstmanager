// This file is part of LinVstManager.

#ifndef SCANRESULT_H
#define SCANRESULT_H

#include <QString>
#include "enums.h"

class ScanResult
{
public:
    ScanResult(QString t_name, QString t_vstPath, VstType t_vstType, QByteArray t_hash, bool t_selected);
    QString name;
    QString vstPath;
    VstType vstType;
    QByteArray hash;
    bool selected;
};

#endif // SCANRESULT_H
