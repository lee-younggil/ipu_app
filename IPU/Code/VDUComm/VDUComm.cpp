#include "VDUComm.h"
#include <QTcpSocket>
#include <QQueue>
#include <QThread>
#include <arpa/inet.h>
#include "Code/Mediator.h"
#include "Util/Util.h"

CVDUComm::CVDUComm() : m_pClientSocket(nullptr), m_pCheckConnectionTimer(nullptr), m_bConnected(false)
{
    m_pCheckConnectionTimer = new QTimer(this);
    connect(m_pCheckConnectionTimer, &QTimer::timeout, this, &CVDUComm::onTimeout);
    m_bRunning = (false);
}

CVDUComm::~CVDUComm()
{
    ServiceStop();
}

bool CVDUComm::initialization()
{
    m_usSeqNo = 0;
    m_triggerNum = 0;
    
    return true;
}

bool CVDUComm::IsConnected() const
{
    return m_pClientSocket && m_pClientSocket->state() == QAbstractSocket::ConnectedState;
}

bool CVDUComm::ServiceStart()
{
    m_bRunning = true;
    
    CreateSocket();
    // StartAsyncReceive(); // Removed call
    // Connect socket signals directly here
    connect(m_pClientSocket, &QTcpSocket::connected, this, &CVDUComm::OnConnected);
    connect(m_pClientSocket, &QTcpSocket::readyRead, this, &CVDUComm::OnReadyRead);
    connect(m_pClientSocket, &QTcpSocket::disconnected, this, &CVDUComm::OnDisconnected);
    
    ConnectToIPU();
    qDebug() << "CVDUComm::ServiceStart - Socket connected signal connected.";

    QThread::msleep(1000);
    m_lastLongHeartbeatTime = QDateTime::currentMSecsSinceEpoch();
    m_lastHeartbeatTime = m_lastLongHeartbeatTime;

    m_pCheckConnectionTimer->start(5000);
    
    return true;
}

void CVDUComm::onTimeout()
{
    // Handle timeout event
    //qDebug() << "Timeout occurred!";
    CheckConnection();
    // Add your timeout handling code here
}

bool CVDUComm::ServiceStop()
{
    m_bRunning = false;

    if (m_pClientSocket)
    {
        m_pClientSocket->close();
     }
 
     // Removed m_pRecvThread cleanup
 
     return true;
}

void CVDUComm::CheckConnection()
{
    const int RETRY_INTERVAL_MS = 5000;  // 재시도 간격 (1초)
    const int HEARTBEAT_INTERVAL_MS = 3000;  // 3초마다 하트비트 전송
    const int LONG_HEARTBEAT_INTERVAL_MS = 60000;  // 1분마다 status 전송

    if (!m_bRunning)
    {
        return;
    }

    switch (m_pClientSocket->state()) {
        case QAbstractSocket::UnconnectedState:
            m_bRunning = false;
            if (!ConnectToIPU())
            {
                HandleConnectionFailure();
            }
            m_bRunning = true;
            break;
        case QAbstractSocket::ConnectedState:
            {
                // 현재 시간 가져오기
                qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

                // 1분마다 추가 하트비트 전송
                if (currentTime - m_lastLongHeartbeatTime >= LONG_HEARTBEAT_INTERVAL_MS)
                {
                    SendStatus();
                    m_lastLongHeartbeatTime = currentTime;  // 마지막 1분 하트비트 시간 갱신
                    m_lastHeartbeatTime = currentTime;
                }

                // 3초마다 하트비트 전송
                if (currentTime - m_lastHeartbeatTime >= HEARTBEAT_INTERVAL_MS)
                {
                    SendHeartbeat();
                    m_lastHeartbeatTime = currentTime;  // 마지막 하트비트 시간 갱신
                }
            }

            
            break;
        case QAbstractSocket::ClosingState:
        case QAbstractSocket::ListeningState:
        case QAbstractSocket::BoundState:
        case QAbstractSocket::HostLookupState:
        case QAbstractSocket::ConnectingState:
            m_bRunning = false;
            qDebug() << "소켓 상태 이상:" << m_pClientSocket->state();
            HandleConnectionFailure();
            //QThread::msleep(RETRY_INTERVAL_MS);
            m_bRunning = true;
            break;
        default:
            m_bRunning = false;
            qDebug() << "알 수 없는 소켓 상태";
            HandleConnectionFailure();
            //QThread::msleep(RETRY_INTERVAL_MS);
            m_bRunning = true;
            break;
    }
}

bool CVDUComm::CreateSocket()
{
    m_pClientSocket = new QTcpSocket();
    return m_pClientSocket != nullptr;
}

