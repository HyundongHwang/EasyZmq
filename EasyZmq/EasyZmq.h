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
// EasyZmq �ݹ�����Լ� Ÿ��
////////////////////////////////////////////////////////////////////////////////
typedef int(*LP_EASYZMQ_ONPUSH)(
    LPCWSTR wRequest,               // �ݹ� ��������
    LPWSTR wResponse,               // �ݹ� ��������
    int nResponseLength);           // �ݹ� ���������� ����



////////////////////////////////////////////////////////////////////////////////
// EasyZmq �ʱ�ȭ�Լ�
//
// ���ϰ� : 
//      ���� : 0, ���
//      ���� : ���� 
//          �����ڵ� : https://github.com/zeromq/libzmq/blob/master/builds/msvc/errno.hpp
////////////////////////////////////////////////////////////////////////////////
EASYZMQ_API int EASYZMQ_Init(
    int nMyPort,                        // �� ��� ��Ʈ
    int nOtherPort,                     // ����� ���� ��Ʈ
    LP_EASYZMQ_ONPUSH pFuncOnPush);     // �ݹ�����Լ� ������



////////////////////////////////////////////////////////////////////////////////
// EasyZmq ��û�Լ�
//
// ���ϰ� : 
//      ���� : 0, ���
//      ���� : ���� 
//          �����ڵ� : https://github.com/zeromq/libzmq/blob/master/builds/msvc/errno.hpp
////////////////////////////////////////////////////////////////////////////////
EASYZMQ_API int EASYZMQ_Request(
    LPCWSTR wRequest,               // ��û����
    LPWSTR wResponse,               // ��������
    int nResponseLength,            // ���������� ����
    int nTimeout);                  // Ÿ�Ӿƿ� milli seconds



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
