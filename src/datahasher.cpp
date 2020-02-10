// This file is part of LinVstManager.

#include "datahasher.h"
#include <QCryptographicHash>
#include <QString>
#include <QFile>

DataHasher::DataHasher()
{
    mHasher = new QCryptographicHash(QCryptographicHash::Sha1);

    QByteArray init;
    mMapSoTmplHash.insert(VstBridge::LinVst, init);
    mMapSoTmplHash.insert(VstBridge::LinVstX, init);
    mMapSoTmplHash.insert(VstBridge::LinVst3, init);
    mMapSoTmplHash.insert(VstBridge::LinVst3X, init);
}

DataHasher::~DataHasher()
{
    delete mHasher;
}

QByteArray DataHasher::calcFilepathHash(const QString &filepath)
{
    // Calculate sha1-hash of filepath_VstDll
    mHasher->reset();
    mHasher->addData(filepath.toUtf8());
    return mHasher->result();
}

QByteArray DataHasher::calcFiledataHash(const QString &filepath)
{
    // Calculate sha1-hash of filepath_VstDll
    QFile file(filepath);
    file.open(QIODevice::ReadOnly);
    mHasher->reset();
    mHasher->addData(&file);
    file.close();
    return mHasher->result();
}

void DataHasher::updateHashSoTmplBridge(const VstBridge bridgeType, const QString bridgeTmplPath)
{
    mMapSoTmplHash.remove(bridgeType);
    mMapSoTmplHash.insert(bridgeType, calcFiledataHash(bridgeTmplPath));
}

QByteArray DataHasher::getHashSoTmplBridge(const VstBridge bridgeType) const
{
    return mMapSoTmplHash.value(bridgeType);
}
