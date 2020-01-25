#ifndef SCANRESULT_H
#define SCANRESULT_H

#include <QString>

class ScanResult
{
public:
    ScanResult(QString t_name, QString t_vstPath, QByteArray t_hash, bool t_selected);
    QString name;
    QString vstPath;
    QByteArray hash;
    bool selected;
};

#endif // SCANRESULT_H
