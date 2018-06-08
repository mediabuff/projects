/* ------------------------------- Includes -------------------------------- */
#include "stdafx.h"
#include "Resource.h"

#include "MainDlg.h"

/* -------------------------------- Macros --------------------------------- */
/* ------------------------------- TypeDefs -------------------------------- */
/* -------------------------------- Classes -------------------------------- */
/* ---------------------------- Static Variables --------------------------- */
static HINSTANCE g_hInstance;
static HWND g_hMainDlg;
static std::thread *pThread;

/* ---------------------------- Global Variables --------------------------- */
/* ------------------------------- Prototypes ------------------------------ */
extern void start(const char cubeState[]);

/* ---------------------------- Static Functions --------------------------- */

/******************************************************************************
*
* Class CMainDlg implementation
*
*****************************************************************************/
CMainDlg::CMainDlg()
{
}

void CMainDlg::write_line(const char *message)
{
    HWND hControl = ::GetDlgItem(g_hMainDlg, IDC_OUTPUT);
    auto length = ::SendMessageA(hControl, WM_GETTEXTLENGTH, 0, 0);
    std::string outputText;
    if (length > 0)
    {
        char *buff = reinterpret_cast<char *>(alloca(length + 2 + 1));
        ::SendMessageA(hControl, WM_GETTEXT, length + 1, reinterpret_cast<LPARAM>(buff));
        buff[length] = '\r';
        buff[length + 1] = '\n';
        buff[length + 2] = 0;
        outputText = buff;
    }
    outputText += message;
    ::SendMessageA(hControl, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(outputText.c_str()));
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    g_hMainDlg = m_hWnd;
    // center the dialog on the screen
    CenterWindow();

    // set icons
    HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_RUBIKS_CUBE),
        IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_RUBIKS_CUBE),
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);

    CEdit cubeStateEdit = GetDlgItem(IDC_CUBESTATE_EDIT);
    cubeStateEdit.LimitText(24);
    cubeStateEdit.SetWindowTextA("rrrrbbbbwwwwggggyyyyoooo");
    return TRUE;
}

LRESULT CMainDlg::OnStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
{
    if (pThread == nullptr)
    {
        CButton startButton(hWndCtl);
        startButton.EnableWindow(FALSE);

        static char cubeState[24 + 1];
        CEdit cubeStateEdit = GetDlgItem(IDC_CUBESTATE_EDIT);
        cubeStateEdit.GetWindowTextA(cubeState, sizeof(cubeState));
        pThread = new std::thread(/*start_wrapper*/[](const char *_cubeState)
        {
            start(_cubeState);
            ::PostMessage(g_hMainDlg, WM_THREAD_DONE, 0, 0);
        }, cubeState);
    }
    return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(wID);
    return 0;
}

LRESULT CMainDlg::OnThreadDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    pThread->join();
    delete pThread;
    pThread = nullptr;
    CButton startButton = GetDlgItem(IDC_START_BTN);
    startButton.EnableWindow();
    return 0;
}