bool CVDUComm::ConnectToIPU()
{
    if (!m_pClientSocket)
    {
        return false;
    }

    qDebug() << "Connection try IP:" << m_deviceIP << " Port:" << m_port;

    m_pClientSocket->connectToHost(m_deviceIP, m_port);

    // Wait for connection to be established (timeout: 2 seconds)
    if (m_pClientSocket->waitForConnected(1000))
    {
        if (IsConnected())
        {
            return true;
        }
        else
        {
            qDebug() << "IsConnected failed:";
            return false;
        }
    }
    else
    {
        qDebug() << "Connection failed:" << m_pClientSocket->errorString();
        return false;
    }
}
 
void CVDUComm::decode(const QByteArray& data)
{
    unsigned char packetBuffer[PACKET_LENGTH_MAX] = {0};

    PACKET *pPacket = (PACKET *)packetBuffer;

    int iStartSTX = 0;
    int currentIndex = 0;
    int length = data.size();
    int packetLength = 0;
    while(1){
        iStartSTX = data.indexOf(STX, currentIndex);
        if (iStartSTX == -1)
        {
            break;
        }

        int iPacketLength = length - iStartSTX;
        if (iPacketLength > PACKET_LENGTH_MAX)
        {
            currentIndex = iStartSTX + 1;
            continue;
        }

        if (iPacketLength > ( PACKET_LENGTH_STX + PACKET_LENGTH_HEADER ))
        {
            memcpy(packetBuffer, data.data() + iStartSTX, PACKET_LENGTH_STX + PACKET_LENGTH_HEADER);
            packetLength = ntohs(pPacket->usLength) + PACKET_LENGTH_STX + PACKET_LENGTH_HEADER + PACKET_LENGTH_ETX;

            if (packetLength < PACKET_LENGTH_MIN || packetLength > PACKET_LENGTH_MAX)
            {
                currentIndex = iStartSTX + 1;
                qDebug() << "incorrect packet length:" << packetLength;
                continue;
            }
            else if( length < packetLength )
            {
                break;
            }
            else
            {
                memcpy(packetBuffer, data.data() + iStartSTX, packetLength);
                
                if( packetBuffer[packetLength - 1] != ETX )
                {
                    qDebug() << "incorrect packet ETX:" << packetBuffer[packetLength - 1];
                    currentIndex = iStartSTX + 1;
                    continue;
                }
                currentIndex = iStartSTX + packetLength;
                unsigned short usMsgID = ntohs(pPacket->usMsgID);
                
                QString strLog;
                strLog.append(QString("R[%1] ").arg(usMsgID, 4, 16, QChar('0')));

                QByteArray byteArray((char *)packetBuffer, packetLength);
                //qDebug() << "CVDUComm decode" << strLog + byteArray.toHex(' ').toUpper();
                
                
                switch (usMsgID)
                {
                    case MsgID_ACK:
                    case MsgID_NAK:
                        break;
                     default:
                        SendAck(ntohs(pPacket->usSeqNo)); // Direct call removed
                        break;
                 }

                // 공통부분 처리
                switch (usMsgID)
                {
                    case MsgID_Status:
                        decodeStatus(pPacket);
                        break;
                    default:
                        break;
                }

                switch (pPacket->byteDeviceCode)
                {
                    case DEVICE_CODE_VDU:
                        switch (usMsgID)
                        {
                            case MsgID_ACK:
                            case MsgID_NAK:
                            case MsgID_Status:
                                break;
                            case MsgID_HeartBeat:
                            case MsgID_Command:
                                break;
                            case MsgID_Trigger:
                                decodeTriggerNum(pPacket->byteData);
                                break;
                            default:
                                qDebug() << "Unknown VDU message ID:" << usMsgID;
                        }
                        break;
                    default:
                        qDebug() << "Unknown device code:" << pPacket->byteDeviceCode;
                }
            }
        }
    }
}

void CVDUComm::decodeTriggerNum(unsigned char* pucaBuf)
{
    qDebug() << "CVDUComm::decodeTriggerNum" << pucaBuf[0] << pucaBuf[1] << pucaBuf[2] << pucaBuf[3] << pucaBuf[4] << pucaBuf[5] << pucaBuf[6];
    PACKET_ENTRY_TRIGGER_NUM *pTrigger = (PACKET_ENTRY_TRIGGER_NUM *)pucaBuf;
    VEHICLE_INFO *pVehicleInfo = new VEHICLE_INFO;
    memset(pVehicleInfo, 0, sizeof(VEHICLE_INFO));
    pVehicleInfo->bitInfoState |= CU_VEHICLE_STATE_TRIGGER_NO;
    pVehicleInfo->ulLastCommunicationTime = CUtil::GetMiliSecTimer();

    pTrigger->uiTriggerNo = htonl(1);
    memcpy(&pVehicleInfo->pktEntryTriggerNum, pTrigger, sizeof(PACKET_ENTRY_TRIGGER_NUM));
    CMediator::getInstance()->PushBackVehicle(pVehicleInfo);
    CMediator::getInstance()->NotifyTrigger(pVehicleInfo->pktEntryTriggerNum.uiTriggerNo);
    qDebug() << "CVDUComm::decodeTriggerNum" << pVehicleInfo->pktEntryTriggerNum.uiTriggerNo;
}

