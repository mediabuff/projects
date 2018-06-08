#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// ATL Headers
#include <atlbase.h>
#include <atlstr.h>		// Use CString from ATL

// WTL Headers
#include <atlapp.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlframe.h>

#define _WTL_NO_CSTRING	// Tell atlmisc.h not to define CString
#include <atlmisc.h>	// Must include before atlcrack.h
#include <atlcrack.h>

// C RunTime Header Files
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>

#include <thread>

extern CAppModule _Module;
