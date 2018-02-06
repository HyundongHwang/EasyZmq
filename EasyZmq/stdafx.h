// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

// TODO: reference additional headers your program requires here



#include <atlwin.h>
#include <atltypes.h>
#include <atlcoll.h>
#include <atlimage.h>
using namespace ATL;



#include <cstdarg>
#include <Dbghelp.h>
#include <locale.h>
#include <time.h>
#include <list>
#include <process.h>
#include <TlHelp32.h>
#include <Strsafe.h>
#include <stdarg.h>



#pragma comment(lib, "Wininet.lib")
#include <WinInet.h>



#ifdef DEBUG
    #pragma comment(lib, "..\\lib\\libzmq\\bin\\debug\\libzmq.lib") 
#else
    #pragma comment(lib, "..\\lib\\libzmq\\bin\\release\\libzmq.lib") 
#endif // DEBUG

#include "..\\lib\\libzmq\\include\\zmq.h"



#include "ScaUtil.h"
