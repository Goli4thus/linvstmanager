# VstDllCheck

 > A stripped down version of 'TestVst" utility included with LinVst (created by osxmidi).
 > (see: https://github.com/osxmidi/LinVst/tree/master/TestVst)

VstDllCheck tests if a dll file is actually a VST plugin or not.

Usage is:
- for 64 bit VSTs: ./VstDllCheck64.exe "vstfile.dll"
- for 32 bit VSTs: ./VstDllCheck32.exe "vstfile.dll"

Return various exit codes depending on evaluation result (see vstdllcheck.h for details).
Paths and VST filenames that contain spaces need to be enclosed in quotes.


Running the commands like described is intended for usage in a context in which 
exit codes are evaluated (like LinVstManager does).

For simple console usage the script "test.sh" is provided 
(hint: run script without arguments to get usage information).

