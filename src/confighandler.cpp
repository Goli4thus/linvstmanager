// This file is part of LinVstManager.

#include "confighandler.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QElapsedTimer>

#include "preferences.h"
#include "vstbucket.h"
#include "modelvstbuckets.h"
#include "defines.h"


ConfigHandler::ConfigHandler(QObject *parent) : QObject(parent)
{
    xmlReader = new QXmlStreamReader();
    xmlWriter = new QXmlStreamWriter();
    xmlWriter->setAutoFormatting(true);

    prefBoolNames << "f_enabledLinVst"
                  << "f_enabledLinVstX"
                  << "f_enabledLinVst3"
                  << "f_enabledLinVst3X"
                  << "f_bridgeDefaultVst2IsX"
                  << "f_bridgeDefaultVst3IsX"
                  << "f_hideBlacklisted";

    prefPathNames << "pathSoLinVst"
                  << "pathSoLinVstX"
                  << "pathSoLinVst3"
                  << "pathSoLinVst3X"
                  << "pathLinkFolder"
                  << "pathCheckTool";

    mapBridgeStr.insert(VstBridge::LinVst,   "LinVst");
    mapBridgeStr.insert(VstBridge::LinVstX,  "LinVstX");
    mapBridgeStr.insert(VstBridge::LinVst3,  "LinVst3");
    mapBridgeStr.insert(VstBridge::LinVst3X, "LinVst3X");

    mapStatusStr.insert(VstStatus::Enabled,     "Enabled");
    mapStatusStr.insert(VstStatus::Disabled,    "Disabled");
    mapStatusStr.insert(VstStatus::Mismatch,    "Mismatch");
    mapStatusStr.insert(VstStatus::No_So,       "No_So");
    mapStatusStr.insert(VstStatus::NotFound,    "NotFound");
    mapStatusStr.insert(VstStatus::NoBridge,    "NoBridge");
    mapStatusStr.insert(VstStatus::Orphan,      "Orphan");
    mapStatusStr.insert(VstStatus::NA,          "NA");
    mapStatusStr.insert(VstStatus::Blacklisted, "Blacklisted");

    mapTypeStr.insert(VstType::VST2, "VST2");
    mapTypeStr.insert(VstType::VST3, "VST3");
}

ConfigHandler::~ConfigHandler()
{
    delete xmlReader;
    delete xmlWriter;
}

void ConfigHandler::writePreferences(const Preferences &prf)
{
    xmlWriter->writeTextElement(prefBoolNames.at(0), prf.bridgeEnabled(VstBridge::LinVst) ? ("true") : ("false"));
    xmlWriter->writeTextElement(prefBoolNames.at(1), prf.bridgeEnabled(VstBridge::LinVstX) ? ("true") : ("false"));
    xmlWriter->writeTextElement(prefBoolNames.at(2), prf.bridgeEnabled(VstBridge::LinVst3) ? ("true") : ("false"));
    xmlWriter->writeTextElement(prefBoolNames.at(3), prf.bridgeEnabled(VstBridge::LinVst3X) ? ("true") : ("false"));

    xmlWriter->writeTextElement(prefBoolNames.at(4), prf.getBridgeDefaultVst2IsX() ? ("true") : ("false"));
    xmlWriter->writeTextElement(prefBoolNames.at(5), prf.getBridgeDefaultVst3IsX() ? ("true") : ("false"));
    xmlWriter->writeTextElement(prefBoolNames.at(6), prf.getHideBlacklisted() ? ("true") : ("false"));

    xmlWriter->writeTextElement(prefPathNames.at(0), prf.getPathSoTmplBridge(VstBridge::LinVst));
    xmlWriter->writeTextElement(prefPathNames.at(1), prf.getPathSoTmplBridge(VstBridge::LinVstX));
    xmlWriter->writeTextElement(prefPathNames.at(2), prf.getPathSoTmplBridge(VstBridge::LinVst3));
    xmlWriter->writeTextElement(prefPathNames.at(3), prf.getPathSoTmplBridge(VstBridge::LinVst3X));

    xmlWriter->writeTextElement(prefPathNames.at(4), prf.getPathLinkFolder());
    xmlWriter->writeTextElement(prefPathNames.at(5), prf.getPathCheckTool());
}

void ConfigHandler::writeVstBuckets(const QList<VstBucket> &vstBuckets)
{
    for (const auto &vstBucket : vstBuckets) {
        xmlWriter->writeStartElement("VstBucketEntry");
        xmlWriter->writeTextElement("name", vstBucket.name);
        xmlWriter->writeTextElement("vstPath", vstBucket.vstPath);
//        xmlWriter->writeTextElement("hash", QString::fromStdString(vstBuckets.at(i).hash.toStdString()));

        xmlWriter->writeTextElement("status", mapStatusStr.value(vstBucket.status));
        xmlWriter->writeTextElement("bridge", mapBridgeStr.value(vstBucket.bridge));
        xmlWriter->writeTextElement("vstType", mapTypeStr.value(vstBucket.vstType));

        xmlWriter->writeEndElement();
    }
}

