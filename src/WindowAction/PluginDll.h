// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GUIPLUGIN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GUIPLUGIN_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
//**************************************************
#include <Plugin.h>

#include <script/VMCore.h>
#include <script/Declarator.h>
//define the actual plugin functions
extern "C" __declspec(dllexport) void GetPluginInfo(struct PluginStartupInfo *Info);
extern "C" __declspec(dllexport) void InitClass(script::VMCore *core);