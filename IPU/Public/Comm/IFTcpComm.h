#ifndef IFTCPCOMM_H
#define IFTCPCOMM_H

#pragma once

#include <queue>
#include <stack>

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h> //TODO


#include "IFComm.h"
#include "../../IPUDefine.h"

#define PACKET_LENGTH_MAX			( 1024 )
#define PACKET_BUF_SIZE				( PACKET_LENGTH_MAX * 4 )
#define PACKET_READ_BUF_SIZE		( 64 )


#define PACKET_HOST_SYSTEM_HEARTBIT	(0x10)
#define PACKET_HOST_IMC_ID_SYSTEM	(30)
#define PACKET_HOST_IMC_ID_ACK		(31)

#define DST_ACK_PACKET_LEN			(8)

class CCircularQueue;

typedef struct
{
    int nSize;
    unsigned char baBuf[32];
}RX_BUF;

typedef struct
{
    int nSize;
    unsigned char baBuf[32];
}TX_BUF;

class CIFTcpComm : public CIFComm
{
public:
    CIFTcpComm(CMediator *hMediator, const QString &strLogSource);
    virtual ~CIFTcpComm(void);

public:
    virtual bool Connect(void);		// 접속
    virtual bool Close(void);		// 접속해제

    virtual int Recv(unsigned char *pucaBuf, const unsigned int uiSize);	// 수신
    virtual int Send(unsigned char *pucaBuf, const unsigned int uiSize);	// 전송

    virtual bool IsOpen(void);				// 접속되있느냐?
    virtual bool IsRecv(void);				// 받을수 있느냐?
    virtual bool IsSend(qint16 dwSendLen);	// 보낼수 있느냐?

    void SetTcpIP(int nTcpIP);
    void SetTcpPort(int nTcpPort);

    CCircularQueue * GetRxQueue();

private:
    const static int LISTEN_THREAD_SLEEP_TIME = 100;

// 자료구조
private:
    CCircularQueue *m_hRxQueue;

// 변수
private:
    CIPUDefine::ConnectState m_bConnect;

#ifdef TODO
    SOCKET m_pSockClient;
    SOCKET m_sockListen;
#endif

    pthread_t *m_pListenThread;
    pthread_t *m_pRecvThread;

    unsigned long int   m_TcpIP;
    unsigned short  m_TcpPort;

    std::stack<RX_BUF> m_sRxBuf;
    std::stack<TX_BUF> m_sTxBuf;

    //fd_set m_fdReadSet;
    //fd_set m_fdReadSetCopy;
    //TIMEVAL m_fdSetTimeout;

// 함수
private:
    static unsigned int ListenThread( void * pvParam);
    static unsigned int ReceiveThread(void * pvParam);
};

#endif // IFTCPCOMM_H
