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
    mConfigVersionLatest = "1.2";
    mConfigVersionHistory << "1.0" << "1.1" << "1.2";

    xmlReader = new QXmlStreamReader();
    xmlWriter = new QXmlStreamWriter();
    xmlWriter->setAutoFormatting(true);

    mPrefBoolNamesOfInterest_V1_2 = {true,   // "f_enabledLinVst"
                                     true,   // "f_enabledLinVstX"
                                     true,   // "f_enabledLinVst3"
                                     true,   // "f_enabledLinVst3X"
                                     true,   // "f_bridgeDefaultVst2IsX"
                                     true,   // "f_bridgeDefaultVst3IsX"
                                     true};  // "f_hideBlacklisted"

    prefBoolNames << "f_enabledLinVst"
                  << "f_enabledLinVstX"
                  << "f_enabledLinVst3"
                  << "f_enabledLinVst3X"
                  << "f_bridgeDefaultVst2IsX"
                  << "f_bridgeDefaultVst3IsX"
                  << "f_hideBlacklisted";

    // Some definitions for backwards compatibility.
    mPrefPathNamesOfInterest_V1_0 = {true,   // "pathSoLinVst"
                                     true,   // "pathSoLinVstX"
                                     true,   // "pathSoLinVst3"
                                     true,   // "pathSoLinVst3X"
                                     true,   // "pathLinkFolder"
                                     false}; // "pathCheckTool"

    mPrefPathNamesOfInterest_V1_1 = {true,   // "pathSoLinVst"
                                     true,   // "pathSoLinVstX"
                                     true,   // "pathSoLinVst3"
                                     true,   // "pathSoLinVst3X"
                                     true,   // "pathLinkFolder"
                                     false,  // "pathCheckTool64"
                                     false}; // "pathCheckTool32"

    mPrefPathNamesOfInterest_V1_2 = {true,   // "pathSoLinVst"
                                     true,   // "pathSoLinVstX"
                                     true,   // "pathSoLinVst3"
                                     true,   // "pathSoLinVst3X"
                                     true};  // "pathLinkFolder"

    prefPathNames << "pathSoLinVst"
                  << "pathSoLinVstX"
                  << "pathSoLinVst3"
                  << "pathSoLinVst3X"
                  << "pathLinkFolder";

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

    mapVstTypeStr.insert(VstType::VST2, "VST2");
    mapVstTypeStr.insert(VstType::VST3, "VST3");

    mapArchTypeStr.insert(ArchType::Arch64, "64");
    mapArchTypeStr.insert(ArchType::Arch32, "32");
    mapArchTypeStr.insert(ArchType::ArchNA, "NA");
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
}

void ConfigHandler::writeVstBuckets(const QVector<VstBucket> &vstBuckets)
{
    for (const auto &vstBucket : vstBuckets) {
        xmlWriter->writeStartElement("VstBucketEntry");
        xmlWriter->writeTextElement("name", vstBucket.name);
        xmlWriter->writeTextElement("vstPath", vstBucket.vstPath);
//        xmlWriter->writeTextElement("hash", QString::fromStdString(vstBuckets.at(i).hash.toStdString()));

        xmlWriter->writeTextElement("status", mapStatusStr.value(vstBucket.status));
        xmlWriter->writeTextElement("bridge", mapBridgeStr.value(vstBucket.bridge));
        xmlWriter->writeTextElement("vstType", mapVstTypeStr.value(vstBucket.vstType));
        xmlWriter->writeTextElement("archType", mapArchTypeStr.value(vstBucket.archType));

        xmlWriter->writeEndElement();
    }
}

