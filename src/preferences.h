#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>
#include "enums.h"


class Preferences
{
public:
    Preferences();

    bool updatePreferences(bool t_enabledLinVst,
                           bool t_enabledLinVstX,
                           bool t_enabledLinVst3,
                           bool t_enabledLinVst3X,
                           QString t_pathSoLinVst,
                           QString t_pathSoLinVstX,
                           QString t_pathSoLinVst3,
                           QString t_pathSoLinVst3X,
                           QString t_pathLinkFolder,
                           QString t_pathCheckTool,
                           bool t_bridgeDefaultVst2IsX,
                           bool t_bridgeDefaultVst3IsX);

    QString getPathLinkFolder() const;
    QString getPathCheckTool() const;
    bool checkToolEnabled() const;
    bool getBridgeDefaultVst2IsX() const;
    bool getBridgeDefaultVst3IsX() const;
    bool bridgeEnabled(VstBridge bridgeType) const;
    QString getPathSoTmplBridge(VstBridge bridgeType) const;

private:
    bool enabledLinVst;
    bool enabledLinVstX;
    bool enabledLinVst3;
    bool enabledLinVst3X;
    QString pathSoLinVst;
    QString pathSoLinVstX;
    QString pathSoLinVst3;
    QString pathSoLinVst3X;
    QString pathLinkFolder;
    QString pathCheckTool;
    bool bridgeDefaultVst2IsX;
    bool bridgeDefaultVst3IsX;
};

#endif // PREFERENCES_H
