#include "CdsLicComm.h"
#include <QDateTime>
#include <arpa/inet.h>

//CCdsLicComm::CCdsLicComm(): CSerialPort()
//  , m_lastError(Status::OK)
//  , m_errorMessage("")
//  , m_usSeqNo(0)
//{

//}

CCdsLicComm::CCdsLicComm(CMediator *hMediator, const QString &strLogSource)
    : CSerialPort(hMediator, strLogSource)
    , m_lastError(Status::OK)
    , m_errorMessage("")
    , m_usSeqNo(0)
{
    qDebug() << "CCdsLicComm::CCdsLicComm";
}

CCdsLicComm::~CCdsLicComm()
{
    ServiceStop();
}

void CCdsLicComm::Log(const QString &strLog, const void *pPacket, unsigned int uiPacketLength)
{
    // Add timestamp and source to log
    QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
    QString logMessage = QString("[%1] [CdsLicComm] %2").arg(timestamp).arg(strLog);
    
    // Call parent's log function
    CSerialPort::Log(logMessage, pPacket, uiPacketLength);
}

void CCdsLicComm::initialization(void)
{
    Log("Initializing CdsLicComm...");
    
    // Call parent initialization
    CSerialPort::initialization();
    
    // Reset error state
    m_lastError = Status::OK;
    m_errorMessage.clear();
    
    // Additional initialization specific to CDS license
    // ...
    
    Log("CdsLicComm initialized successfully");
}

bool CCdsLicComm::ServiceStart(void)
{
    Log("Starting CdsLicComm service...");
    
    // Reset error state
    m_lastError = Status::OK;
    m_errorMessage.clear();
    
    // Try to open serial port
    bool success = openSerialPort(false);
    if (!success) {
        setError(Status::ERROR_PORT_UNAVAILABLE, "Failed to open serial port");
        Log("Failed to start CdsLicComm service: Serial port unavailable");
        return false;
    }
    
    // Additional startup tasks specific to CDS license
    // ...
    
    Log("CdsLicComm service started successfully");
    emit eventOccurred("Service started successfully");
    return true;
}

bool CCdsLicComm::ServiceStop(void)
{
    Log("Stopping CdsLicComm service...");
    
    // Call parent's ServiceStop
    bool success = CSerialPort::ServiceStop();
    if (!success) {
        setError(Status::ERROR_COMMUNICATION, "Failed to stop service properly");
        Log("Failed to stop CdsLicComm service cleanly");
        return false;
    }
    
    Log("CdsLicComm service stopped successfully");
    emit eventOccurred("Service stopped");
    return true;
}

void CCdsLicComm::setError(Status status, const QString &message)
{
    // Only update if it's a new error message
    if (m_lastError != status || m_errorMessage != message) {
        m_lastError = status;
        m_errorMessage = message;
        
        // Log error
        Log(QString("Error: %1").arg(message));
        
        // Emit error signal
        emit errorOccurred(status, message);
    }
}

void CCdsLicComm::decode(const QByteArray& data)
{
    unsigned char packetBuffer[PACKET_LENGTH_MAX] = {0};

    PACKET *pPacket = (PACKET *)packetBuffer;

    int iStartSTX = 0;
    int currentIndex = 0;
    //int length = data.size();
    int packetLength = 0;
    //qDebug() << "decode in" << data.toHex(' ').toUpper();;
    if (m_recvQueue.isEmpty()){
        m_recvQueue.enqueue(data);
    }else{
        m_recvQueue.enqueue(m_recvQueue.dequeue() + data);
    }
    while(1){
        if (m_recvQueue.isEmpty())
        {
            break;
        }
        QByteArray dequeueData = m_recvQueue.dequeue();
        int length = dequeueData.size();
        qDebug() << "CCdsLicComm decode in" << dequeueData.toHex(' ').toUpper();;
        iStartSTX = dequeueData.indexOf(STX, currentIndex);
        if (iStartSTX == -1)
        {
            break;
        }

        int iPacketLength = length - iStartSTX;
        //qDebug() << "CCdsLicComm decode iStartSTX:" << iStartSTX << "currentIndex:" << currentIndex << "length:" << length << "iPacketLength:" << iPacketLength;
        if (iPacketLength > PACKET_LENGTH_MAX)
        {
            currentIndex = iStartSTX + 1;
            m_recvQueue.enqueue(dequeueData.mid(iStartSTX + 1));
            qDebug() << "continue incorrect packet length:" << iPacketLength;
            continue;
        }

        if (iPacketLength > ( PACKET_LENGTH_STX + PACKET_LENGTH_HEADER ))
        {
            memcpy(packetBuffer, dequeueData.data() + iStartSTX, PACKET_LENGTH_STX + PACKET_LENGTH_HEADER);
            packetLength = pPacket->usLength + PACKET_LENGTH_STX + PACKET_LENGTH_HEADER + PACKET_LENGTH_ETX;

            if (packetLength < PACKET_LENGTH_MIN || packetLength > PACKET_LENGTH_MAX)
            {
                currentIndex = iStartSTX + 1;
                qDebug() << "incorrect packet length:" << packetLength;
                continue;
            }
            else if( length < packetLength )
            {
                qDebug() << "length < packetLength incorrect packet length:" << packetLength;
                m_recvQueue.enqueue(dequeueData);
                break;
            }
            else
            {
                memcpy(packetBuffer, dequeueData.data() + iStartSTX, packetLength);
                
                if( packetBuffer[packetLength - 1] != ETX )
                {
                    qDebug() << "incorrect packet ETX:" << packetBuffer[packetLength - 1];
                    currentIndex = iStartSTX + 1;
                    continue;
                }
                if(length > (iStartSTX + packetLength))
                {
                    qDebug() << "length > (iStartSTX + packetLength) incorrect packet length:" << length;
                    //QByteArray enquueByteArray((char *)packetBuffer + iStartSTX + packetLength, length - (iStartSTX + packetLength));
                    m_recvQueue.enqueue(dequeueData.mid(iStartSTX + packetLength));
                }
                unsigned short usMsgID = pPacket->usMsgID;
                
                QString strLog;
                strLog.append(QString("R[%1] ").arg(usMsgID, 4, 16, QChar('0')));

                //QByteArray byteArray((char *)packetBuffer, packetLength);
                //qDebug() << "CCdsLicComm decode" << strLog + byteArray.toHex(' ').toUpper();
                
                
                switch (usMsgID)
                {
                    case MsgID_ACK:
                    case MsgID_NAK:
                        break;
                     default:
                        //SendAck(ntohs(pPacket->usSeqNo)); // Direct call removed
                        break;
                 }

                // 공통부분 처리
                switch (usMsgID)
                {
                    case KsgID_LicStatus:
                        decodeStatus(pPacket);
                        qDebug() << "decodeStatus";
                        break;
                    case MsgID_LicCamZoom:
                        //decodeLicCamZoom(pPacket);
                        break;
                    case MsgID_LicCamFocus:
                        //decodeLicCamFocus(pPacket);
                        break;
                    case MsgID_LicCamIris:
                        //decodeLicCamIris(pPacket);
                        break;
                    default:
                        break;
                }
            }
        }else
        {
            qDebug() << "incorrect packet length:" << packetLength;
            m_recvQueue.enqueue(dequeueData);
            break;
        }
    }
    
    //qDebug() << "CCdsLicComm decode out";
}

