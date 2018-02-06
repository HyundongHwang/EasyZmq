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
// �����Ƽ Ŭ�󿡼� ���ڳ� Ŭ���� ������ �ݹ� ���Ź޴� �Լ�
////////////////////////////////////////////////////////////////////////////////
typedef int(*LP_EASYZMQ_ONPUSH)(
    LPCSTR szRequest,               // �ݹ� ��������
    LPSTR szResponse,               // �ݹ� ��������
    int nResponseLength);           // �ݹ� ���������� ����



                                    ////////////////////////////////////////////////////////////////////////////////
                                    // ���ڳ� Ŭ��� ����� �ϱ����� �ʱ�ȭ�Լ�
                                    // �����Ƽ Ŭ�󿡼� ����� �����ϱ��� �ݵ�� ȣ���ؾ� �Ѵ�.
                                    ////////////////////////////////////////////////////////////////////////////////
EASYZMQ_API int EASYZMQ_Init(
    int nMyPort,                        // �� ��� ��Ʈ
    int nOtherPort,                     // ����� ���� ��Ʈ
    LP_EASYZMQ_ONPUSH pFuncOnPush);     // �ݹ�����Լ� ������



EASYZMQ_API int EASYZMQ_Close(
    LPSTR szResponse,
    int nResponseLength);



////////////////////////////////////////////////////////////////////////////////
// �����Ƽ Ŭ�󿡼� ���ڳ� Ŭ��� ��û�ϴ� �Լ�
////////////////////////////////////////////////////////////////////////////////
EASYZMQ_API int EASYZMQ_Request(
    LPCSTR szRequest,               // ��û����
    LPSTR szResponse,               // ��������
    int nResponseLength,            // ���������� ����
    int nTimeout);                  // Ÿ�Ӿƿ� milli seconds



int _EASYZMQ_Request(
    LPCSTR szRequest,
    LPSTR szResponse,
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



int _RequestResponse(LPCSTR szRequest, LPSTR szResponse, int nResponseLength, int nTimeout);



int _ResetReqSocket();