quint8 ConfigHandler::readPreferences(Preferences &prf, QString configVersion)
{
    // Read 4 boolean values
    QVector<bool> boolValues;
    QVector<bool> prefBoolNamesOfInterest;
    QString temp;
    QString elementName;

    prefBoolNamesOfInterest = mPrefBoolNamesOfInterest_V1_2;

    int k = 0;
    for (int i = 0; i < prefBoolNamesOfInterest.size(); i++) {
        xmlReader->readNextStartElement();
        if (prefBoolNamesOfInterest.at(i)) {
            elementName = xmlReader->name().toString();
            if (elementName == prefBoolNames.at(k++)) {
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
        } else {
            // Skip the closing element (returns false), if it wasn't yet
            // read indirectly by means of xmlReader->readElementText().
            xmlReader->skipCurrentElement();
        }
    }

    QStringList pathValues;
    QVector<bool> prefPathNamesOfInterest;
    if (configVersion == "1.0") {
        prefPathNamesOfInterest = mPrefPathNamesOfInterest_V1_0;
    } else if (configVersion == "1.1") {
        prefPathNamesOfInterest = mPrefPathNamesOfInterest_V1_1;
    } else { // Latest config version
        prefPathNamesOfInterest = mPrefPathNamesOfInterest_V1_2;
    }

    k = 0;
    volatile bool xmlStatus;
    for (int i = 0; i < prefPathNamesOfInterest.size(); i++) {
        xmlStatus = xmlReader->readNextStartElement();
        if (!xmlStatus) {
            qDebug() << Q_FUNC_INFO << "Probably something wrong with "
                                       "not skipping closing element.";
        }
        if (prefPathNamesOfInterest.at(i)) {
            // The right order of config file entries is expected here.
            elementName = xmlReader->name().toString();
            if (elementName == prefPathNames.at(k++)) {
                pathValues.append(xmlReader->readElementText());
            } else {
                return false;
            }
        } else {
            // Skip the closing element (returns false), if it wasn't yet
            // read indirectly by means of xmlReader->readElementText().
            xmlReader->skipCurrentElement();
        }
    }

    QVector<VstBridge> emptyList;
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
                          emptyList);


    // Skip the closing element </Preferences> (returns false)
    xmlReader->skipCurrentElement();

    return true;
}

quint8 ConfigHandler::readVstBucket(QVector<VstBucket> &vstBuckets)
{
    QString name;
    QString vstPath;
    QByteArray hash;
    VstStatus status;
    VstBridge bridge;
    VstType vstType;
    ArchType archType;
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
        vstType = mapVstTypeStr.key(temp);
    } else {
        return false;
    }

    if (mConfigVersionLoaded != "1.0") {
        xmlReader->readNextStartElement();
        QString elementName = xmlReader->name().toString();
        if ((elementName == "bitType") || (elementName == "archType")) {
            temp = xmlReader->readElementText();
            archType = mapArchTypeStr.key(temp);
        } else {
            return false;
        }
    } else {
        archType = ArchType::ArchNA;
    }

    vstBuckets.append(VstBucket(name,
                                vstPath,
                                hash, // being re-calculated later
                                hash, // being re-calculated later
                                status,
                                bridge,
                                vstType,
                                archType,
                                false));

    // Skip the closing element (returns false)
    xmlReader->skipCurrentElement();

    return true;
}

RvConfFile ConfigHandler::saveConfig(const Preferences &prf, const QVector<VstBucket> &vstBuckets)
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
    xmlWriter->writeAttribute("version", mConfigVersionLatest);

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

RvConfFile ConfigHandler::loadConfig(Preferences &prf, QVector<VstBucket> &vstBuckets)
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

    QString elementName;
    if (xmlReader->readNextStartElement()) {
        if (xmlReader->name() == "linvstmanagerconfig") {
            mConfigVersionLoaded = xmlReader->attributes().value("version").toString();

            // Check if config version is known
            if (mConfigVersionHistory.contains(mConfigVersionLoaded)) {
                while (xmlReader->readNextStartElement()) {
                    elementName = xmlReader->name().toString();
                    if (elementName == "Preferences") {
                        if (!readPreferences(prf, mConfigVersionLoaded)) {
                            return RvConfFile::ParsingError;
                        }
                    } else if (elementName == "VstBuckets") {
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
                return RvConfFile::VersionError;
            }
        } else {
            return RvConfFile::ParsingError;
        }
    }

    xmlReader->clear();

    configFile.close();

//    qDebug() << "=========   loadConfig done!   =============";

    return RvConfFile::CH_OK;
}
