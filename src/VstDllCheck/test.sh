# test.sh
#
# Purpose: 
# Simple test script to launch "VstDllCheck{64, 32}.exe" with the specified VST 
# and output exit code in human readable format.

# Checks
if [[ $# -eq 0 ]]; then
    echo -e "Usage: test.sh [ARCHITECTURE] [PATH]\n"
    echo -e "       with: - 'ARCHITECTURE': '64' or '32'"
    echo -e "             - 'PATH': full path to VST\n"
    exit 1
fi

if [ -z "$1" ]
then
    echo "First argument 'architecture' not specified ('64' or '32')"
    exit 1
fi

if [ -z "$2" ]
then
    echo "Second argument 'VST path' not specified (full path to VST.dll/vst3)"
    exit 1
fi

# Actual execution
cmd64="./VstDllCheck64.exe"
cmd32="./VstDllCheck32.exe"

if [[ $1 -eq 64 ]]; then
    cmd=$cmd64
elif [[ $1 -eq 32 ]]; then
    cmd=$cmd32
else
    echo "Unsupported first argument. Either '64' or '32')"
    exit 1
fi

# Run the command
$cmd $2

# Interpret exit code
case $? in
    0) echo "D_CHECK_PASSED" ;;
    1) echo "D_CHECK_FAILED_LIBRARYLOADERROR" ;;
    2) echo "D_CHECK_FAILED_ENTRYPOINT_NOT_FOUND" ;;
    3) echo "D_CHECK_FAILED_AEFFECTERROR" ;;
    4) echo "D_CHECK_FAILED_NO_KEFFECTMAGIC_NO_VST" ;;
    5) echo "D_CHECK_FAILED_NO_PROCESSREPLACING" ;;
    6) echo "D_CHECK_FAILED_NO_EDITOR" ;;
esac