void CCdsLicComm::encodeControl(unsigned short usMsgID, char* pData, int flag, int value, char* reserved)
{
    PACKET_LIC_CONTROL *pControl = (PACKET_LIC_CONTROL *)(pData + PACKET_LENGTH_STX + PACKET_LENGTH_HEADER + PACKET_LENGTH_MsgID);
	memset( pControl, 0, sizeof(PACKET_LIC_CONTROL) );
    pControl->byteBossSign = flag;
    pControl->byteValue = value;
    if (reserved != nullptr) {
        memcpy(pControl->reserve, reserved, 4);
    }

    QByteArray byteArray(reinterpret_cast<const char*>(pControl), sizeof(PACKET_LIC_CONTROL));
    //emit packetSent(byteArray); // Emit the signal with the sent data
    qDebug() << "CCdsLicComm::encodeControl SendPacket - Hex Data:" << byteArray.toHex(' ').toUpper();
}

void CCdsLicComm::encodePacket(unsigned short usMsgID, char* pData)
{
    PACKET_LIC_CONTROL *pControl = (PACKET_LIC_CONTROL *)(pData + PACKET_LENGTH_STX + PACKET_LENGTH_HEADER + PACKET_LENGTH_MsgID);

    unsigned short iDataLength = sizeof(PACKET_LIC_CONTROL);

    int iPacketSize = PACKET_LENGTH_STX + PACKET_LENGTH_HEADER + PACKET_LENGTH_MsgID + iDataLength + PACKET_LENGTH_ETX;

    PACKET *pPacket = (PACKET *)pData;

    pPacket->byteSTX = STX;
	pPacket->usSeqNo = 0;//htons( GetSeqNo() );
	pPacket->byteRetryCnt = 0;
	pPacket->usLength = htons( PACKET_LENGTH_MsgID + iDataLength );
	//pPacket->byteDeviceCode = DEVICE_CODE;
	//pPacket->byteDeviceID = ( ( g_Mediator.m_hSwitchover.IsActive() == true ) ? 0x80 : 0x00 ) | g_Mediator.m_config.GetDeviceID();
	pPacket->usMsgID = htons( usMsgID );
	if( ( pData != nullptr ) && ( iDataLength != 0 ) )
	{
        qDebug() << "encodePacket" << iDataLength;
		memcpy( pPacket->byteData, pData + PACKET_LENGTH_STX + PACKET_LENGTH_HEADER + PACKET_LENGTH_MsgID, iDataLength );
	}	
	pPacket->byteData[iDataLength] = ETX;

    QByteArray byteArray(reinterpret_cast<const char*>(pPacket), 19);
    //emit packetSent(byteArray); // Emit the signal with the sent data
    qDebug() << "CCdsLicComm::encodePacket SendPacket - Hex Data:" << byteArray.toHex(' ').toUpper(); 
}

void CCdsLicComm::decodeStatus(PACKET* data)
{
    PACKET_LIC_STATUS *pState = (PACKET_LIC_STATUS *)data->byteData;
    //qDebug() << "decodeStatus temperature " << pState->usTempurature;
    //qDebug() << "decodeStatus cdsFirst " << pState->usCdsFirst;
    //qDebug() << "decodeStatus cdsSecond " << pState->usCdsSecond;
}

unsigned short CCdsLicComm::GetSeqNo()
{
	return ++m_usSeqNo;
}
