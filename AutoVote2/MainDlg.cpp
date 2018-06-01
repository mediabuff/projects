/* ------------------------------- Includes -------------------------------- */
#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"

/* -------------------------------- Macros --------------------------------- */
//
// http://pbskids.org/cgi-registry/poll/poll.pl?results=arthur/games/sugarbowlpoll/results.tmpl&site_id=arthur&poll_id=music3&question1.exists=true&q1=2
//
/*const wchar_t VOTE_REFERER[] = L"http://pbskids.org/arthur/games/sugarbowlpoll/index.html";
const wchar_t VOTE_SERVER[] = L"pbskids.org";									// URL Server (http is implied)
const wchar_t VOTE_URL[] = L"/cgi-registry/poll/poll.pl"						// URL Object
						   L"?results=arthur/games/sugarbowlpoll/results.tmpl"	// Parameters
						   L"&site_id=arthur"
						   L"&poll_id=food_texture"
						   L"&question1.exists=true"
						   L"&q1=%s";

#define VOTE_OPTION_1 _T("1")											// Value for option 1
#define VOTE_RESULT_1 _T("Soft")										// Used for field name to display results and
const TCHAR VOTE_RESULT_INDICATOR1[] = VOTE_RESULT_1 _T("<br>");		//  to find results in the response (usually field name followed by <br>)
#define VOTE_OPTION_2 _T("2")											// Value for option 2
#define VOTE_RESULT_2 _T("Crunchy")										// Used for field name to display results and
const TCHAR VOTE_RESULT_INDICATOR2[] = VOTE_RESULT_2 _T("<br>");		//  to find results in the response (usually field name followed by <br>)*/

/* ------------------------------- TypeDefs -------------------------------- */
/* -------------------------------- Classes -------------------------------- */
/* ---------------------------- Static Variables --------------------------- */
/* ---------------------------- Global Variables --------------------------- */
/* ------------------------------- Prototypes ------------------------------ */
/* ---------------------------- Static Functions --------------------------- */
static void VoteOnce(const VOTETHREADPARAMS &voteParams, CString *pResultHtml)
{
	// TLG: I copied this basic code from a MSDN sample, don't really like the flow or the error handling

	// Create a session
	HINTERNET hSession = ::WinHttpOpen(
		L"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)", // IE7 on XP agent string
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,	// Access type [WINHTTP_ACCESS_TYPE_DEFAULT_PROXY]
		WINHTTP_NO_PROXY_NAME,				// Proxy [WINHTTP_NO_PROXY_NAME]
		WINHTTP_NO_PROXY_BYPASS,			// Proxy bypass [WINHTTP_NO_PROXY_BYPASS]
		0);									// Flags [WINHTTP_FLAG_ASYNC]

	// Build an HTTP/HTTPS connection
	HINTERNET hConnect = NULL;
	if(hSession)
	{
		hConnect = ::WinHttpConnect(hSession,
			voteParams.server,				// Host
			INTERNET_DEFAULT_HTTP_PORT,		// Port [or INTERNET_DEFAULT_HTTPS_PORT]
			0);								// Reserved
	}

	// Create an HTTP request handle
	HINTERNET hRequest = NULL;
	if(hConnect)
	{
		hRequest = ::WinHttpOpenRequest(hConnect,
			L"GET",							// Verb
			voteParams.url,					// Object name
			NULL,							// Version
			voteParams.referer,				// Referer [WINHTTP_NO_REFERER]
			WINHTTP_DEFAULT_ACCEPT_TYPES,	// Accept types [WINHTTP_DEFAULT_ACCEPT_TYPES]
			0);								// Flags [WINHTTP_FLAG_SECURE, WINHTTP_FLAG_REFRESH, etc]
	}

	// Build/send the request
	BOOL bSuccess = FALSE;
	if(hRequest)
	{
		bSuccess = ::WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,	// Headers [WINHTTP_NO_ADDITIONAL_HEADERS]
			0,								// Headers length (or -1 if NUL terminated)
			WINHTTP_NO_REQUEST_DATA,		// Optional request data [WINHTTP_NO_REQUEST_DATA]
			0,								// Optional length
			0,								// Total length
			0);								// Context (will be passed to callbacks)
	}
	// End the request, we want to receive data now
	if(bSuccess)
		bSuccess = ::WinHttpReceiveResponse(hRequest, NULL);

	// Keep checking for data until there is nothing left
	if(bSuccess)
	{
		DWORD dwBytesAvailable = 0;
		do
		{
			// Check for available data
			if(!WinHttpQueryDataAvailable(hRequest, &dwBytesAvailable))
			{
				dwBytesAvailable = 0;
				ATLTRACE(_T("Error calling WinHttpQueryDataAvailable(), win32Err = %d\n"), ::GetLastError());
			}

			if(dwBytesAvailable > 0)
			{
				std::auto_ptr<char> pszOutBuffer(new char[dwBytesAvailable+1]);
				if(pszOutBuffer.get() == NULL)
				{
					ATLTRACE(_T("Error allocating memory for reading response\n"));
					dwBytesAvailable = 0;
				}
				else
				{
					// Read the data.
					ZeroMemory(pszOutBuffer.get(), dwBytesAvailable+1);

					DWORD dwNumBytesRead = 0;
					if(!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer.get(), dwBytesAvailable, &dwNumBytesRead))
						ATLTRACE(_T("Error calling WinHttpReadData(), win32Err = %d\n"), ::GetLastError());
					else
						*pResultHtml += pszOutBuffer.get();
				}
			}
		}
		while(dwBytesAvailable > 0);
	}

	// Report any errors.
	if(!bSuccess)
		ATLTRACE(_T("Error calling WinHttp API, win32Err = %d\n"), ::GetLastError());

	// Close any open handles.
	if(hRequest)
		::WinHttpCloseHandle(hRequest);
	if(hConnect)
		::WinHttpCloseHandle(hConnect);
	if(hSession)
		::WinHttpCloseHandle(hSession);
}

