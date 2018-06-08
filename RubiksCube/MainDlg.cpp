/* ------------------------------- Includes -------------------------------- */
#include "stdafx.h"
#include "Resource.h"

#include "MainDlg.h"

/* -------------------------------- Macros --------------------------------- */
/* ------------------------------- TypeDefs -------------------------------- */
/* -------------------------------- Classes -------------------------------- */
/* ---------------------------- Static Variables --------------------------- */
CWindow CMainDlg::s_MainDlg;
std::thread *CMainDlg::s_pThread;

/* ---------------------------- Global Variables --------------------------- */
const char SOLVED_CUBE_STATE[] = "rrrrbbbbwwwwggggyyyyoooo";

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

void CMainDlg::WriteLine(const char *message)
{
    CEdit outputEdit = s_MainDlg.GetDlgItem(IDC_OUTPUT);
    CString outputText;
    outputEdit.GetWindowTextA(outputText);
    if (outputText.GetLength() > 0)
    {
        outputText.Append("\r\n");
    }
    outputText.Append(message);
    outputEdit.SetWindowTextA(outputText);
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    s_MainDlg = *this;

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
    cubeStateEdit.SetWindowTextA(SOLVED_CUBE_STATE);
    return TRUE;
}

LRESULT CMainDlg::OnStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
{
    if (s_pThread == nullptr)
    {
        CButton startButton(hWndCtl);
        startButton.EnableWindow(FALSE);

        static char cubeState[24 + 1];
        CEdit cubeStateEdit = GetDlgItem(IDC_CUBESTATE_EDIT);
        cubeStateEdit.GetWindowTextA(cubeState, sizeof(cubeState));
        s_pThread = new std::thread(/*start_wrapper*/[](const char *_cubeState)
        {
            start(_cubeState);
            s_MainDlg.PostMessage(WM_THREAD_DONE);
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
    s_pThread->join();
    delete s_pThread;
    s_pThread = nullptr;
    CButton startButton = GetDlgItem(IDC_START_BTN);
    startButton.EnableWindow();
    return 0;
}
