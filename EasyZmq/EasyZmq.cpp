// EasyZmq.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "EasyZmq.h"



static LP_EASYZMQ_ONPUSH s_pFuncOnPush = NULL;



static HANDLE s_hZmqRepThread = NULL;
static void* s_pZmqContextReq = NULL;
static void* s_pZmqContextRep = NULL;
static void* s_pSocketReq = NULL;
static void* s_pSocketRep = NULL;



static const int EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL = 300 * 1024;



static int s_nOtherPort = 0;
static int s_nMyPort = 0;



static LPCWSTR RESPONSE_FAIL_CONNECTION_TIMEOUT = L"{ \r\n \
    \"code\": \"FAIL_CONNECTION_TIMEOUT\", \r\n \
    \"desc\": \"실패 !!! 연결중 타임아웃이 발생하였습니다. !!!\" \r\n \
}";



EASYZMQ_API int EASYZMQ_Init(
    int nMyPort,
    int nOtherPort,
    LP_EASYZMQ_ONPUSH pFuncOnPush)
{
    USES_CONVERSION;

    ScaUtil::WriteLog(L"EASYZMQ_Init start \n");

    s_nMyPort = nMyPort;
    s_nOtherPort = nOtherPort;



    CRegKey rkUninstall;
    LSTATUS lstResult = rkUninstall.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");

    bool bInstVcredist2010 = false;

    DWORD i = 0;
    wchar_t wKeyName[1024] = { 0, };
    DWORD dwLen = 1024;

    while (ERROR_NO_MORE_ITEMS != rkUninstall.EnumKey(i, wKeyName, &dwLen))
    {
        CRegKey rkApp;
        rkApp.Open(rkUninstall.m_hKey, wKeyName);
        wchar_t wDisplayNameValue[1024] = { 0, };
        DWORD dwLen2 = 1024;
        rkApp.QueryStringValue(L"DisplayName", wDisplayNameValue, &dwLen2);

        //wDisplayNameValue: Microsoft Visual C++ 2010  x86 Redistributable - 10.0.30319
        if (_IsProgramInstalled(wDisplayNameValue, L"2010", L"redistributable"))
        {
            bInstVcredist2010 = true;
        }


        if (bInstVcredist2010)
            break;

        dwLen = 1024;
        ZeroMemory(wKeyName, 1024);
        i++;
    }

    ScaUtil::WriteLog(L"EASYZMQ_Init bInstVcredist2010 : %d \n", bInstVcredist2010);



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //http://s3.ap-northeast-2.amazonaws.com/coconut-client/vcredist_x86-vs2010.exe
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (!bInstVcredist2010)
    {
        _DownloadAndExec(L"http://s3.ap-northeast-2.amazonaws.com/coconut-client/vcredist_x86-vs2010.exe", L"vcredist_x86-vs2010.exe", L"/q");
    }



    ScaUtil::WriteLog(L"EASYZMQ_Init zmq start \n");
    int nResult = 0;

    ScaUtil::WriteLog(L"EASYZMQ_Init s_nOtherPort : %d \n", s_nOtherPort);
    ScaUtil::WriteLog(L"EASYZMQ_Init s_nMyPort : %d \n", s_nMyPort);

    s_pFuncOnPush = pFuncOnPush;
    _CloseZmqContextSocketThread();
    s_pZmqContextReq = zmq_ctx_new();
    ScaUtil::WriteLog(L"EASYZMQ_Init s_pZmqContextReq : 0x%x \n", s_pZmqContextReq);

    s_pSocketReq = zmq_socket(s_pZmqContextReq, ZMQ_REQ);
    ScaUtil::WriteLog(L"EASYZMQ_Init s_pSocketReq : 0x%x \n", s_pSocketReq);

    s_pZmqContextRep = zmq_ctx_new();
    ScaUtil::WriteLog(L"EASYZMQ_Init s_pZmqContextRep : 0x%x \n", s_pZmqContextRep);

    s_pSocketRep = zmq_socket(s_pZmqContextRep, ZMQ_REP);
    ScaUtil::WriteLog(L"EASYZMQ_Init s_pSocketRep : 0x%x \n", s_pSocketRep);

    char szOtherCon[1024] = { 0, };
    sprintf(szOtherCon, "tcp://127.0.0.1:%d", s_nOtherPort);
    nResult = zmq_connect(s_pSocketReq, szOtherCon);

    if (nResult < 0)
    {
        _CloseZmqContextSocketThread();
        return nResult;
    }

    s_hZmqRepThread = (HANDLE)_beginthreadex(NULL, 0, _ZmqRepThreadFunc, NULL, 0, NULL);
    ScaUtil::WriteLog(L"EASYZMQ_Init end \n");
    return nResult;
}