// Search resultHtml for 'indicator', copy text up to first '<' back to 'pResult'
static void CopyResult(const CString &resultHtml, const TCHAR *indicator, CString *pResult)
{
	int iStart = resultHtml.Find(indicator);
	if(iStart == -1)
	{
		pResult->Empty();
		return;
	}
	iStart += static_cast<int>(_tcslen(indicator));
	int iEnd = resultHtml.Find(_T('<'), iStart);
	if(iStart == -1)
		iEnd = iStart + 6;
	*pResult = resultHtml.Mid(iStart, iEnd - iStart);
}

static DWORD WINAPI VoteThread(LPVOID lpThreadParameter)
{
	VOTETHREADPARAMS *pVoteThreadParams = reinterpret_cast<VOTETHREADPARAMS *>(lpThreadParameter);
	while(!pVoteThreadParams->stopRequested)
	{
		CString resultHtml;

		::VoteOnce(*pVoteThreadParams, &resultHtml);
		::CopyResult(resultHtml, pVoteThreadParams->indicator1, &pVoteThreadParams->voteResult1);
		::CopyResult(resultHtml, pVoteThreadParams->indicator2, &pVoteThreadParams->voteResult2);
		::PostMessage(pVoteThreadParams->hDlg, WM_VOTEREGISTERED, 0, 0);
		Sleep(500);
	}
	return 0;
}

static bool RegQueryStringValue(CRegKey *pKey, const TCHAR *valueName, CString *pValue, size_t maxLen)
{
	ULONG numChars = maxLen+1;
	if(pKey->QueryStringValue(valueName, pValue->GetBufferSetLength(maxLen+1), &numChars) != ERROR_SUCCESS)
	{	// Note: Doesn't handle strings that are larger than the requested maximum
		pValue->ReleaseBuffer(0);
		return false;
	}
	pValue->ReleaseBufferSetLength(numChars-1);
	return true;
}

/******************************************************************************
 *
 * Class CMainDlg implementation
 *
 *****************************************************************************/
