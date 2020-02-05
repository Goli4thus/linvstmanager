// This file is part of LinVstManager.

#ifndef PATHHASHER_H
#define PATHHASHER_H

class QCryptographicHash;
class QByteArray;
class QString;

class PathHasher
{
public:
    PathHasher();
    ~PathHasher();
    QByteArray calcFilepathHash(QString filepath);

private:
    QCryptographicHash *mHasher;
};

#endif // PATHHASHER_H
