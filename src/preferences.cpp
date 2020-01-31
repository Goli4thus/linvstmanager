// This file is part of LinVstManager.

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
    pathCheckTool = "";
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
                                    QString t_pathCheckTool,
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
    if (pathCheckTool != t_pathCheckTool) {
        pathCheckTool = t_pathCheckTool;
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

QString Preferences::getPathLinkFolder() const
{
    return pathLinkFolder;
}

QString Preferences::getPathCheckTool() const
{
    return pathCheckTool;
}

bool Preferences::checkToolEnabled() const
{
    return !(pathCheckTool.isEmpty());
}

bool Preferences::getBridgeDefaultVst2IsX() const
{
    return bridgeDefaultVst2IsX;
}

bool Preferences::getBridgeDefaultVst3IsX() const
{
    return bridgeDefaultVst3IsX;
}

bool Preferences::bridgeEnabled(VstBridge bridgeType) const
{
    switch (bridgeType) {
        case VstBridge::LinVst:
            return enabledLinVst;
        break;
        case VstBridge::LinVstX:
            return enabledLinVstX;
        break;
        case VstBridge::LinVst3:
            return enabledLinVst3;
        break;
        case VstBridge::LinVst3X:
            return enabledLinVst3X;
        break;
    }
    return false;
}

QString Preferences::getPathSoTmplBridge(VstBridge bridgeType) const
{
    switch (bridgeType) {
        case VstBridge::LinVst:
            return pathSoLinVst;
        break;
        case VstBridge::LinVstX:
            return pathSoLinVstX;
        break;
        case VstBridge::LinVst3:
            return pathSoLinVst3;
        break;
        case VstBridge::LinVst3X:
            return pathSoLinVst3X;
        break;
    }
    return "";
}
