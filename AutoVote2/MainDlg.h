#pragma once
/* ------------------------------- Includes -------------------------------- */
/* -------------------------------- Macros --------------------------------- */
/* ---------------------------- Global Variables --------------------------- */
const UINT WM_VOTEREGISTERED = WM_APP;

/* -------------------------------- Classes -------------------------------- */
struct VOTETHREADPARAMS
{
    HWND hDlg;
    bool stopRequested;
    // Parameters loaded from the registry/formatted, parameters to the thread
    CString urlFormat;						// Format string for the URL (%s for the option chosen)
    CString referer;
    CString server;
    CString url;							// Not loaded, this is the formatted URL with the option
    CString option1;
    CString result1;
    CString indicator1;						// Not loaded, this is result1 + "<br>"
    CString option2;
    CString result2;
    CString indicator2;						// Not loaded, this is result2 + "<br>"
    // Results loaded from response, parameters from thread in VM_VOTEREGISTERED
    CString voteResult1;
    CString voteResult2;
};

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

    BEGIN_MSG_MAP(CMainDlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER(ID_APP_ABOUT, BN_CLICKED, OnAppAbout)
        COMMAND_HANDLER(IDC_START_STOP_VOTING, BN_CLICKED, OnBnClickedStartStopVoting)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        MESSAGE_HANDLER(WM_VOTEREGISTERED, OnVoteRegistered);
        MSG_WM_SYSCOMMAND(OnSysCommand);
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedStartStopVoting(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnVoteRegistered(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    void OnSysCommand(UINT nID, CPoint point);

private:
    HANDLE m_hVoteThread;
    VOTETHREADPARAMS m_voteThreadParams;
    CString m_textStartVoting;
    CString m_textStopVoting;
};
