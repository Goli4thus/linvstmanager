// This file is part of LinVstManager.

#include "preferences.h"
#include <QVector>

Preferences::Preferences()
{
    enabledLinVst = false;
    enabledLinVstX = false;
    enabledLinVst3 = false;
    enabledLinVst3X = false;
    bridgeDefaultVst2IsX = false;
    bridgeDefaultVst3IsX = false;
    hideBlacklisted = false;
    pathSoLinVst = "";
    pathSoLinVstX = "";
    pathSoLinVst3 = "";
    pathSoLinVst3X = "";
    pathLinkFolder = "";
    pathCheckTool64 = "";
    pathCheckTool32 = "";
}

bool Preferences::updatePreferences(bool pEnabledLinVst,
                                    bool pEnabledLinVstX,
                                    bool pEnabledLinVst3,
                                    bool pEnabledLinVst3X,
                                    bool pBridgeDefaultVst2IsX,
                                    bool pBridgeDefaultVst3IsX,
                                    bool pHideBlacklisted,
                                    const QString &pPathSoLinVst,
                                    const QString &pPathSoLinVstX,
                                    const QString &pPathSoLinVst3,
                                    const QString &pPathSoLinVst3X,
                                    const QString &pPathLinkFolder,
                                    const QString &pPathCheckTool64,
                                    const QString &pPathCheckTool32,
                                    QVector<VstBridge> &pChangedBridges)
{
    bool prefsChanged = false;

    if (enabledLinVst != pEnabledLinVst) {
        enabledLinVst = pEnabledLinVst;
        prefsChanged = true;
    }
    if (enabledLinVstX != pEnabledLinVstX) {
        enabledLinVstX = pEnabledLinVstX;
        prefsChanged = true;
    }
    if (enabledLinVst3 != pEnabledLinVst3) {
        enabledLinVst3 = pEnabledLinVst3;
        prefsChanged = true;
    }
    if (enabledLinVst3X != pEnabledLinVst3X) {
        enabledLinVst3X = pEnabledLinVst3X;
        prefsChanged = true;
    }
    if (pathSoLinVst != pPathSoLinVst) {
        pathSoLinVst = pPathSoLinVst;
        prefsChanged = true;
        pChangedBridges.append(VstBridge::LinVst);
    }
    if (pathSoLinVstX != pPathSoLinVstX) {
        pathSoLinVstX = pPathSoLinVstX;
        prefsChanged = true;
        pChangedBridges.append(VstBridge::LinVstX);
    }
    if (pathSoLinVst3 != pPathSoLinVst3) {
        pathSoLinVst3 = pPathSoLinVst3;
        prefsChanged = true;
        pChangedBridges.append(VstBridge::LinVst3);
    }
    if (pathSoLinVst3X != pPathSoLinVst3X) {
        pathSoLinVst3X = pPathSoLinVst3X;
        prefsChanged = true;
        pChangedBridges.append(VstBridge::LinVst3X);
    }
    if (pathLinkFolder != pPathLinkFolder) {
        pathLinkFolder = pPathLinkFolder;
        prefsChanged = true;
    }
    if (pathCheckTool64 != pPathCheckTool64) {
        pathCheckTool64 = pPathCheckTool64;
        prefsChanged = true;
    }
    if (pathCheckTool32 != pPathCheckTool32) {
        pathCheckTool32 = pPathCheckTool32;
        prefsChanged = true;
    }
    if (hideBlacklisted != pHideBlacklisted) {
        hideBlacklisted = pHideBlacklisted;
        prefsChanged = true;
    }
    if (bridgeDefaultVst2IsX != pBridgeDefaultVst2IsX) {
        bridgeDefaultVst2IsX = pBridgeDefaultVst2IsX;
        prefsChanged = true;
    }
    if (bridgeDefaultVst3IsX != pBridgeDefaultVst3IsX) {
        bridgeDefaultVst3IsX = pBridgeDefaultVst3IsX;
        prefsChanged = true;
    }

    return prefsChanged;
}

QString Preferences::getPathLinkFolder() const
{
    return pathLinkFolder;
}

QString Preferences::getPathCheckTool64() const
{
    return pathCheckTool64;
}

QString Preferences::getPathCheckTool32() const
{
    return pathCheckTool32;
}

bool Preferences::checkTool64Enabled() const
{
    return !(pathCheckTool64.isEmpty());
}

bool Preferences::checkTool32Enabled() const
{
    return !(pathCheckTool32.isEmpty());
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
        case VstBridge::LinVstX:
            return enabledLinVstX;
        case VstBridge::LinVst3:
            return enabledLinVst3;
        case VstBridge::LinVst3X:
            return enabledLinVst3X;
    }
    return false;
}

QString Preferences::getPathSoTmplBridge(VstBridge bridgeType) const
{
    switch (bridgeType) {
        case VstBridge::LinVst:
            return pathSoLinVst;
        case VstBridge::LinVstX:
            return pathSoLinVstX;
        case VstBridge::LinVst3:
            return pathSoLinVst3;
        case VstBridge::LinVst3X:
            return pathSoLinVst3X;
    }
    return "";
}

bool Preferences::setHideBlacklisted(bool value)
{
    bool prefsChanged = false;
    if (hideBlacklisted != value) {
        hideBlacklisted = value;
        prefsChanged = true;
    }
    return prefsChanged;
}

bool Preferences::getHideBlacklisted() const
{
    return hideBlacklisted;
}