quint8 ConfigHandler::readPreferences(Preferences &prf)
{
    // Read 4 boolean values
    QList<bool> boolValues;
    QString temp;
    for (int i = 0; i < prefBoolNames.size(); i++) {
        xmlReader->readNextStartElement();
        if (xmlReader->name() == prefBoolNames.at(i)) {
            temp = xmlReader->readElementText();
            if (temp == "true") {
                boolValues.append(true);
            } else if (temp == "false") {
                boolValues.append(false);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    // Read 6 string values
    QStringList pathValues;
    for (int i = 0; i < prefPathNames.size(); i++) {
        xmlReader->readNextStartElement();
        if (xmlReader->name() == prefPathNames.at(i)) {
            pathValues.append(xmlReader->readElementText());
        } else {
            return false;
        }
    }

    prf.updatePreferences(boolValues.at(0),
                          boolValues.at(1),
                          boolValues.at(2),
                          boolValues.at(3),
                          boolValues.at(4),
                          boolValues.at(5),
                          boolValues.at(6),
                          pathValues.at(0),
                          pathValues.at(1),
                          pathValues.at(2),
                          pathValues.at(3),
                          pathValues.at(4),
                          pathValues.at(5));

    // Skip the closing element (returns false)
    xmlReader->skipCurrentElement();

    return true;
}

quint8 ConfigHandler::readVstBucket(QList<VstBucket> &vstBuckets)
{
    QString name;
    QString vstPath;
    QByteArray hash;
    VstStatus status;
    VstBridge bridge;
    VstType vstType;
    QString temp;

    xmlReader->readNextStartElement();
    if (xmlReader->name() == "name") {
        name = xmlReader->readElementText();
    } else {
        return false;
    }

    xmlReader->readNextStartElement();
    if (xmlReader->name() == "vstPath") {
        vstPath = xmlReader->readElementText();
    } else {
        return false;
    }

    xmlReader->readNextStartElement();
    if (xmlReader->name() == "status") {
        temp = xmlReader->readElementText();
        status = mapStatusStr.key(temp);
    } else {
        return false;
    }

    xmlReader->readNextStartElement();
    if (xmlReader->name() == "bridge") {
        temp = xmlReader->readElementText();
        bridge = mapBridgeStr.key(temp);
    } else {
        return false;
    }

    xmlReader->readNextStartElement();
    if (xmlReader->name() == "vstType") {
        temp = xmlReader->readElementText();
        vstType = mapTypeStr.key(temp);
    } else {
        return false;
    }

    vstBuckets.append(VstBucket(name,
                                vstPath,
                                hash, // being re-calculated later
                                status,
                                bridge,
                                vstType,
                                false));

    // Skip the closing element (returns false)
    xmlReader->skipCurrentElement();

    return true;
}

RvConfFile ConfigHandler::saveConfig(const Preferences &prf, const QList<VstBucket> &vstBuckets)
{
    QElapsedTimer timer;
    timer.start();

    QFile configFile(QDir::homePath() + QString(D_CONFIG_FILE_PATH));
    QFileInfo fileInfo(configFile);
    QDir configDir(fileInfo.path());

    if (!configDir.exists()) {
        // config file folder doesn't exist yet. Create it.
        if(!configDir.mkpath(configDir.path())) {
            qDebug() << "ERROR ===>>> on creating config folder!!!";
            return RvConfFile::ErrorWriteDir;
        }
    }

    if (!(configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))) {
        qDebug() << "ERROR ===>>> on writing config file!!!";
        emit configSaveDone(true);
        return RvConfFile::ErrorWriteFile;
    }

    xmlWriter->setDevice(&configFile);

    xmlWriter->writeStartDocument();
    xmlWriter->writeDTD("<!DOCTYPE linvstmanagerconfig>");
    xmlWriter->writeStartElement("linvstmanagerconfig");
    xmlWriter->writeAttribute("version", "1.0");

    // ===========
    // Preferences
    // ===========
    xmlWriter->writeStartElement("Preferences");
    writePreferences(prf);
    xmlWriter->writeEndElement();

    // ==========
    // VstBuckets
    // ==========
    xmlWriter->writeStartElement("VstBuckets");
    writeVstBuckets(vstBuckets);
    xmlWriter->writeEndElement();

    // ============
    xmlWriter->writeEndDocument();   // </linvstmanagerconfig>

    configFile.close();

    qDebug() << QString("=========   saveConfig done! (saving took %1 milliseconds)  =============").arg(timer.elapsed());

    emit this->configSaveDone(false);

    return RvConfFile::CH_OK;
}

RvConfFile ConfigHandler::loadConfig(Preferences &prf, QList<VstBucket> &vstBuckets)
{
    // Check if file actually exists (check folder path first)
    QFile configFile(QDir::homePath() + QString(D_CONFIG_FILE_PATH));
    if (!configFile.exists()) {
        // If the file doesn't exist yet, it's not a real error but probably just first time running the app
        qDebug() << "NOTE ===>>> config file doesn't exist!!!";
        return RvConfFile::NotExists;
    } else {
        if (!(configFile.open(QIODevice::ReadOnly))) {
            qDebug() << "ERROR ===>>> on opening config file!!!";
            return RvConfFile::ErrorLoad;
        }
    }

    xmlReader->setDevice(&configFile);

    if (xmlReader->readNextStartElement()) {
        if (xmlReader->name() == "linvstmanagerconfig"
                && xmlReader->attributes().value("version") == "1.0") {
            while (xmlReader->readNextStartElement()) {
                if (xmlReader->name() == "Preferences") {
                    if (!readPreferences(prf)) {
                        return RvConfFile::ParsingError;
                    }
                } else if (xmlReader->name() == "VstBuckets") {
                    while (xmlReader->readNextStartElement()) {
                        if (xmlReader->name() == "VstBucketEntry") {
                            if (!readVstBucket(vstBuckets)) {
                                return RvConfFile::ParsingError;
                            }
                        }
                    }

                    xmlReader->skipCurrentElement(); // Skip closing element
                } else {
                    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Should not happen. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
                }
            }
        } else {
            return RvConfFile::ParsingError;
        }
    }

    xmlReader->clear();

    configFile.close();

    qDebug() << "=========   loadConfig done!   =============";

    return RvConfFile::CH_OK;
}
