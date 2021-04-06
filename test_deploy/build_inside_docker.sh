#!/bin/bash

# Purpose:
# Build script to be run inside of a running docker container..
# Result is all build console output being redirected to the specified file.

image=$(uname -n)

mkdir -p ../LinVstManager/test_deploy/build_output
cmake ../LinVstManager > ../LinVstManager/test_deploy/build_output/build_output__$image.txt 2>&1
make -j4 >> ../LinVstManager/test_deploy/build_output/build_output__$image.txt 2>&1

# Output Qt version
if command -v qmake &> /dev/null; then
    qmake -v >> ../LinVstManager/test_deploy/build_output/build_output__$image.txt 2>&1
elif command -v qmake-qt5 &> /dev/null; then 
    qmake-qt5 -v >> ../LinVstManager/test_deploy/build_output/build_output__$image.txt 2>&1
else
    echo "INFO: No suitable 'qmake' command found." >> ../LinVstManager/test_deploy/build_output/build_output__$image.txt 2>&1
fi

