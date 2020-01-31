/*
 * --------------------
 * VstDllCheck (header)
 * --------------------
 * A stripped down version of 'TestVst" utility included with LinVst (created by osxmidi).
 * (see: https://github.com/osxmidi/LinVst/tree/master/TestVst)
 *
 * Purpose: An even smaller utility that only checks if a dll-file is actually a VST-file 
 *          or not.  This program is used in conjunction with LinVstManager (specifically 
 *          during 'scan' operation).
 */

#define D_CHECK_PASSED                        (0)
#define D_CHECK_FAILED_LIBRARYLOADERROR       (1)
#define D_CHECK_FAILED_ENTRYPOINT_NOT_FOUND   (2)
#define D_CHECK_FAILED_AEFFECTERROR           (3)
#define D_CHECK_FAILED_NO_KEFFECTMAGIC_NO_VST (4)
#define D_CHECK_FAILED_NO_PROCESSREPLACING    (5)
#define D_CHECK_FAILED_NO_EDITOR              (6)
