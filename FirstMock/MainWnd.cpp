#include "stdafx.h"
#include "MainWnd.h"



MainWnd* MainWnd::s_pCurrent = NULL;

MainWnd::MainWnd()
{
    s_pCurrent = this;
    _LoadLibrary();
}


MainWnd::~MainWnd()
{
    _UnloadLibrary();
}

bool MainWnd::_LoadLibrary()
{
    m_hDll = ::LoadLibrary(L"EasyZmq.dll");
    m_pFunc_EASYZMQ_INIT = (LP_EASYZMQ_INIT)GetProcAddress(m_hDll, "EASYZMQ_Init");
    m_pFunc_EASYZMQ_REQUEST = (LP_EASYZMQ_REQUEST)GetProcAddress(m_hDll, "EASYZMQ_Request");

    return true;
}

void MainWnd::_UnloadLibrary()
{
    if (m_hDll == NULL)
    {
        ::FreeLibrary(m_hDll);
        m_hDll = NULL;
    }
}

LRESULT MainWnd::_On_WM_CREATE(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    USES_CONVERSION;

    auto x = 0;
    auto y = 0;
    auto w = 500;
    auto h = 30;

    m_arrLabelFuncSimple.Add(CSTRING_FUNC_SIMPLE_PAIR(L"EASYZMQ_INIT", MainWnd::_OnClickBtn_EASYZMQ_INIT));
    m_arrLabelFuncSimple.Add(CSTRING_FUNC_SIMPLE_PAIR(L"EASYZMQ_REQUEST hello", MainWnd::_OnClickBtn_EASYZMQ_REQUEST_hello));
    m_arrLabelFuncSimple.Add(CSTRING_FUNC_SIMPLE_PAIR(L"Clear", MainWnd::_OnClickBtn_Clear));

    for (auto i = 0; i < m_arrLabelFuncSimple.GetCount(); i++)
    {
        auto pair = m_arrLabelFuncSimple[i];
        auto strBtnName = pair.strLabel;
        auto nBtnId = i;
        CWindow wndBtn;
        wndBtn.Create(L"Button", this->m_hWnd, CRect(x, y, x + w, y + h), strBtnName, WS_CHILD | WS_VISIBLE | BS_LEFT, NULL, nBtnId);
        y += h;
    }

    m_edLog.Create(L"Edit", this->m_hWnd, CRect(0, 0, 0, 0), L"로그", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN, NULL, 1001);
    return 0;
}

LRESULT MainWnd::_On_WM_DESTROY(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::PostQuitMessage(0);
    return 0;
}

LRESULT MainWnd::_On_WM_SIZE(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CRect rcClient;
    ::GetClientRect(m_hWnd, &rcClient);
    m_edLog.MoveWindow(0, rcClient.Height() / 2, rcClient.Width(), rcClient.Height() / 2);
    return 0;
}

int MainWnd::_EASYZMQ_OnPush(LPCWSTR wRequest, LPWSTR wResponse, int nResponseLength)
{
    MainWnd::s_pCurrent->_WriteLog(CString(L"CALLBACK ME <- OTHER : ") + wRequest);

    if (::lstrcmp(wRequest, L"hello안녕") == 0)
    {
        ::lstrcpy(wResponse, L"world세계");
    }
    else
    {
        ::lstrcpy(wResponse, L"unknown !!!");
    }

    MainWnd::s_pCurrent->_WriteLog(CString(L"CALLBACK ME -> OTHER : ") + wResponse);
    return 0;
}

void MainWnd::_WriteLog(LPCWSTR wLog)
{
    USES_CONVERSION;

    CString strEd;
    m_edLog.GetWindowText(strEd);
    strEd += L"\r\n";
    strEd += wLog;
    m_edLog.SetWindowText(strEd);
}

void MainWnd::_ReadFile(LPCWSTR wFileName, LPWSTR wFileContent, int nFileContent)
{
    USES_CONVERSION;
    wchar_t wJsonFilePath[MAX_PATH] = { 0, };
    ::GetModuleFileName(NULL, wJsonFilePath, MAX_PATH);
    ::PathRemoveFileSpec(wJsonFilePath);
    ::PathAppend(wJsonFilePath, wFileName);
    CAtlFile file;
    file.Create(wJsonFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
    file.Read(wFileContent, nFileContent);
    file.Close();
}

void MainWnd::_CallRequestApi(LPCWSTR wFileName, int nTimeout)
{
    wchar_t wContent[EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL / sizeof(wchar_t)] = { 0, };
    _ReadFile(wFileName, wContent, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL);
    _WriteLog(CString(L"REQ : ") + wContent);
    wchar_t wResponse[EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL / sizeof(wchar_t)] = { 0, };
    m_pFunc_EASYZMQ_REQUEST(wContent, wResponse, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL, nTimeout);
    _WriteLog(CString(L"RES : ") + wResponse);
}

LRESULT MainWnd::_On_ID_BTN(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    auto nBtnId = wID;
    auto pair = m_arrLabelFuncSimple[nBtnId];
    pair.pFuncSimple(this);
    return 0;
}

void MainWnd::_OnClickBtn_EASYZMQ_REQUEST_hello(MainWnd* pThis)
{
    wchar_t* wRequest = L"hello안녕";
    wchar_t wResponse[EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL] = { 0, };
    pThis->_WriteLog(CString(L"CALL ME -> OTHER : ") + wRequest);
    pThis->m_pFunc_EASYZMQ_REQUEST(wRequest, wResponse, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL, 3000);
    pThis->_WriteLog(CString(L"CALL ME <- OTHER : ") + wResponse);
}

void MainWnd::_OnClickBtn_EASYZMQ_INIT(MainWnd* pThis)
{
    auto nResult = pThis->m_pFunc_EASYZMQ_INIT(1000, 1001, _EASYZMQ_OnPush);
    CString wMsg = L"";
    wMsg.Format(L"INIT : %s", nResult == 0 ? L"성공" : L"실패");
    pThis->_WriteLog(wMsg);
}

void MainWnd::_OnClickBtn_Clear(MainWnd* pThis)
{
    pThis->m_edLog.SetWindowText(L"");
}
