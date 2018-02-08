<!-- TOC -->

- [EasyZmq](#easyzmq)
- [데모](#데모)
- [구현](#구현)
- [배포](#배포)
- [참고사항](#참고사항)

<!-- /TOC -->

# EasyZmq
윈도우 어플리케이션에서 IPC방식으로 ZMQ가 많이 활용되고 있는데, 이를 기반으로 더 쉽게 사용하기 위해 만들어 보았다. <br>
2개의 프로세스가 상호통신하는 시나리오로 ZMQ의 REQ/REP 통신패턴을 양측에 구현한 것이다. <br>
ZMQ를 직접 사용하는 것에 비해 EasyZmq를 사용했을때 장점은 아래와 같다. <br>

- 단순한 인터페이스
    - 함수가 3개 뿐이다. `EASYZMQ_Init`, `EASYZMQ_Request`, `LP_EASYZMQ_ONPUSH`
- 단순한 스레드 관리
    - 응답을 수신대기 하는 스레드를 별도로 만들필요 없이 콜백함수만 등록하면 `EasyZmq` 내부에서 스레드 만들어 준다.
- 단순한 종속성 관리
    - libzmq.dll이 vcredist2010에 대한 종속성이 있는데, `EASYZMQ_Init` 첫 실행시 시스템을 조회하여 vcredist2010를 다운로드후 silent 자동설치해 준다.
- 유니코드 인코딩 메시지 사용
    - 각 시스템의 코드페이지가 달라 생기는 문제를 `EasyZmq` 내부에서 유니코드 인코딩 메시지를 사용해서 해결한다.
- 자동 헬스체크
    - 상대프로세스가 응답불능상태일때를 파악하기 요청후 3초동안 응답이 없으면 타임아웃이 발생된다.

# 데모
`/deploy/FirstMock.exe` 를 두번 실행하여 프로세스를 2개 띄운후, 송신포트 수신포트를 입력한다.

![](https://s9.postimg.org/5ln9r8b9r/screenshot_1.png)

# 구현

`MainWnd.h`

```cpp
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
typedef int(*LP_EASYZMQ_INIT)(
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
typedef int(*LP_EASYZMQ_REQUEST)(
    LPCWSTR wRequest,               // 요청전문
    LPWSTR wResponse,               // 응답전문
    int nResponseLength,            // 응답전문의 길이
    int nTimeout);                  // 타임아웃 milli seconds
```

`MainWnd.cpp`

```cpp
void MainWnd::_OnClickBtn_EASYZMQ_INIT(MainWnd* pThis)
{
    auto nResult = 0;
    wchar_t wTmp[1024] = { 0, };
    ::GetWindowText(pThis->m_edMyPort, wTmp, 1024);
    auto nMyPort = ::_wtoi(wTmp);
    ::GetWindowText(pThis->m_edOtherPort, wTmp, 1024);
    auto nOtherPort = ::_wtoi(wTmp);

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
```


# 배포
아래 파일들을 함께 배포해야 함.
- EasyZmq.dll
- libzmq.dll

# 참고사항
- ZMQ 란?
    - http://kr.zeromq.org/
- ZMQ REQ/REP 패턴
    - http://www.potatogim.net/wiki/ZeroMQ/%EC%86%8C%EC%BC%93%EA%B3%BC_%ED%8C%A8%ED%84%B4