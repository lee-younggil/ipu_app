#include "IFTcpComm.h"

#include "Code/Mediator.h"
#include "Public/CircularQueue/CircularQueue.h"

//#include <ws2tcpip.h>


using namespace std;
CIFTcpComm::CIFTcpComm(CMediator *hMediator, const QString &strLogSource)
    : CIFComm(hMediator, strLogSource)

{
    //?Œì¼“ ì´ˆê¸°??    //WSADATA wsaData;
    //WSAStartup( WINSOCK_VERSION, &wsaData );

    m_TcpIP = 0;
    m_TcpPort = 0;
    //m_pSockClient( INVALID_SOCKET )
    m_pListenThread = nullptr;
    m_pRecvThread = nullptr;
    //m_sockListen( INVALID_SOCKET )
    m_bConnect = CIPUDefine::DISCONNECT;

    //m_hRxQueue = new CCircularQueue();
    //m_hRxQueue->initQueue( PACKET_BUF_SIZE * 5 );
}


CIFTcpComm::~CIFTcpComm(void)
{
    //WSACleanup();

    //m_hRxQueue->deleteQueue();
    //delete m_hRxQueue;
}

bool CIFTcpComm::IsOpen(void)
{
    return (( m_bConnect == CIPUDefine::CONNECTED )? true:false );
}

bool CIFTcpComm::Connect(void)
{
    bool bRet = false;
    switch( m_bConnect )
    {
#ifdef TODO
    case CIPUDefine::DISCONNECT:
        m_pListenThread = AfxBeginThread( ListenThread, this, THREAD_PRIORITY_NORMAL );
        if( m_pListenThread == NULL )
        {
            Log( ("ListenThread Start Fail") );
            bRet =false;
        }
        else
        {
            m_bConnect = CIPUDefine::CONNECTED;
            m_pListenThread->m_bAutoDelete = FALSE;

            m_pRecvThread = AfxBeginThread( ReceiveThread, this, THREAD_PRIORITY_NORMAL );
            bRet = true;
        }
        break;
#endif

    case CIPUDefine::CONNECTING:
        bRet =false;
        break;
    case CIPUDefine::CONNECTED:
        bRet = true;
        break;

    case CIPUDefine::DISCONNECT:
        bRet = false;
        break;
    }
    return bRet;
}

bool CIFTcpComm::Close(void)
{
    switch( m_bConnect )
    {
    case CIPUDefine::DISCONNECT:	// ?‘ì†ì¤‘ì´ ?„ë‹ˆ?¼ë©´ TRUE ë¦¬í„´
        break;
    case CIPUDefine::CONNECTING:	// ?‘ì†?œë„ì¤??íƒœë¥?ë²—ì–´???Œì¼“ ì¢…ë£Œ
        m_bConnect = CIPUDefine::DISCONNECT;
        break;
    case CIPUDefine::CONNECTED:		// ?‘ì†ì¤‘ì´?¼ë©´ ?Œì¼“ ?«ê³  TRUE ë¦¬í„´
#ifdef TODO
        shutdown( m_sockListen, SD_BOTH );
        closesocket( m_sockListen );
        m_sockListen = INVALID_SOCKET;


        if( WaitForSingleObject( m_pListenThread->m_hThread, LISTEN_THREAD_SLEEP_TIME * 10 ) != WAIT_OBJECT_0 )
        {
            Log("ListenThread Stop Fail");
            TerminateThread (m_pListenThread->m_hThread, 0);
        }
#endif
        m_pListenThread = NULL;
        m_bConnect = CIPUDefine::DISCONNECT;
        break;

    }
    return true;
}

#define DEFAULT_PORT "20385"

unsigned int CIFTcpComm::ReceiveThread(void* pvParam )
{
    CIFTcpComm *pThis = (CIFTcpComm *)pvParam;

    QString strLog;
    //pThis->Log("RX-ACU Thread Start");

    enum eBufSize { BUF_SIZE = 4096 };

    struct addrinfo *result = NULL;
    int iResult;

#ifdef TODO
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    while( pThis->m_bConnect == CIPUDefine::CONNECTED)
    {
        unsigned char usRecvBuf[PACKET_LENGTH_MAX] = {0x00, };
        unsigned char *pBuf = NULL;
        RX_BUF buffer;

        int bytesReceived = recv(pThis->m_pSockClient, (char*)usRecvBuf, PACKET_LENGTH_MAX, 0);

        if(bytesReceived > 0)
        {

            QString strLog;
            strLog.asprintf("R[%02d] ", bytesReceived);
            for(int i = 0; i < bytesReceived; i++)
            {
                strLog + strLog.asprintf("%02x ", usRecvBuf[i]);
            }
            pThis->Log(strLog);

            pThis->m_hRxQueue->push(usRecvBuf, bytesReceived);
        }
        else
        {
            continue;
        }

    }
#endif

    return 0;
}

