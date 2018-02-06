#pragma once

class ScaUtil
{
public:
    static void HttpDownload(LPCWSTR strUrl, char* szBuf, int nBufSize);
    static void HttpDownload(LPCWSTR strUrl, LPCWSTR strPathLocal);
    static LPCWSTR GetLastErrDesc(DWORD dwLastErr);
    static bool IsProcessRunning(LPCWSTR wProcessName);
    static void WriteLog(LPCWSTR wFormat, ...);
};