EASYZMQ_API int EASYZMQ_Request(
    LPCWSTR wRequest,
    LPWSTR wResponse,
    int nResponseLength,
    int nTimeout)
{
    int nResult = _EASYZMQ_Request(wRequest, wResponse, nResponseLength, nTimeout);
    return nResult;
}



int _EASYZMQ_Request(
    LPCWSTR wRequest,
    LPWSTR wResponse, 
    int nResponseLength,
    int nTimeout)
{
    USES_CONVERSION;
    ScaUtil::WriteLog(L"EASYZMQ_Request start nTimeout : %d \n", nTimeout);
    int nResult = 0;



    wchar_t wPingRes[1024] = { 0, };
    nResult = _RequestResponse(L"ping", wPingRes, 1024, 3000);

    if (::lstrcmp(wPingRes, L"pong") != 0)
    {
        ::lstrcpy(wResponse, RESPONSE_FAIL_CONNECTION_TIMEOUT);
        nResult = _ResetReqSocket();
        return nResult;
    }



    nResult = _RequestResponse(wRequest, wResponse, nResponseLength, nTimeout);

    if (nResult < 0)
    {
        ::lstrcpy(wResponse, RESPONSE_FAIL_CONNECTION_TIMEOUT);
        nResult = _ResetReqSocket();
        return nResult;
    }



    ScaUtil::WriteLog(L"EASYZMQ_Request end \n");
    return nResult;
}



unsigned int WINAPI _ZmqRepThreadFunc(void* pParam)
{
    USES_CONVERSION;

    ScaUtil::WriteLog(L"_ZmqRepThreadFunc start \n");

    int nResult = 0;
    char szMyCon[1024] = { 0, };
    sprintf(szMyCon, "tcp://127.0.0.1:%d", s_nMyPort);
    nResult = zmq_bind(s_pSocketRep, szMyCon);
    wchar_t wRecv[EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL / sizeof(wchar_t)] = { 0, };
    wchar_t wResponse[EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL / sizeof(wchar_t)] = { 0, };

    while (true)
    {
        ::ZeroMemory(wRecv, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL);
        ::ZeroMemory(wResponse, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL);
        nResult = zmq_recv(s_pSocketRep, wRecv, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL, 0);
        ScaUtil::WriteLog(L"_ZmqRepThreadFunc ::lstrlen(szRecv) : %d \n", ::lstrlen(wRecv));

        if (::lstrcmp(wRecv, L"ping") == 0)
        {
            ::lstrcpy(wResponse, L"pong");
        }
        else
        {
            if (s_pFuncOnPush != NULL)
            {
                s_pFuncOnPush(wRecv, wResponse, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL);
            }
        }

        ScaUtil::WriteLog(L"EASYZMQ_Init ::lstrlen(szResponse) : %d \n", ::lstrlen(wResponse));
        nResult = zmq_send(s_pSocketRep, wResponse, EASYZMQ_BRIDGE_BUFFER_SIZE_NORMAL, 0);
    }

    return nResult;
}



void _CloseZmqContextSocketThread()
{
    ScaUtil::WriteLog(L"_CloseZmqContextSocketThread start \n");

    int nResult = 0;

    if (s_hZmqRepThread != NULL)
    {
        ::TerminateThread(s_hZmqRepThread, NULL);
        ::CloseHandle(s_hZmqRepThread);
        s_hZmqRepThread = NULL;
        ::Sleep(1000);
    }

    if (s_pSocketReq != NULL)
    {
        nResult = zmq_close(s_pSocketReq);
        s_pSocketReq = NULL;
    }

    if (s_pSocketRep != NULL)
    {
        nResult = zmq_close(s_pSocketRep);
        s_pSocketRep = NULL;
    }

    if (s_pZmqContextReq != NULL)
    {
        //nResult = zmq_ctx_term(s_pZmqContextReq);
        s_pZmqContextReq = NULL;
    }

    if (s_pZmqContextRep != NULL)
    {
        //nResult = zmq_ctx_term(s_pZmqContextRep);
        s_pZmqContextRep = NULL;
    }

    ScaUtil::WriteLog(L"_CloseZmqContextSocketThread end \n");
}



