// This file is part of LinVstManager.

#include "legacyconfigparser.h"
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDebug>

LegacyConfigParser::LegacyConfigParser()
{

}

QStringList LegacyConfigParser::parseLegacyConfig(QString configPath)
{
    /* Escaped spaces contained within values of the original "linvstmanage.ini"
     * file are a problem. Therefore a temporary, "sanitized" ini-file is created,
     * which will be parsed instead.
     */
    QFile sanitizedFile(QFileInfo(configPath).path() + "/tmpSanitized.ini");
    QFile configFile(configPath);

    QByteArray byteContent;
    if (configFile.open(QIODevice::ReadOnly)) {
        byteContent = configFile.readAll();
        configFile.close();
    } else {
        qDebug() << "[LegacyConfigParser]: error on configFile.open(): " << configFile.errorString();
        return QStringList();
    }

    QString content(byteContent);
    content.replace(QString("\\ "), QString(" "));

    if (sanitizedFile.open(QIODevice::WriteOnly)) {
        sanitizedFile.write(content.toUtf8());
        sanitizedFile.close();
    } else {
        qDebug() << "[LegacyConfigParser]: error on sanitizedFile.open(): " << sanitizedFile.errorString();
        return QStringList();
    }


    // Now the actual ini-file parsing
    QStringList dllPaths, dllNameList;
    QString path, dllNames;

    QSettings config(QFileInfo(sanitizedFile).filePath(), QSettings::IniFormat);

    // Iterate over all "vendor" groups
    // (groups are being parsed in alphabetical order)
    foreach(const QString &group, config.childGroups()) {
        if (group == "GeneralSettings") {
            continue;
        }
        config.beginGroup(group);

        // Read the two fixed keys' values
        path = config.value("path").toString();
        // Comma separated list results in QStringList by default
        dllNameList = config.value("dll_names").toStringList();

        // Construct dll paths
        foreach(const QString &dllName, dllNameList) {
            // Convert to absolute paths if needed
            if (path.startsWith('~')) {
                dllPaths.append(QDir::homePath() + QString(path + dllName).remove(0, 1));
            } else {
                dllPaths.append(QString(path + dllName));
            }
        }

        config.endGroup();
    }

    // Delete temporary file
    QFile::remove(QFileInfo(sanitizedFile).filePath());

    return dllPaths;
}
