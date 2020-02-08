// This file is part of LinVstManager.

#ifndef PATHHASHER_H
#define PATHHASHER_H

class QCryptographicHash;
class QByteArray;
class QString;
#include <QMap>
#include "enums.h"

class DataHasher
{
public:
    DataHasher();
    ~DataHasher();
    QByteArray calcFilepathHash(const QString &filepath);
    QByteArray calcFiledataHash(const QString &filepath);
    QString calcLinkSalt(const QByteArray &filePathHash);

    void updateHashSoTmplBridge(const VstBridge bridgeType, const QString &bridgeTmplPath);
    QByteArray getHashSoTmplBridge(const VstBridge bridgeType) const;

private:
    QCryptographicHash *mHasher;
    QMap<VstBridge, QByteArray> mMapSoTmplHash;
};

#endif // PATHHASHER_H
