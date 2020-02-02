# VstDllCheck

 > A stripped down version of 'TestVst" utility included with LinVst (created by osxmidi).
 > (see: https://github.com/osxmidi/LinVst/tree/master/TestVst)

VstDllCheck tests if a dll file is actually a VST plugin or not.

Usage is ./VstDllCheck.exe "vstfile.dll"

paths and vst filenames that contain spaces need to be enclosed in quotes.

Return various exit codes depending on evaluation result (see vstdllcheck.h for details).

