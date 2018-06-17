/* ------------------------------- Includes -------------------------------- */
#include "stdafx.h"
#include "Resource.h"

#include "MainDlg.h"

/* -------------------------------- Macros --------------------------------- */
/* ------------------------------- TypeDefs -------------------------------- */
/* -------------------------------- Classes -------------------------------- */
/* ---------------------------- Static Variables --------------------------- */
/* ---------------------------- Global Variables --------------------------- */
CAppModule _Module;

/* ------------------------------- Prototypes ------------------------------ */
/* ---------------------------- Static Functions --------------------------- */

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
    HRESULT hRes = ::CoInitialize(NULL);
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    // HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

    hRes = _Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    int nRet = 0;
    {
        CMainDlg dlgMain;
        dlgMain.DoModal();
    }

    _Module.Term();
    ::CoUninitialize();

    return nRet;
}