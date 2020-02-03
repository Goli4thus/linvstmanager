// This file is part of LinVstManager.

#ifndef LEGACYCONFIGPARSER_H
#define LEGACYCONFIGPARSER_H

class QString;
class QStringList;

class LegacyConfigParser
{
public:
    LegacyConfigParser();
    QStringList parseLegacyConfig(QString configPath);
};

#endif // LEGACYCONFIGPARSER_H
