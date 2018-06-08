#pragma once
/* ------------------------------- Includes -------------------------------- */
/* -------------------------------- Macros --------------------------------- */
/* ---------------------------- Global Variables --------------------------- */
const UINT WM_THREAD_DONE = WM_APP;

/* -------------------------------- Classes -------------------------------- */

/******************************************************************************
*
* Class CMainDlg declaration
*
*****************************************************************************/
class CMainDlg : public CDialogImpl<CMainDlg>
{
    typedef CDialogImpl<CMainDlg> Parent;

public:
    enum { IDD = IDD_MAINDLG };

    CMainDlg();

    static void write_line(const char *message);

    BEGIN_MSG_MAP(CMainDlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER(IDC_START_BTN, BN_CLICKED, OnStart)
        COMMAND_HANDLER(IDOK, BN_CLICKED, OnCancel)
        COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel)
        MESSAGE_HANDLER(WM_THREAD_DONE, OnThreadDone)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    LRESULT OnStart(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnThreadDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
