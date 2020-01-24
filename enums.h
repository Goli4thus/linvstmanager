#ifndef ENUMS
#define ENUMS


enum VstStatus {
    Enabled,     // VST is enabled via active softlink
    Disabled,    // VST is disabled due to missing softlink
    Mismatch,    // Mismatch between linvst.so and *.so file associated with VST-dll
    No_So,       // VST-dll has no associated VST-so file
    NotFound,    // VST-dll can't be found using the specified config path
    NoBridge,    // No suitable bridge has been enabled in preferences.
    Orphan,      // The so-file seems orphaned as it doesn't refer to an existing VST-dll
    NA,          // Initial state
    Blacklisted  // VST is blacklisted from being handled
};

enum VstBridge {
    LinVst,
    LinVstX,
    LinVst3,
    LinVst3X
};

enum VstType {
    VST2,
    VST3
};

enum TableColumnPosType {
    NewlyAdded = 0,
    Status = 1,
    Name = 2,
    Type = 3,
    Bridge = 4,
    Path = 5,
    Index = 6
};

// Return values re config file
enum RvConfFile {
    CH_OK,
    NotExists,
    ErrorLoad,
    ErrorWriteDir,
    ErrorWriteFile,
    ParsingError
};

// Return values re link handler
enum RvLinkHandler {
    LH_OK
};

#endif // ENUMS
