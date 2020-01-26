#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include <QObject>
#include <QMap>
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
    ~ConfigHandler();
private:
    QXmlStreamWriter *xmlWriter;
    QXmlStreamReader *xmlReader;
    void writePreferences(const Preferences &prf);
    void writeVstBuckets(const QList<VstBucket> &vstBuckets);
    quint8 readPreferences(Preferences &prf);
    quint8 readVstBucket(QList<VstBucket> &vstBuckets);
    QStringList prefBoolNames;
    QStringList prefPathNames;
    QMap<VstBridge, QString> mapBridgeStr;
    QMap<VstStatus, QString> mapStatusStr;
    QMap<VstType, QString> mapTypeStr;

signals:
    void configSaveDone(bool f_ErrorOnOpeningFile);

public slots:
    RvConfFile saveConfig(const Preferences &prf, const QList<VstBucket> &vstBuckets);
    RvConfFile loadConfig(Preferences &prf, QList<VstBucket> &vstBuckets);
};

#endif // CONFIGHANDLER_H
