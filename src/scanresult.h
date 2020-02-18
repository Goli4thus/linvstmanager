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
    ScanResult(QString pName,
               QString pVstPath,
               VstType pVstType,
               ArchType pArchType,
               VstProbabilityType pProbType,
               bool pVerified,
               QByteArray pPathHash,
               QByteArray pSoFileHash,
               bool pSelected);
    ScanResult() = default;
    ~ScanResult() = default;
    ScanResult(const ScanResult &) = default;
    QString name;
    QString vstPath;
    VstType vstType;
    ArchType archType;
    VstProbabilityType probType;
    bool verified;
    QByteArray pathHash;
    QByteArray soFileHash;
    bool selected;
};
Q_DECLARE_METATYPE(ScanResult);

#endif // SCANRESULT_H