unsigned int CIFTcpComm::ListenThread( void* pvParam )
{
    CIFTcpComm *pThis = (CIFTcpComm *)pvParam;

#ifdef TODO
    QString strLog;
    pThis->Log("ListenThread Start");

    pThis->m_sockListen = socket( AF_INET, SOCK_STREAM, 0 );
    if( pThis->m_sockListen == INVALID_SOCKET )
    {
        strLog.asprintf( ("Fail Crate Listen socket : %d"), WSAGetLastError() );
        pThis->Log( strLog );

        pThis->Log("ListenThread Stop");
        return 0;
    }

    BOOL opt = TRUE;

    //setsockopt( pThis->m_sockListen, (IPPROTO_TCP | SO_REUSEADDR), TCP_NODELAY, (const char *)&opt, sizeof(opt) );
    setsockopt( pThis->m_sockListen, IPPROTO_TCP, TCP_NODELAY, (const char *)&opt, sizeof(opt) );

    SOCKADDR_IN serveraddr;
    ZeroMemory( &serveraddr, sizeof( serveraddr ));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = pThis->m_TcpIP;
    serveraddr.sin_port = pThis->m_TcpPort;

    int retval = bind( pThis->m_sockListen, (SOCKADDR *)&serveraddr, sizeof(serveraddr) );
    if( retval == SOCKET_ERROR )
    {
        strLog.asprintf( ("Fail bind : %d"), WSAGetLastError() );
        pThis->Log( strLog );

        pThis->Log("ListenThread Stop");
        pThis->m_sockListen = INVALID_SOCKET;
        return 0;
    }

    retval = listen( pThis->m_sockListen, SOMAXCONN );
    if( retval == SOCKET_ERROR )
    {
        strLog.asprintf( ("Fail listen : %d"), WSAGetLastError() );
        pThis->Log( strLog );

        pThis->Log("ListenThread Stop");
        pThis->m_sockListen = INVALID_SOCKET;
        return 0;
    }


    unsigned int uiWatchdogID = pThis->m_hMediator->RegisterWatchdogID( ("ListenThread"), LISTEN_THREAD_SLEEP_TIME );

    FD_ZERO(&pThis->m_fdReadSet);
    FD_SET(pThis->m_sockListen, &pThis->m_fdReadSet);

    SOCKET client_sock;

    while( pThis->m_bConnect == CIPUDefine::CONNECTED )
    {
        pThis->m_hMediator->StopWatchdogID( uiWatchdogID );

        Sleep( LISTEN_THREAD_SLEEP_TIME );

        SOCKADDR_IN clientaddr;
        int addrlen = sizeof( clientaddr );

        pThis->m_fdReadSetCopy = pThis->m_fdReadSet;
        pThis->m_fdSetTimeout.tv_sec = 1;
        pThis->m_fdSetTimeout.tv_usec = 5000;

        int fd_num = -1;
        /*if( (fd_num = select(0, &pThis->m_fdReadSetCopy, 0, 0, &pThis->m_fdSetTimeout)) == SOCKET_ERROR){
            strLog.asprintf( ("Fail select : %d"), WSAGetLastError() );
            pThis->Log( strLog );
            continue;
        }*/

        fd_num = select(0, &pThis->m_fdReadSetCopy, NULL, NULL, &pThis->m_fdSetTimeout);


        //if(fd_num == 0)
        //	continue;

        //for(int i=0; i < pThis->m_fdReadSet.fd_count; i++)
        for(int i=0; i <= pThis->m_fdReadSetCopy.fd_count; i++)
        {
            SOCKET currentSocket = pThis->m_fdReadSetCopy.fd_array[i];

            if( FD_ISSET(currentSocket, &pThis->m_fdReadSetCopy) )
            {
                //?ˆë¡œ ?°ê²°???¤ì–´??ê²½ìš°
                if(currentSocket == pThis->m_sockListen)
                {
                    //strLog.asprintf( ("Close m_pSockClient [%d] "), pThis->m_pSockClient );
                    //pThis->Log( strLog );

                    //closesocket(pThis->m_pSockClient);
                    //FD_CLR(currentSocket, &pThis->m_fdReadSet);

                    client_sock = accept( pThis->m_sockListen, (SOCKADDR *)&clientaddr, &addrlen );

                    if( client_sock == INVALID_SOCKET )
                    {
                        int iRet = WSAGetLastError();
                        if( iRet != WSAEINTR )	// 10004
                        {
                            strLog.asprintf( ("Fail accept[%d] : %d"), i, WSAGetLastError() );
                            pThis->Log( strLog );

                            continue;
                        }
                    }
                    FD_SET(client_sock, &pThis->m_fdReadSet);

                    pThis->m_pSockClient = client_sock;
                    pThis->m_hRxQueue->clear();

                    strLog.asprintf( ("new accept [%d] "), client_sock );
                    pThis->Log( strLog );
                }
                else
                {
                    pThis->m_hMediator->RefreshWatchdogID( uiWatchdogID );

                    enum eBufSize { BUF_SIZE = 1024 };
                    UCHAR buf[1024] = {0x00, };
                    int bytesReceived = recv(client_sock, (char*)buf, BUF_SIZE, 0);

                    if(bytesReceived <= 0)
                    {
                        strLog.asprintf("close [%d], [%d] ", client_sock, pThis->m_pSockClient);
                        pThis->Log(strLog);

                        closesocket(pThis->m_pSockClient);
                        FD_CLR(currentSocket, &pThis->m_fdReadSet);
                        pThis->m_hRxQueue->clear();
                    }
                    else
                    {
                        strLog.asprintf("accept [%d] != [%d] ", client_sock, pThis->m_pSockClient);
                        pThis->Log(strLog);

                        FD_SET(client_sock, &pThis->m_fdReadSet);

                        pThis->m_pSockClient = client_sock;

                        pThis->m_hRxQueue->clear();

                        //enum eBufSize { BUF_SIZE = 1024 };
                        //UCHAR buf[1024] = {0x00, };
                        //int bytesReceived = recv(client_sock, (char*)buf, BUF_SIZE, 0);
                        /*QString strLog;
                        strLog.asprintf("R[%02d] ", bytesReceived);
                        for(int i = 0; i < bytesReceived; i++)
                        {
                            strLog.Appendsprintf("%02x ", usRecvBuf[i]);
                        }
                        pThis->Log(strLog);*/

                        pThis->m_hRxQueue->push(buf, bytesReceived);
                        strLog.asprintf( ("exist accept [%d], [%d] "), currentSocket, client_sock);
                        pThis->Log( strLog );
                    }

                }
                //if (--fd_num <= 0) break;
            }
        }
    }

    pThis->m_hMediator->StopWatchdogID( uiWatchdogID );
    pThis->Log("ListenThread Stop");
#endif

    return 0;
}

