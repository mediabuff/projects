#include "stdafx.h"

#include <string>
#include <thread>
#include "Resource.h"

const UINT THREAD_DONE = WM_APP;

extern void start(const char cubeState[]);

static HINSTANCE g_hInstance;
static HWND g_hMainDlg;

void write_line(const char *message)
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

static void start_wrapper(const char *cubeState)
{
    start(cubeState);
    PostMessage(g_hMainDlg, THREAD_DONE, 0, 0);
}

static void center_window(HWND hWnd)
{
    WINDOWINFO winInfo;
    winInfo.cbSize = sizeof(winInfo);
    ::GetWindowInfo(hWnd, &winInfo);
    WINDOWINFO desktopInfo;
    desktopInfo.cbSize = sizeof(desktopInfo);
    ::GetWindowInfo(::GetDesktopWindow(), &desktopInfo);

    auto x = ((desktopInfo.rcWindow.right - desktopInfo.rcWindow.left) - (winInfo.rcWindow.right - winInfo.rcWindow.left)) / 2;
    auto y = ((desktopInfo.rcWindow.bottom - desktopInfo.rcWindow.top) - (winInfo.rcWindow.bottom - winInfo.rcWindow.top)) / 2;
    ::SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

static void set_dialog_icon()
{
    auto hIcon = LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_RUBIKS_CUBE), IMAGE_ICON,
        ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    if (hIcon)
    {
        SendMessage(g_hMainDlg, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
    }

    // Load the 128 x 128 icon for the larger icon displayed during the Alt+Tab 
    hIcon = LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_RUBIKS_CUBE), IMAGE_ICON, 128, 128, 0);
    if (hIcon)
    {
        SendMessage(g_hMainDlg, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
    }
}

INT_PTR CALLBACK MainDlgCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static std::thread *pThread;

    switch (message)
    {
    case WM_INITDIALOG:
        g_hMainDlg = hDlg;
        SendDlgItemMessage(hDlg, IDC_CUBESTATE_EDIT, EM_SETLIMITTEXT, 24, 0);
        ::SetDlgItemTextA(hDlg, IDC_CUBESTATE_EDIT, "rrrrbbbbwwwwggggyyyyoooo");
        center_window(hDlg);
        set_dialog_icon();
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            ::EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_START_STOP_BTN)
        {
            if (pThread == nullptr)
            {
                ::EnableWindow(::GetDlgItem(hDlg, IDC_START_STOP_BTN), FALSE);

                static char cubeState[24 + 1];
                ::GetDlgItemTextA(hDlg, IDC_CUBESTATE_EDIT, cubeState, sizeof(cubeState));
                pThread = new std::thread(start_wrapper, cubeState);
            }
        }
        break;

    case THREAD_DONE:
        pThread->join();
        delete pThread;
        pThread = nullptr;
        ::EnableWindow(::GetDlgItem(hDlg, IDC_START_STOP_BTN), TRUE);
        break;
    }

    return FALSE;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), nullptr, MainDlgCallback);
    return 0;
}
