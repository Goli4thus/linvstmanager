// This file is part of LinVstManager.

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
    void writeVstBuckets(const QVector<VstBucket> &vstBuckets);
    quint8 readPreferences(Preferences &prf, QString configVersion);
    quint8 readVstBucket(QVector<VstBucket> &vstBuckets);
    QStringList prefBoolNames;
    QStringList prefPathNames;
    QMap<VstBridge, QString> mapBridgeStr;
    QMap<VstStatus, QString> mapStatusStr;
    QMap<VstType, QString> mapVstTypeStr;
    QMap<ArchType, QString> mapArchTypeStr;
    QString mConfigVersionLatest;
    QStringList mConfigVersionHistory;
    QString mConfigVersionLoaded;
    QVector<bool> mPrefBoolNamesOfInterest_V1_2;
    QVector<bool> mPrefPathNamesOfInterest_V1_0;
    QVector<bool> mPrefPathNamesOfInterest_V1_1;
    QVector<bool> mPrefPathNamesOfInterest_V1_2;

signals:
    void configSaveDone(bool f_ErrorOnOpeningFile);

public slots:
    RvConfFile saveConfig(const Preferences &prf, const QVector<VstBucket> &vstBuckets);
    RvConfFile loadConfig(Preferences &prf, QVector<VstBucket> &vstBuckets);
};

#endif // CONFIGHANDLER_H
