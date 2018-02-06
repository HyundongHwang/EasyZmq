#pragma once
#include "FirstMock.h"



////////////////////////////////////////////////////////////////////////////////
// EasyZmq 수신 콜백함수
////////////////////////////////////////////////////////////////////////////////
typedef int(*LP_EASYZMQ_ONPUSH)(
    LPCWSTR wRequest,               // 콜백 수신전문
    LPWSTR wResponse,               // 콜백 응답전문
    int nResponseLength);           // 콜백 응답전문의 길이



////////////////////////////////////////////////////////////////////////////////
// EasyZmq 초기화함수
////////////////////////////////////////////////////////////////////////////////
typedef int(*LP_EASYZMQ_INIT)(
    int nMyPort,                        // 내 대기 포트
    int nOtherPort,                     // 상대측 접속 포트
    LP_EASYZMQ_ONPUSH pFuncOnPush);     // 콜백수신함수 포인터



////////////////////////////////////////////////////////////////////////////////
// EasyZmq 요청함수
////////////////////////////////////////////////////////////////////////////////
typedef int(*LP_EASYZMQ_REQUEST)(
    LPCWSTR wRequest,               // 요청전문
    LPWSTR wResponse,               // 응답전문
    int nResponseLength,            // 응답전문의 길이
    int nTimeout);                  // 타임아웃 milli seconds



static const int EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL = 300 * 1024;



class MainWnd;
typedef void(*LP_FUNC_SIMPLE)(MainWnd* pThis);

struct CSTRING_FUNC_SIMPLE_PAIR
{
    CSTRING_FUNC_SIMPLE_PAIR(CString strLabelNew, LP_FUNC_SIMPLE pFuncSimpleNew)
    {
        strLabel = strLabelNew;
        pFuncSimple = pFuncSimpleNew;
    }

    CString strLabel;
    LP_FUNC_SIMPLE pFuncSimple;
};



class MainWnd
    : public CWindowImpl<MainWnd>
{
public:
    const int ID_EDIT_LOG = 2000;

    MainWnd();
    virtual ~MainWnd();

    DECLARE_WND_CLASS(L"MainWnd")

    BEGIN_MSG_MAP(MainWnd)
        MESSAGE_HANDLER(WM_CREATE, _On_WM_CREATE)
        MESSAGE_HANDLER(WM_DESTROY, _On_WM_DESTROY)
        MESSAGE_HANDLER(WM_SIZE, _On_WM_SIZE)
        COMMAND_CODE_HANDLER(BN_CLICKED, _On_ID_BTN)
    END_MSG_MAP()

    static MainWnd* s_pCurrent;

private:
    static int _EASYZMQ_OnPush(LPCWSTR wRequest, LPWSTR wResponse, int nResponseLength);

    bool _LoadLibrary();
    void _UnloadLibrary();
    void _WriteLog(LPCWSTR wLog);
    void _ReadFile(LPCWSTR wFileName, LPWSTR wFileContent, int nFileContent);
    void _CallRequestApi(LPCWSTR wFileName, int nTimeout);

    LRESULT _On_WM_CREATE(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT _On_WM_DESTROY(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT _On_WM_SIZE(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT _On_ID_BTN(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

    static void _OnClickBtn_EASYZMQ_INIT(MainWnd* pThis);
    static void _OnClickBtn_EASYZMQ_REQUEST_hello(MainWnd* pThis);
    static void _OnClickBtn_Clear(MainWnd* pThis);

    CWindow m_edLog;

    HMODULE m_hDll;

    LP_EASYZMQ_INIT m_pFunc_EASYZMQ_INIT;
    LP_EASYZMQ_REQUEST m_pFunc_EASYZMQ_REQUEST;

    CAtlArray<CSTRING_FUNC_SIMPLE_PAIR> m_arrLabelFuncSimple;
};
