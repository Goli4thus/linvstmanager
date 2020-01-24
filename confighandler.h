#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include <QObject>
class QXmlStreamReader;
class QXmlStreamWriter;

#include "enums.h"
class Preferences;
class VstBucket;

class ConfigHandler : public QObject
{
    Q_OBJECT
public:
    explicit ConfigHandler(QObject *parent = nullptr);
private:
    QXmlStreamWriter *xmlWriter;
    QXmlStreamReader *xmlReader;
    void writePreferences(const Preferences &prf);
    void writeVstBuckets(const QList<VstBucket> &vstBuckets);
    quint8 readPreferences(Preferences &prf);
    quint8 readVstBucket(QList<VstBucket> &vstBuckets);

signals:
    void configSaveDone(bool f_ErrorOnOpeningFile);

public slots:
    RvConfFile saveConfig(const Preferences &prf, const QList<VstBucket> &vstBuckets);
    RvConfFile loadConfig(Preferences &prf, QList<VstBucket> &vstBuckets);
};

#endif // CONFIGHANDLER_H
