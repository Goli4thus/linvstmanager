// This file is part of LinVstManager.

#ifndef SCANRESULT_H
#define SCANRESULT_H

#include <QString>
#include <QByteArray>
#include "enums.h"
#include <QMetaType>

class ScanResult
{
public:
    ScanResult(QString t_name, QString t_vstPath, VstType t_vstType, bool t_verified, QByteArray t_hash, bool t_selected);
    ScanResult() = default;
    ~ScanResult() = default;
    ScanResult(const ScanResult &) = default;
    QString name;
    QString vstPath;
    VstType vstType;
    bool verified;
    QByteArray hash;
    bool selected;
};
Q_DECLARE_METATYPE(ScanResult);

#endif // SCANRESULT_H
