#include "preferences.h"

Preferences::Preferences()
{
    enabledLinVst = false;
    enabledLinVstX = false;
    enabledLinVst3 = false;
    enabledLinVst3X = false;
    pathSoLinVst = "";
    pathSoLinVstX = "";
    pathSoLinVst3 = "";
    pathSoLinVst3X = "";
    pathLinkFolder = "";
    bridgeDefaultVst2IsX = false;
    bridgeDefaultVst3IsX = false;
}

bool Preferences::updatePreferences(bool t_enabledLinVst,
                                    bool t_enabledLinVstX,
                                    bool t_enabledLinVst3,
                                    bool t_enabledLinVst3X,
                                    QString t_pathSoLinVst,
                                    QString t_pathSoLinVstX,
                                    QString t_pathSoLinVst3,
                                    QString t_pathSoLinVst3X,
                                    QString t_pathLinkFolder,
                                    bool t_bridgeDefaultVst2IsX,
                                    bool t_bridgeDefaultVst3IsX)
{
    bool prefsChanged = false;

    if (enabledLinVst != t_enabledLinVst) {
        enabledLinVst = t_enabledLinVst;
        prefsChanged = true;
    }
    if (enabledLinVstX != t_enabledLinVstX) {
        enabledLinVstX = t_enabledLinVstX;
        prefsChanged = true;
    }
    if (enabledLinVst3 != t_enabledLinVst3) {
        enabledLinVst3 = t_enabledLinVst3;
        prefsChanged = true;
    }
    if (enabledLinVst3X != t_enabledLinVst3X) {
        enabledLinVst3X = t_enabledLinVst3X;
        prefsChanged = true;
    }
    if (pathSoLinVst != t_pathSoLinVst) {
        pathSoLinVst = t_pathSoLinVst;
        prefsChanged = true;
    }
    if (pathSoLinVstX != t_pathSoLinVstX) {
        pathSoLinVstX = t_pathSoLinVstX;
        prefsChanged = true;
    }
    if (pathSoLinVst3 != t_pathSoLinVst3) {
        pathSoLinVst3 = t_pathSoLinVst3;
        prefsChanged = true;
    }
    if (pathSoLinVst3X != t_pathSoLinVst3X) {
        pathSoLinVst3X = t_pathSoLinVst3X;
        prefsChanged = true;
    }
    if (pathLinkFolder != t_pathLinkFolder) {
        pathLinkFolder = t_pathLinkFolder;
        prefsChanged = true;
    }
    if (bridgeDefaultVst2IsX != t_bridgeDefaultVst2IsX) {
        bridgeDefaultVst2IsX = t_bridgeDefaultVst2IsX;
        prefsChanged = true;
    }
    if (bridgeDefaultVst3IsX != t_bridgeDefaultVst3IsX) {
        bridgeDefaultVst3IsX = t_bridgeDefaultVst3IsX;
        prefsChanged = true;
    }

    return prefsChanged;
}

bool Preferences::getEnabledLinVst() const
{
    return enabledLinVst;
}

bool Preferences::getEnabledLinVstX() const
{
    return enabledLinVstX;
}

bool Preferences::getEnabledLinVst3() const
{
    return enabledLinVst3;
}

bool Preferences::getEnabledLinVst3X() const
{
    return enabledLinVst3X;
}

QString Preferences::getPathSoLinVst() const
{
    return pathSoLinVst;
}

QString Preferences::getPathSoLinVstX() const
{
    return pathSoLinVstX;
}

QString Preferences::getPathSoLinVst3() const
{
    return pathSoLinVst3;
}

QString Preferences::getPathSoLinVst3X() const
{
    return pathSoLinVst3X;
}

QString Preferences::getPathLinkFolder() const
{
    return pathLinkFolder;
}

bool Preferences::getBridgeDefaultVst2IsX() const
{
    return bridgeDefaultVst2IsX;
}

bool Preferences::getBridgeDefaultVst3IsX() const
{
    return bridgeDefaultVst3IsX;
}