void CVDUComm::decodeStatus(PACKET* pPacket)
{
    PACKET_STATE *pState = (PACKET_STATE *)(pPacket->byteData);
    /*
	if( g_Mediator.m_hSwitchover.IsActive() == true )
	{
		CTimeSpan ts;
		CTime t = CTime::GetCurrentTime();
		CTime t2 = CUtil::GetCTime( pState->dateData );

		if( t > t2 )
		{
			ts = t - t2;
		}
		else
		{
			ts = t2 - t;
		}
        
		if( ts.GetTotalSeconds() > 2 )
		{
			if( g_Mediator.m_hMatch.IsDetectionEvent() == false )
			{
				SendTimeSync();
			}			
		}
        
	}
    */
}

void CVDUComm::HandleACKMessage(const QByteArray& data)
{
    if (GetDeviceCode(data) == DEVICE_CODE_MCU)
    {
        // Handle IPU ACK
    }
    else
    {
        // Handle other device ACK
    }
    return;
}

void CVDUComm::HandleOtherMessage(const QByteArray& data)
{
    Q_UNUSED(data)

    if (m_pClientSocket)
    {
        if (m_pClientSocket->isOpen())
        {
            m_pClientSocket->close();
        }
        delete m_pClientSocket;
        m_pClientSocket = nullptr;
    }
}

void CVDUComm::SendAck(unsigned short usSeqNo)
{
    //qDebug() << "SendAck called with SeqNo:" << usSeqNo;

    PACKET_ACK tAck;
    memset(&tAck, 0, sizeof(PACKET_ACK));
    tAck.usSeqNo = usSeqNo;

    //qDebug() << "Send ACK:" << usSeqNo;
    SendPacket(DEVICE_CODE_IPU, MsgID_ACK, (unsigned char *)&tAck, sizeof(PACKET_ACK));
}

void CVDUComm::TestTriggerNum(unsigned short usSeqNo)
{
    qDebug() << "TestTriggerNum called with SeqNo:" << usSeqNo;
    PACKET_ENTRY_TRIGGER_NUM tTrigger;
    memset(&tTrigger, 0, sizeof(PACKET_ENTRY_TRIGGER_NUM));
    tTrigger.byteVehiclePosition = 0x11;
	tTrigger.dateData = CUtil::GetYYYYMMDDhhmmssmsms0(QDateTime::currentDateTime());
    tTrigger.byteVDSType = 0x01; // VDU Type
    tTrigger.uiTriggerNo = m_triggerNum; // Trigger Number
    memset(tTrigger.DUMMY,0,7); // Dummy data

    decodeTriggerNum(reinterpret_cast<unsigned char*>(&tTrigger));
    CMediator::getInstance()->SendImage(tTrigger.uiTriggerNo);
}

bool CVDUComm::SendPacket(int iDeviceNo, unsigned short usMsgID, const unsigned char* pData, unsigned short iDataLength) 
{
    if (!m_pClientSocket || !m_pClientSocket->isOpen())
    {
        qDebug() << "Socket is not open!";
        return false;
    }

    char message[PACKET_LENGTH_MAX];

    int iPacketSize = PACKET_LENGTH_STX + PACKET_LENGTH_HEADER + PACKET_LENGTH_MsgID + iDataLength + PACKET_LENGTH_ETX;
	PACKET *pPacket = (PACKET *)message;
	memset( pPacket, 0, iPacketSize );

    message[0] = STX;
	pPacket->usSeqNo = htons(GetSeqNo( iDeviceNo ));
	pPacket->byteRetryCnt = 0;
	pPacket->usLength = htons(PACKET_LENGTH_MsgID + iDataLength);
	pPacket->byteDeviceCode = DEVICE_CODE;
	pPacket->byteDeviceID = 0x81;
	pPacket->usMsgID = htons(usMsgID);

	if( ( pData != nullptr ) && ( iDataLength != 0 ) )
	{
		memcpy( pPacket->byteData, pData, iDataLength );
	}	
	pPacket->byteData[iDataLength] = ETX;
                
    QString strLog;
    strLog.append(QString("T[%1] ").arg(htons(pPacket->usMsgID), 4, 16, QChar('0')));

    QByteArray byteArray(message, iPacketSize);
    emit packetSent(byteArray); // Emit the signal with the sent data
    //qDebug() << "CVDUComm::SendPacket" << strLog + byteArray.toHex(' ').toUpper();

    qint64 bytesWritten = m_pClientSocket->write(message, iPacketSize);
    if (bytesWritten == -1)
    {
        qDebug() << "Write error:" << m_pClientSocket->errorString();
        return false;
    }

    return m_pClientSocket->flush();
}

