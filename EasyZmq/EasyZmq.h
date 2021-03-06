// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the EasyZmq_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EASYZMQ_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once



#ifdef EASYZMQ_EXPORTS
#define EASYZMQ_API extern "C" __declspec(dllexport)
#else
#define EASYZMQ_API extern "C" __declspec(dllimport)
#endif


////////////////////////////////////////////////////////////////////////////////
// EasyZmq 콜백수신함수 타입
////////////////////////////////////////////////////////////////////////////////
typedef int(*LP_EASYZMQ_ONPUSH)(
    LPCWSTR wRequest,               // 콜백 수신전문
    LPWSTR wResponse,               // 콜백 응답전문
    int nResponseLength);           // 콜백 응답전문의 길이



////////////////////////////////////////////////////////////////////////////////
// EasyZmq 초기화함수
//
// 리턴값 : 
//      성공 : 0, 양수
//      실패 : 음수 
//          에러코드 : https://github.com/zeromq/libzmq/blob/master/builds/msvc/errno.hpp
////////////////////////////////////////////////////////////////////////////////
EASYZMQ_API int EASYZMQ_Init(
    int nMyPort,                        // 내 대기 포트
    int nOtherPort,                     // 상대측 접속 포트
    LP_EASYZMQ_ONPUSH pFuncOnPush);     // 콜백수신함수 포인터



////////////////////////////////////////////////////////////////////////////////
// EasyZmq 요청함수
//
// 리턴값 : 
//      성공 : 0, 양수
//      실패 : 음수 
//          에러코드 : https://github.com/zeromq/libzmq/blob/master/builds/msvc/errno.hpp
////////////////////////////////////////////////////////////////////////////////
EASYZMQ_API int EASYZMQ_Request(
    LPCWSTR wRequest,               // 요청전문
    LPWSTR wResponse,               // 응답전문
    int nResponseLength,            // 응답전문의 길이
    int nTimeout);                  // 타임아웃 milli seconds



int _EASYZMQ_Request(
    LPCWSTR wRequest,
    LPWSTR wResponse,
    int nResponseLength,
    int nTimeout);


unsigned int WINAPI _ZmqRepThreadFunc(void* pParam);



void _CloseZmqContextSocketThread();



int _GetValidByteCountFromCharPtr(char* pSz);



bool _IsProgramInstalled(LPWSTR wDisplayName, LPCWSTR wComp, LPCWSTR wComp2);



void _DownloadAndExec(LPCWSTR wUrl, LPCWSTR wFileName, LPCWSTR wOption);



unsigned int WINAPI _DownloadAndExecThreadFunc(void* pParam);



struct _DownloadAndExecInfo
{
    LPCWSTR wUrl = NULL;
    LPCWSTR wFileName = NULL;
    LPCWSTR wOption = NULL;
};



int _RequestResponse(LPCWSTR wRequest, LPWSTR wResponse, int nResponseLength, int nTimeout);



int _InitReqSocket();
