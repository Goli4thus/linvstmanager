#ifndef DATASTORE_H
#define DATASTORE_H

#include <QMap>
#include "enums.h"

class DataStore
{
public:
    static QString getStatusStr(VstStatus status) {
        switch (status) {
            case VstStatus::Enabled:     return "Enabled";
            case VstStatus::Disabled:    return "Disabled";
            case VstStatus::Mismatch:    return "Mismatch";
            case VstStatus::No_So:       return "No_So";
            case VstStatus::Conflict:    return "Conflict";
            case VstStatus::NoBridge:    return "NoBridge";
            case VstStatus::NotFound:    return "NotFound";
            case VstStatus::Orphan:      return "Orphan";
            case VstStatus::NA:          return "NA";
            case VstStatus::Blacklisted: return "Blacklisted";
            default: return QString("--- %1").arg(status);
        }
    };

private:
    DataStore() {};
};

#endif // DATASTORE_H
