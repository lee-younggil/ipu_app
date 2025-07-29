#ifndef VVPCOMM_H
#define VVPCOMM_H

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QThread>
#include <QByteArray>
#include <QTimer>
#include <QDebug>
#include "CompletionStruct.h"

class CVVPComm : public QObject
{
    Q_OBJECT
public:
    CVVPComm();
    ~CVVPComm();

    bool initialization();
    virtual bool ServiceStart();
    virtual bool ServiceStop();
    void SetDeviceIP(const QString& ip){m_deviceIP = ip;}
    void SetPort(int port){m_port = port;}
    int GetDeviceCode(const QByteArray& data);
    bool SendPacket(int iDeviceNo, unsigned short usMsgID, const unsigned char* pData, unsigned short iDataLength);
    void SendAck(unsigned short usSeqNo );
    unsigned short GetSeqNo( int iDeviceNo );
    bool IsConnected() const;

private slots:
    void onCheckConnection();
    void onConnected();
    void onReadyRead();      // Slot to handle readyRead signal
    void onDisconnected();   // Slot to handle disconnected signal

signals:
    void finished();
    void packetSent(const QByteArray& data);
    void packetRecv(const QByteArray& data);
    void requestSendAck(unsigned short usSeqNo); // Signal to request sending ACK

private:
enum tag_DEVICE_CODE
	{
		DEVICE_CODE_MCU = 0x01,
		DEVICE_CODE_VDU = 0x03,
		DEVICE_CODE_IPU = 0x04,
		DEVICE_CODE_VMS = 0x06,
		DEVICE_CODE_ENV = 0x07,
		DEVICE_CODE_TIU = 0x08,
		DEVICE_CODE_NVR = 0x09,
	};
    bool CreateSocket();
    bool ConnectToIPU();
    void HandleACKMessage(const QByteArray& data);
    void HandleOtherMessage(const QByteArray& data);
    void HandleConnectionFailure();
    
    void SendHeartbeat();
    void SendStatus();
    void decode(const QByteArray& data);
    void decodeStatus(PACKET* pPacket);

    QTcpSocket* m_pClientSocket;
    QQueue<QByteArray> m_sendQueue; // Keep send queue if used elsewhere
    QTimer* m_pCheckConnectionTimer; // Timer for checking connection

    bool m_bRunning;
    bool m_bConnected;  // 연결 상태 플래그 추가
    QString m_deviceIP;
    int m_port;
    unsigned short m_usSeqNo;
    qint64 m_lastHeartbeatTime;  // 마지막 하트비트 전송 시간
    qint64 m_lastLongHeartbeatTime;  // 마지막 1분 하트비트 전송 시간
};

#endif // VVPCOMM_H
