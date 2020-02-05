// This file is part of LinVstManager.

#include "pathhasher.h"
#include <QCryptographicHash>
#include <QString>

PathHasher::PathHasher()
{
    mHasher = new QCryptographicHash(QCryptographicHash::Sha1);
}

PathHasher::~PathHasher()
{
    delete mHasher;
}

QByteArray PathHasher::calcFilepathHash(QString filepath)
{
    // Calculate sha1-hash of filepath_VstDll
    mHasher->reset();
    mHasher->addData(filepath.toUtf8());
    return mHasher->result();
}
