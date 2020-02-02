/*
 * --------------------
 * VstDllCheck (source)
 * --------------------
 * A stripped down version of 'TestVst" utility included with LinVst (created by osxmidi).
 * (see: https://github.com/osxmidi/LinVst/tree/master/TestVst)
 *
 * Purpose: An even smaller utility that only checks if a dll-file is actually a VST-file 
 *          or not.  This program is used in conjunction with LinVstManager (specifically 
 *          during 'scan' operation).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>

#define WIN32_LEAN_AND_MEAN

#include "vstdllcheck.h"

#include <windows.h>
#include <shellapi.h>

#define APPLICATION_CLASS_NAME "dssi_vst"

#define OLD_PLUGIN_ENTRY_POINT "main"
#define NEW_PLUGIN_ENTRY_POINT "VSTPluginMain"

typedef int16_t VstInt16;	
typedef int32_t VstInt32;		
typedef int64_t VstInt64;		
typedef intptr_t VstIntPtr;
#define VESTIGECALLBACK __cdecl
#include "vestige.h"

typedef AEffect *(VESTIGECALLBACK *VstEntry)(audioMasterCallback audioMaster);

using namespace std;

VstIntPtr VESTIGECALLBACK hostCallback(AEffect *plugin, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt)    
{		 
    VstIntPtr rv = 0;

    switch (opcode) {  
        case audioMasterVersion:
            rv = 2400;
            break;
        default:
            break;           
    }    

    return rv; 
}  


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdline, int cmdshow)
{ 
    LPWSTR *args;
    int numargs;

    args = CommandLineToArgvW(GetCommandLineW(), &numargs);

    if(args == NULL) {
        printf("CommandLine arguments failed\n");
        exit(0);
    }

    if(args[1] == NULL) {
        printf("Usage: testvst_minimal.exe path_to_vst_dll\n");	
        exit(0);
    }	

    HINSTANCE libHandle = 0;

    libHandle = LoadLibraryW(args[1]);
    if (!libHandle) {
        return D_CHECK_FAILED_LIBRARYLOADERROR;
    }

    LocalFree(args);

    VstEntry getinstance = 0;

    getinstance = (VstEntry)GetProcAddress(libHandle, NEW_PLUGIN_ENTRY_POINT);

    if (!getinstance) {
        getinstance = (VstEntry)GetProcAddress(libHandle, OLD_PLUGIN_ENTRY_POINT);
        if (!getinstance) {
            if(libHandle) {
                FreeLibrary(libHandle);    
            }
            return D_CHECK_FAILED_ENTRYPOINT_NOT_FOUND;
        }
    }

    AEffect *m_plugin = getinstance(hostCallback);
    if (!m_plugin)	{
        if(libHandle) {
            FreeLibrary(libHandle);    
        }
        return D_CHECK_FAILED_AEFFECTERROR;
    }

    if (m_plugin->magic != kEffectMagic) {
        if(libHandle) {
            FreeLibrary(libHandle);	    
        }
        return D_CHECK_FAILED_NO_KEFFECTMAGIC_NO_VST;
    }

    if (!(m_plugin->flags & effFlagsCanReplacing)) {
        if(libHandle) {
            FreeLibrary(libHandle);	    
        }
        return D_CHECK_FAILED_NO_PROCESSREPLACING;
    }

    if(m_plugin->flags & effFlagsHasEditor) {
        if(libHandle) {
            FreeLibrary(libHandle);	    
        }
        return D_CHECK_PASSED;
    } else { 
        if(libHandle) {
            FreeLibrary(libHandle);	    
        }
        return D_CHECK_FAILED_NO_EDITOR;
    }
}

