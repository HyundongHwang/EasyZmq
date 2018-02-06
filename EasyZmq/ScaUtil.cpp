#include "stdafx.h"
#include "ScaUtil.h"


void ScaUtil::HttpDownload(LPCWSTR strUrl, LPSTR szBuf, int nBufSize)
{
    HINTERNET hInternet = ::InternetOpen(
        L"HttpTest",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL,
        0);

    if (hInternet == NULL)
    {
        OutputDebugString(L"HttpDownload 인터넷에 연결할 수 없습니다. \n"); //hhdtrace
        return;
    }

    HINTERNET hUrl = ::InternetOpenUrl(
        hInternet,
        strUrl,
        NULL,
        0,
        INTERNET_FLAG_RELOAD,
        0);

    if (hUrl == NULL)
    {
        OutputDebugString(L"HttpDownload 지정된 URL에 연결할 수 없습니다. \n"); //hhdtrace
        ::InternetCloseHandle(hInternet);
        return;
    }

    DWORD dwProgress = 0;

    while (TRUE)
    {
        DWORD dwSize = 0;
        ::InternetQueryDataAvailable(hUrl, &dwSize, 0, 0);

        DWORD dwRead = 0;
        ::InternetReadFile(hUrl, szBuf + dwProgress, dwSize, &dwRead);
        dwProgress += dwRead;

        if (dwRead == 0)
        {
            OutputDebugString(L"HttpDownload 다운로드 완료 했습니다. \n");
            break;
        }

        MSG msg = { 0, };
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

void ScaUtil::HttpDownload(LPCWSTR wUrl, LPCWSTR wPathLocal)
{
    HINTERNET hInternet = ::InternetOpen(
        L"HttpTest",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL,
        0);

    if (hInternet == NULL)
    {
        OutputDebugString(L"HttpDownload 인터넷에 연결할 수 없습니다. \n"); //hhdtrace
        return;
    }

    HINTERNET hUrl = ::InternetOpenUrl(
        hInternet,
        wUrl,
        NULL,
        0,
        INTERNET_FLAG_RELOAD,
        0);

    if (hUrl == NULL)
    {
        OutputDebugString(L"HttpDownload 지정된 URL에 연결할 수 없습니다. \n"); //hhdtrace
        ::InternetCloseHandle(hInternet);
        return;
    }

    HANDLE hFile = ::CreateFile(
        wPathLocal,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    DWORD dwProgress = 0;

    {
        wchar_t wStr[1024] = { 0, };
        wsprintf(wStr, L"HttpDownload [%s] 파일을 다운로드 시작합니다. \n", wPathLocal);
        ::OutputDebugString(wStr);//hhddebug
    }

    while (TRUE)
    {
        DWORD dwSize = 0;
        ::InternetQueryDataAvailable(hUrl, &dwSize, 0, 0);

        BYTE* buf = new BYTE[dwSize];
        ::ZeroMemory(buf, dwSize);

        DWORD dwRead = 0;
        ::InternetReadFile(hUrl, buf, dwSize, &dwRead);

        DWORD dwWritten = 0;
        ::WriteFile(hFile, buf, dwRead, &dwWritten, NULL);

        dwProgress += dwWritten;

        if (dwRead == 0)
        {
            {
                wchar_t wStr[1024] = { 0, };
                wsprintf(wStr, L"HttpDownload [%s] 파일을 다운로드 완료 했습니다. \n", wPathLocal);
                ::OutputDebugString(wStr);//hhddebug
            }

            break;
        }

        MSG msg = { 0, };
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }

        delete[]buf;
    }

    ::CloseHandle(hFile);
}

LPCWSTR ScaUtil::GetLastErrDesc(DWORD dwLastErr)
{
    {
        wchar_t wStr[1024] = { 0, };
        wsprintf(wStr, L"GetLastErrDesc dwLastErr : %d \n", dwLastErr);
        ::OutputDebugString(wStr);//hhddebug
    }

    wchar_t wDesc[1024] = { 0, };
    LPVOID pVoid = NULL;

    ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwLastErr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&pVoid,
        0,
        NULL);

    wsprintf(wDesc, L"dwLastErr : %d(0x%x) \nstrDesc : %s", dwLastErr, dwLastErr, (LPWSTR)pVoid);

    if (pVoid != NULL)
    {
        ::LocalFree(pVoid);
        pVoid = NULL;
    }

    {
        wchar_t wStr[1024] = { 0, };
        wsprintf(wStr, L"GetLastErrDesc wDesc : %s \n", wDesc);
        ::OutputDebugString(wStr);//hhddebug
    }

    return wDesc;
}

bool ScaUtil::IsProcessRunning(LPCWSTR wProcessName)
{
    bool bExist = false;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (::Process32First(snapshot, &entry))
    {
        while (::Process32Next(snapshot, &entry))
        {
            if (StrStrIW(entry.szExeFile, wProcessName) != NULL)
            {
                {
                    wchar_t wStr[1024] = { 0, };
                    wsprintf(wStr, L"ScaUtil::IsProcessRunning FOUND!!! entry.szExeFile : %s \n", entry.szExeFile);
                    ::OutputDebugString(wStr);//hhddebug
                }

                {
                    wchar_t wStr[1024] = { 0, };
                    wsprintf(wStr, L"ScaUtil::IsProcessRunning FOUND!!! wProcessName : %s \n", wProcessName);
                    ::OutputDebugString(wStr);//hhddebug
                }

                bExist = true;
                break;
            }
        }
    }

    CloseHandle(snapshot);
    return bExist;
}

void ScaUtil::WriteLog(LPCWSTR wFormat, ...)
{
    va_list argList;
    va_start(argList, wFormat);
    wchar_t wStr[1024] = { 0, };
    vswprintf(wStr, 1024, wFormat, argList);
    ::OutputDebugString(wStr);
    va_end(argList);
}