bool CIFTcpComm::IsRecv()
{
    if( m_bConnect == CIPUDefine::CONNECTED )
    {
#ifdef TODO
        if(m_hRxQueue->IsEmpty() != TRUE)
        {
            return true;
        }
        else
        {
            return false;
        }
#endif
    }
    else
        return false;
}

int CIFTcpComm::Recv(unsigned char *pucaBuf, const unsigned int uiSize)
{
    if ( m_bConnect != CIPUDefine::CONNECTED ) return 0;

#ifdef TODO
    if( m_hRxQueue->IsEmpty() == true) return 0;

    int nQueueSize = m_hRxQueue->GetQueueSize();

    if(nQueueSize < DST_ACK_PACKET_LEN)
    {
        m_hRxQueue->clear();
        return 0;
    }

    return nQueueSize;
#endif

    return 0;
}

CCircularQueue * CIFTcpComm::GetRxQueue()
{
    return m_hRxQueue;
}

int CIFTcpComm::Send(unsigned char *pucaBuf, const unsigned int uiSize)
{
    QString strLog;

    unsigned char buf[1024] = {'\0',};
    unsigned char *pBuf = NULL;
    //TX_BUF buffer;

    int bytesSend = 0; //send(m_pSockClient, (char*)buf, uiSize, 0);

    if (pucaBuf != NULL)
    {
#ifdef TODO
        bytesSend = send(m_pSockClient, (char*)pucaBuf, uiSize, 0);

        strLog.asprintf(("Success send : %d"), bytesSend);
        //Log(strLog);
#endif
    }
    else
    {
        strLog.asprintf(("Fail send : %d"), bytesSend);
    }
    return true;
}

bool CIFTcpComm::IsSend(qint16 dwSendLen)
{
    if( m_bConnect == CIPUDefine::CONNECTED )
    {
#if 1 //
        //int res;
        //fd_set fdWrite = { 0 };
        //TIMEVAL stTime;
        //stTime.tv_sec = 0;
        //stTime.tv_usec = 0;
#ifdef TODO
        FD_ZERO(&fdWrite);
        FD_SET(m_pSockClient, &fdWrite);

        res = select( m_pSockClient+1, NULL, &fdWrite, NULL, &stTime);

        if( res <= 0 )
        {
            return false;
        }
        else if( res < 0 )	// SOCKET_ERROR
        {

            QString strLog;
            strLog.asprintf("ERROR : select()2 : %d",WSAGetLastError());
            Log( strLog );

            return false;
        }
#endif

#else
        int res;
        //fd_set fdWrite = { 0 };
        //TIMEVAL stTime;
        m_fdSetTimeout.tv_sec = 0;
        m_fdSetTimeout.tv_usec = 0;

        //FD_ZERO(&fdWrite);
        //FD_SET(m_pSockClient, &fdWrite);

        //res = select( m_pSockClient+1, NULL, &fdWrite, NULL, &tm_fdSetTimeoutTime);
        res = select( 0, &m_fdReadSetCopy, 0, 0, &m_fdSetTimeout);

        if( res <= 0 )
        {
            return false;
        }
        else if( res < 0 )	// SOCKET_ERROR
        {
            QString strLog;
            strLog.asprintf("ERROR : select()2 : %d",WSAGetLastError());
            Log( strLog );
            return false;
        }
#endif
        return true;
    }
    return false;
}

void CIFTcpComm::SetTcpIP(int nTcpIP)
{
    m_TcpIP = nTcpIP;
}

void CIFTcpComm::SetTcpPort(int nTcpPort)
{
    m_TcpPort = nTcpPort;
}
