// This file is part of LinVstManager.

#ifndef ENUMS
#define ENUMS

#include <QMetaType>


enum VstStatus {
    Enabled,     // VST is enabled via active softlink
    Disabled,    // VST is disabled due to missing softlink
    Mismatch,    // Mismatch between linvst.so and *.so file associated with VST-dll
    No_So,       // VST-dll has no associated VST-so file
    NoBridge,    // No suitable bridge has been enabled in preferences.
    Conflict,    // VST with the same name already exists
    NotFound,    // VST-dll can't be found using the specified config path
    Orphan,      // The so-file seems orphaned as it doesn't refer to an existing VST-dll
    NA,          // Initial state
    Blacklisted  // VST is blacklisted from being handled
};
Q_DECLARE_METATYPE(VstStatus);

enum VstBridge {
    LinVst,
    LinVstX,
    LinVst3,
    LinVst3X
};
Q_DECLARE_METATYPE(VstBridge);

enum VstType {
    VST2,
    VST3,
    NoVST
};

enum BitType {
    Bits64,
    Bits32,
    BitsNA
};

namespace nsMainWindow {
    enum TableColumnPosType {
        NewlyAdded = 0,
        Status = 1,
        Name = 2,
        VstType = 3,
        BitType = 4,
        Bridge = 5,
        Path = 6,
        Index = 7
    };
}
namespace nsMW = nsMainWindow;

namespace nsDialogScan {
    enum TableColumnPosType {
        Status = 0,
        Name = 1,
        VstType = 2,
        BitType = 3,
        Path = 4,
        Index = 5
    };
}
namespace nsDS = nsDialogScan;

// Return values re config file
enum RvConfFile {
    CH_OK,
    NotExists,
    ErrorLoad,
    ErrorWriteDir,
    ErrorWriteFile,
    ParsingError,
    VersionError
};

// Return values re link handler
enum RvLinkHandler {
    LH_OK,
    LH_NOT_OK,
    LH_IGNORED
};

#endif // ENUMS
