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
    auto wId = 1000;

    CWindow wndS0;
    wndS0.Create(L"Static", this->m_hWnd, CRect(0, y, 100, y + h), L"MyPort : ", WS_CHILD | WS_VISIBLE, NULL, wId++);
    m_edMyPort.Create(L"Edit", this->m_hWnd, CRect(100, y, 200, y + h), L"", WS_CHILD | WS_VISIBLE | WS_BORDER, NULL, wId++);
    CWindow wndS1;
    wndS1.Create(L"Static", this->m_hWnd, CRect(200, y, 3000, y + h), L"OtherPort : ", WS_CHILD | WS_VISIBLE, NULL, wId++);
    m_edOtherPort.Create(L"Edit", this->m_hWnd, CRect(300, y, 400, y + h), L"", WS_CHILD | WS_VISIBLE | WS_BORDER, NULL, wId++);
    y += h;

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

    m_edLog.Create(L"Edit", this->m_hWnd, CRect(0, 0, 0, 0), L"로그", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN, NULL, wId++);
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

void MainWnd::_OnClickBtn_Clear(MainWnd* pThis)
{
    pThis->m_edLog.SetWindowText(L"");
}



void MainWnd::_OnClickBtn_EASYZMQ_INIT(MainWnd* pThis)
{
    auto nResult = 0;
    wchar_t wTmp[1024] = { 0, };
    ::GetWindowText(pThis->m_edMyPort, wTmp, 1024);
    auto nMyPort = ::_wtoi(wTmp);
    ::GetWindowText(pThis->m_edOtherPort, wTmp, 1024);
    auto nOtherPort = ::_wtoi(wTmp);

    {
        CString wMsg = L"";
        wMsg.Format(L"INIT : nMyPort(%d) nOtherPort(%d)", nMyPort, nOtherPort);
        pThis->_WriteLog(wMsg);
    }

    nResult = pThis->m_pFunc_EASYZMQ_INIT(nMyPort, nOtherPort, _EASYZMQ_OnPush);

    {
        CString wMsg = L"";

        if (nResult >= 0)
        {
            wMsg.Format(L"INIT : 성공");
        }
        else
        {
            wMsg.Format(L"INIT : 실패 : %d", nResult);
        }

        pThis->_WriteLog(wMsg);
    }
}

void MainWnd::_OnClickBtn_EASYZMQ_REQUEST_hello(MainWnd* pThis)
{
    wchar_t* wRequest = L"hello안녕";
    wchar_t wResponse[EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL] = { 0, };
    pThis->_WriteLog(CString(L"CALL ME -> OTHER : ") + wRequest);
    auto nResult = pThis->m_pFunc_EASYZMQ_REQUEST(wRequest, wResponse, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL, 3000);
    CString wMsg = L"";

    if (nResult >= 0)
    {
        pThis->_WriteLog(CString(L"CALL ME <- OTHER : ") + wResponse);
    }
    else
    {
        wMsg.Format(L"CALL : 실패 : %d", nResult);
    }

    pThis->_WriteLog(wMsg);
}