CMainDlg::CMainDlg()
	: m_hVoteThread(NULL), m_textStartVoting(_T("&Start Voting")), m_textStopVoting(_T("&Stop Voting"))
{
	CRegKey keySoftware;
	if(keySoftware.Open(HKEY_CURRENT_USER, _T("Software")) == ERROR_SUCCESS)
	{
		CRegKey keyAutoVote;
		if(keyAutoVote.Create(keySoftware, _T("AutoVote")) == ERROR_SUCCESS)
		{
			RegQueryStringValue(&keyAutoVote, _T("UrlFormat"), &m_voteThreadParams.urlFormat, 1024);
			RegQueryStringValue(&keyAutoVote, _T("Referer"), &m_voteThreadParams.referer, 1024);
			RegQueryStringValue(&keyAutoVote, _T("Server"), &m_voteThreadParams.server, 256);
			RegQueryStringValue(&keyAutoVote, _T("Option1"), &m_voteThreadParams.option1, 256);
			RegQueryStringValue(&keyAutoVote, _T("Result1"), &m_voteThreadParams.result1, 256);
			m_voteThreadParams.indicator1 = m_voteThreadParams.result1 + _T("<br>");
			RegQueryStringValue(&keyAutoVote, _T("Option2"), &m_voteThreadParams.option2, 256);
			RegQueryStringValue(&keyAutoVote, _T("Result2"), &m_voteThreadParams.result2, 256);
			m_voteThreadParams.indicator2 = m_voteThreadParams.result2 + _T("<br>");
		}
	}
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	CButton radioOption = GetDlgItem(IDC_RADIO_OPTION1);
	radioOption.SetWindowText(m_voteThreadParams.result1);
	radioOption.SetCheck(IDC_RADIO_OPTION1);

	radioOption = GetDlgItem(IDC_RADIO_OPTION2);
	radioOption.SetWindowText(m_voteThreadParams.result2);

	return TRUE;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnBnClickedStartStopVoting(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_hVoteThread == NULL)
	{
		CButton radioOption1 = GetDlgItem(IDC_RADIO_OPTION1);
		int check1 = radioOption1.GetCheck();

		CButton radioOption2 = GetDlgItem(IDC_RADIO_OPTION2);
		int check2 = radioOption2.GetCheck();

		const TCHAR *option = NULL;
		if(check1)
			option = m_voteThreadParams.option1;
		else if(check2)
			option = m_voteThreadParams.option2;
		if(option == NULL)
			return 0;

		m_voteThreadParams.url.Format(m_voteThreadParams.urlFormat, option);
		m_voteThreadParams.hDlg = (HWND) *this;
		m_voteThreadParams.stopRequested = false;
		m_hVoteThread = ::CreateThread(NULL, 0, VoteThread, &m_voteThreadParams, 0, NULL);
		if(m_hVoteThread != NULL)
		{
			GetDlgItem(IDC_START_STOP_VOTING).SetWindowText(m_textStopVoting);
			radioOption1.EnableWindow(FALSE);
			radioOption2.EnableWindow(FALSE);
		}
	}
	else
	{
		m_voteThreadParams.stopRequested = true;
		::WaitForSingleObject(m_hVoteThread, INFINITE);
		::CloseHandle(m_hVoteThread);
		m_hVoteThread = NULL;
		GetDlgItem(IDC_START_STOP_VOTING).SetWindowText(m_textStartVoting);
		GetDlgItem(IDC_RADIO_OPTION1).EnableWindow();
		GetDlgItem(IDC_RADIO_OPTION2).EnableWindow();
	}
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnVoteRegistered(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDC_RESULT1).SetWindowText(m_voteThreadParams.voteResult1);
	GetDlgItem(IDC_RESULT2).SetWindowText(m_voteThreadParams.voteResult2);
	return 0;
}

void CMainDlg::OnSysCommand(UINT nID, CPoint point)
{
	::OutputDebugString(_T("OnSysCommandCalled()\n"));
	if(nID == SC_SCREENSAVE)
		::OutputDebugString(_T("SC_SCREENSAVE detected\n"));
	// Default is for messages to be handled, tell WTL we didn't handle this one
	SetMsgHandled(FALSE);
}
