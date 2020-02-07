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
    ScanResult(QString t_name, QString t_vstPath, VstType t_vstType, bool t_verified, QByteArray t_pathHash, QByteArray t_soFileHash, bool t_selected);
    ScanResult() = default;
    ~ScanResult() = default;
    ScanResult(const ScanResult &) = default;
    QString name;
    QString vstPath;
    VstType vstType;
    bool verified;
    QByteArray pathHash;
    QByteArray soFileHash;
    bool selected;
};
Q_DECLARE_METATYPE(ScanResult);

#endif // SCANRESULT_H