int _GetValidByteCountFromCharPtr(char* pSz)
{
    byte* pBuf = (byte*)pSz;
    int lengthStr = ::strlen(pSz);

    for (int i = 0; i < lengthStr * 3; i++)
    {
        if (pBuf[i] == '\0')
            return i + 1;
    }

    return -1;
}



bool _IsProgramInstalled(LPWSTR wDisplayName, LPCWSTR wComp, LPCWSTR wComp2)
{
    if (StrStrIW(wDisplayName, wComp) == NULL ||
        StrStrIW(wDisplayName, wComp2) == NULL)
        return false;

    return true;
}



void _DownloadAndExec(LPCWSTR wUrl, LPCWSTR wFileName, LPCWSTR wOption)
{
    USES_CONVERSION;

    _DownloadAndExecInfo* pInfo = new _DownloadAndExecInfo;
    pInfo->wUrl = wUrl;
    pInfo->wFileName = wFileName;
    pInfo->wOption = wOption;
    _beginthreadex(NULL, 0, _DownloadAndExecThreadFunc, pInfo, 0, NULL);
}



unsigned int WINAPI _DownloadAndExecThreadFunc(void* pParam)
{
    USES_CONVERSION;

    _DownloadAndExecInfo* pInfo = (_DownloadAndExecInfo*)pParam;

    wchar_t wTmpFilePath[MAX_PATH] = { 0, };
    ::GetTempPath(MAX_PATH, wTmpFilePath);
    ::PathCombine(wTmpFilePath, wTmpFilePath, pInfo->wFileName);
    ScaUtil::HttpDownload(pInfo->wUrl, wTmpFilePath);
    char szCmd[1024] = { 0, };
    sprintf(szCmd, "\"%s\" %s", W2A(wTmpFilePath), W2A(pInfo->wOption));
    ::WinExec(szCmd, SW_SHOW);

    delete pInfo;

    return 0;
}






int _RequestResponse(LPCWSTR wRequest, LPWSTR wResponse, int nResponseLength, int nTimeout)
{
    USES_CONVERSION;

    int nResult = 0;
    int lengthReq = 0;

    //zmq 
    nResult = zmq_setsockopt(s_pSocketReq, ZMQ_RCVTIMEO, &nTimeout, sizeof(int));

    lengthReq = (::lstrlen(wRequest) + 1) * sizeof(wchar_t);

    ScaUtil::WriteLog(L"EASYZMQ_Request ::lstrlen(szRequestHeaderBody) : %d \n", ::lstrlen(wRequest));

    nResult = zmq_send(s_pSocketReq, wRequest, lengthReq, 0);
    ScaUtil::WriteLog(L"EASYZMQ_Request zmq_send nResult : %d \n", nResult);

    nResult = zmq_recv(s_pSocketReq, wResponse, nResponseLength, 0);

    ScaUtil::WriteLog(L"EASYZMQ_Request ::lstrlen(szResponse) : %d \n", ::lstrlen(wResponse));

    return nResult;
}



int _ResetReqSocket()
{
    int nResult = 0;

    if (s_pSocketReq != NULL)
    {
        nResult = zmq_close(s_pSocketReq);
        s_pSocketReq = NULL;
    }

    s_pZmqContextReq = zmq_ctx_new();
    s_pSocketReq = zmq_socket(s_pZmqContextReq, ZMQ_REQ);
    char szEasyZmqCon[1024] = { 0, };
    sprintf(szEasyZmqCon, "tcp://127.0.0.1:%d", s_nOtherPort);
    int nNewResult = zmq_connect(s_pSocketReq, szEasyZmqCon);
    return nNewResult;
}