void CVDUComm::HandleConnectionFailure()
{
    // Handle connection failure and retry logic
    if (m_pClientSocket)
    {
        if (m_pClientSocket->isOpen())
        {
            m_pClientSocket->close();
        }
    }
    
    m_bConnected = false;  // 연결 해제 상태로 설정
}

int CVDUComm::GetDeviceCode(const QByteArray& data)
{
    return data[0];
}

unsigned short CVDUComm::GetSeqNo( int iDeviceNo )
{
    Q_UNUSED(iDeviceNo)

    return ++m_usSeqNo;
}

void CVDUComm::SendHeartbeat()
{
    if (m_bRunning && m_pClientSocket && m_pClientSocket->state() == QAbstractSocket::ConnectedState)
    {
        SendPacket(0, MsgID_HeartBeat, nullptr, 0);
    }
}

void CVDUComm::SendStatus()
{
    if (m_bRunning && m_pClientSocket && m_pClientSocket->state() == QAbstractSocket::ConnectedState)
    {
        const int STATUS_LEN = 0x3E;
        unsigned char* pData = new unsigned char[STATUS_LEN];
        memset(pData, 0, STATUS_LEN);

        QDateTime currentDateTime = QDateTime::currentDateTime(); // UTC 시간
        int year = currentDateTime.date().year();
        int month = currentDateTime.date().month();
        int day = currentDateTime.date().day();
        int hour = currentDateTime.time().hour();
        int minute = currentDateTime.time().minute();
        int second = currentDateTime.time().second();
        int msec = currentDateTime.time().msec();

        // 시간을 pData에 추가 (Big-Endian 형식)
        pData[0] = (year >> 8) & 0xFF;  // Year 상위 바이트
        pData[1] = year & 0xFF;         // Year 하위 바이트
        pData[2] = month;               // Month
        pData[3] = day;                 // Day
        pData[4] = hour;                // Hour
        pData[5] = minute;              // MinuteSend ACK
        pData[6] = second;              // Second
        pData[7] = (msec >> 8) & 0xFF;  // Milliseconds 상위 바이트
        pData[8] = msec & 0xFF;         // Milliseconds 하위 바이트
        pData[9] = 0x01;
        SendPacket(0, MsgID_Status, pData, STATUS_LEN);
         delete[] pData;
     }
 }

 void CVDUComm::OnConnected()
 {
    qDebug() << "Socket connected.";
    // 연결 성공
    qDebug() << "VDU 연결 성공!";
    m_bConnected = true;
    // Optionally connect signals to the new socket instance
    // connect(m_pClientSocket, &QTcpSocket::readyRead, this, &CVDUComm::OnReadyRead);
    // connect(m_pClientSocket, &QTcpSocket::disconnected, this, &CVDUComm::OnDisconnected);
 }
 
 void CVDUComm::OnReadyRead()
 {
     if (!m_pClientSocket) return;
 
     // Read all available data
     QByteArray data = m_pClientSocket->readAll();
     if (!data.isEmpty()) {
        //qDebug() << "Data received:" << data.toHex(' ').toUpper();
        //qDebug() << "Data received, size:" << data.size();
        // Process the received data directly
        decode(data);
        // Optionally emit packetRecv if needed elsewhere
        emit packetRecv(data);
     } else {
         qDebug() << "ReadyRead signal received, but no data read.";
     }
 }
 
 void CVDUComm::OnDisconnected()
 {
     qDebug() << "Socket disconnected.";
     m_bConnected = false;
     // Optionally disconnect signals from the old socket instance
     // if(m_pClientSocket) {
     //     disconnect(m_pClientSocket, &QTcpSocket::readyRead, this, &CVDUComm::OnReadyRead);
     //     disconnect(m_pClientSocket, &QTcpSocket::disconnected, this, &CVDUComm::OnDisconnected);
     // }
     // HandleConnectionFailure will clean up the socket pointer later in the connect loop
 }
