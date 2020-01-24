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
}

void ConfigHandler::writePreferences(const Preferences &prf)
{
    xmlWriter->writeTextElement("f_enabledLinVst", prf.getEnabledLinVst() ? ("true") : ("false"));
    xmlWriter->writeTextElement("f_enabledLinVstX", prf.getEnabledLinVstX() ? ("true") : ("false"));
    xmlWriter->writeTextElement("f_enabledLinVst3", prf.getEnabledLinVst3() ? ("true") : ("false"));
    xmlWriter->writeTextElement("f_enabledLinVst3X", prf.getEnabledLinVst3X() ? ("true") : ("false"));

    xmlWriter->writeTextElement("f_bridgeDefaultVst2IsX", prf.getBridgeDefaultVst2IsX() ? ("true") : ("false"));
    xmlWriter->writeTextElement("f_bridgeDefaultVst3IsX", prf.getBridgeDefaultVst3IsX() ? ("true") : ("false"));

    xmlWriter->writeTextElement("pathSoLinVst", prf.getPathSoLinVst());
    xmlWriter->writeTextElement("pathSoLinVstX", prf.getPathSoLinVstX());
    xmlWriter->writeTextElement("pathSoLinVst3", prf.getPathSoLinVst3());
    xmlWriter->writeTextElement("pathSoLinVst3X", prf.getPathSoLinVst3X());

    xmlWriter->writeTextElement("pathLinkFolder", prf.getPathLinkFolder());
}

void ConfigHandler::writeVstBuckets(const QList<VstBucket> &vstBuckets)
{
    for (int i = 0; i < vstBuckets.size(); ++i) {
        xmlWriter->writeStartElement("VstBucketEntry");
        xmlWriter->writeTextElement("name", vstBuckets.at(i).name);
        xmlWriter->writeTextElement("vstPath", vstBuckets.at(i).vstPath);
//        xmlWriter->writeTextElement("hash", QString::fromStdString(vstBuckets.at(i).hash.toStdString()));

        QString strStatus;
        switch (vstBuckets.at(i).status) {
        case VstStatus::Enabled:
            strStatus = "Enabled";
            break;
        case VstStatus::Disabled:
            strStatus = "Disabled";
            break;
        case VstStatus::Mismatch:
            strStatus = "Mismatch";
            break;
        case VstStatus::No_So:
            strStatus = "No_So";
            break;
        case VstStatus::NotFound:
            strStatus = "NotFound";
            break;
        case VstStatus::NoBridge:
            strStatus = "NoBridge";
            break;
        case VstStatus::Orphan:
            strStatus = "Orphan";
            break;
        case VstStatus::NA:
            strStatus = "NA";
            break;
        case VstStatus::Blacklisted:
            strStatus = "Blacklisted";
            break;
        }
        xmlWriter->writeTextElement("status", strStatus);

        QString strBridge;
        switch (vstBuckets.at(i).bridge) {
        case VstBridge::LinVst:
            strBridge = "LinVst";
            break;
        case VstBridge::LinVstX:
            strBridge = "LinVstX";
            break;
        case VstBridge::LinVst3:
            strBridge = "LinVst3";
            break;
        case VstBridge::LinVst3X:
            strBridge = "LinVst3";
            break;
        }
        xmlWriter->writeTextElement("bridge", strBridge);

        QString strVstType;
        switch (vstBuckets.at(i).vstType) {
        case VstType::VST2:
            strVstType = "VST2";
            break;
        case VstType::VST3:
            strVstType = "VST3";
            break;
        }
        xmlWriter->writeTextElement("vstType", strVstType);

        xmlWriter->writeEndElement();
    }
}

quint8 ConfigHandler::readPreferences(Preferences &prf)
{
    // Read 4 boolean values
    QStringList boolNames = {"f_enabledLinVst",
                             "f_enabledLinVstX",
                             "f_enabledLinVst3",
                             "f_enabledLinVst3X",
                             "f_bridgeDefaultVst2IsX",
                             "f_bridgeDefaultVst3IsX"};
    QList<bool> boolValues;
    QString temp;
    for (int i = 0; i < boolNames.size(); i++) {
        xmlReader->readNextStartElement();
        if (xmlReader->name() == boolNames.at(i)) {
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
    QStringList strNames = {"pathSoLinVst",
                            "pathSoLinVstX",
                            "pathSoLinVst3",
                            "pathSoLinVst3X",
                            "pathLinkFolder"};
    QStringList strValues;
    for (int i = 0; i < strNames.size(); i++) {
        xmlReader->readNextStartElement();
        if (xmlReader->name() == strNames.at(i)) {
            strValues.append(xmlReader->readElementText());
        } else {
            return false;
        }
    }

    prf.updatePreferences(boolValues.at(0),
                          boolValues.at(1),
                          boolValues.at(2),
                          boolValues.at(3),
                          strValues.at(0),
                          strValues.at(1),
                          strValues.at(2),
                          strValues.at(3),
                          strValues.at(4),
                          boolValues.at(4),
                          boolValues.at(5));

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
        QString strStatus = xmlReader->readElementText();
        if (strStatus == "Enabled") {
            status = VstStatus::Enabled;
        } else  if (strStatus == "Disabled") {
            status = VstStatus::Disabled;
        } else  if (strStatus == "Mismatch") {
            status = VstStatus::Mismatch;
        } else if (strStatus == "No_So") {
            status = VstStatus::No_So;
        } else  if (strStatus == "NotFound") {
            status = VstStatus::NotFound;
        } else  if (strStatus == "NoBridge") {
            status = VstStatus::NoBridge;
        } else  if (strStatus == "Orphan") {
            status = VstStatus::Orphan;
        } else  if (strStatus == "NA") {
            status = VstStatus::NA;
        } else  if (strStatus == "Blacklisted") {
            status = VstStatus::Blacklisted;
        } else {
            return false;
        }
    } else {
        return false;
    }

    xmlReader->readNextStartElement();
    if (xmlReader->name() == "bridge") {
        QString strBridge = xmlReader->readElementText();
        if (strBridge == "LinVst") {
            bridge = VstBridge::LinVst;
        } else if (strBridge == "LinVstX") {
            bridge = VstBridge::LinVstX;
        } else if (strBridge == "LinVst3") {
            bridge = VstBridge::LinVst3;
        } else if (strBridge == "LinVst3") {
            bridge = VstBridge::LinVst3X;
        } else {
            return false;
        }
    } else {
        return false;
    }

    xmlReader->readNextStartElement();
    if (xmlReader->name() == "vstType") {
        QString strVstType = xmlReader->readElementText();
        if (strVstType == "VST2") {
            vstType = VstType::VST2;
        } else if (strVstType == "VST3") {
            vstType = VstType::VST3;
        } else {
            return false;
        }
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
