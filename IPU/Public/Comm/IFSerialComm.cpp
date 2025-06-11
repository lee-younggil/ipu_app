#include "IFSerialComm.h"
#include <QDebug>

#define SHARE_PORT 5

int g_iaBaudRate[NUM_BAUDRATE] =
{
    110,	300,	600,	1200,	2400,
    4800,	9600,	14400,	19200,	38400,
    56000,	57600,	115200,	128000,	256000
};

//CIFSerialComm::CIFSerialComm() : CService()
//    , m_iPort(1)
//    , m_iBaud(115200)
//    , m_dwInQueueSize(1024)
//    , m_dwOutQueueSize(1024)
//    , m_serialPort(nullptr)
//    , m_bOpened(false)
//    , m_lastError(SerialError::NoError)
//{
//    // Create serial port object
//    m_serialPort = new QSerialPort(this);

//    // Connect signals
//    if (m_serialPort) {
//        connect(m_serialPort, &QSerialPort::errorOccurred, this, &CIFSerialComm::handleSerialError);
//        //connect(m_serialPort, &QSerialPort::readyRead, this, &CIFSerialComm::handleReadyRead);
//    }

//    // Set default protocol handler
//    setProtocolHandler(new SerialProtocolHandler());
//}

CIFSerialComm::CIFSerialComm(CMediator *hMediator, const QString &strLogSource)
    : CIFComm(hMediator, strLogSource, CommType::Serial)
    , m_iPort(1)
    , m_iBaud(115200)
    , m_dwInQueueSize(1024)
    , m_dwOutQueueSize(1024)
    , m_serialPort(nullptr)
    , m_bOpened(false)
    , m_lastError(SerialError::NoError)
{
    // Create serial port object
    m_serialPort = new QSerialPort(this);
    
    // Connect signals
    if (m_serialPort) {
        connect(m_serialPort, &QSerialPort::errorOccurred, this, &CIFSerialComm::handleSerialError);
        //connect(m_serialPort, &QSerialPort::readyRead, this, &CIFSerialComm::handleReadyRead);
    }
    
    // Set default protocol handler
    setProtocolHandler(new SerialProtocolHandler());
}

CIFSerialComm::~CIFSerialComm()
{
    Close();
    
    // Clean up protocol handler
    if (m_protocolHandler) {
        delete m_protocolHandler;
        m_protocolHandler = nullptr;
    }
    
    // Clean up serial port
    if (m_serialPort) {
        delete m_serialPort;
        m_serialPort = nullptr;
    }
}

void CIFSerialComm::SetShareDevice(void* hShareDevice)
{
    // QSerialPort에서는 사용하지 않음
}

bool CIFSerialComm::Connect()
{
    if (m_bOpened) {
        return true; // 이미 열려있음
    }
    
    if (!m_serialPort) {
        setError(SerialError::NotOpenError, "Serial port object not initialized");
        return false;
    }
    
    // 포트 이름 설정 (/dev/ttyUSB 등)
    QString portName = QString("/dev/ttyUSB%1").arg(m_iPort);
    m_serialPort->setPortName(portName);
    
    // 보드레이트 및 기타 설정
    m_serialPort->setBaudRate(m_iBaud);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    
    // 시리얼 포트 열기
    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        setError(SerialError::OpenError, m_serialPort->errorString());
        return false;
    }
    
    m_bOpened = true;
    emit serialPortConnected();
    emit connectionStatusChanged(CommStatus::Connected, "Serial port connected");
    
    return true;
}

bool CIFSerialComm::Close()
{
    if (!m_bOpened) {
        return true; // 이미 닫혀있음
    }
    
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    
    m_bOpened = false;
    emit serialPortDisconnected();
    emit connectionStatusChanged(CommStatus::Disconnected, "Serial port disconnected");
    
    return true;
}

int CIFSerialComm::Send(unsigned char *pucaBuf, const unsigned int uiSize)
{
    if (!m_bOpened || !m_serialPort || !m_serialPort->isOpen() || !pucaBuf || uiSize == 0) {
        setError(SerialError::WriteError, "Cannot send: port not open or invalid data");
        return -1;
    }
    
    // 데이터 쓰기
    qint64 bytesWritten = m_serialPort->write(reinterpret_cast<const char*>(pucaBuf), uiSize);
    
    if (bytesWritten < 0) {
        setError(SerialError::WriteError, m_serialPort->errorString());
        return -1;
    }
    
    // 데이터 전송 완료 대기
    if (!m_serialPort->waitForBytesWritten(1000)) {
        setError(SerialError::TimeoutError, "Write timeout");
        return -1;
    }
    
    // 전송 완료 신호 발생
    emit dataSent(bytesWritten);
    
    return bytesWritten;
}

int CIFSerialComm::Recv(unsigned char *pucaBuf, const unsigned int uiSize)
{
    if (!m_bOpened || !m_serialPort || !m_serialPort->isOpen() || !pucaBuf || uiSize == 0) {
        setError(SerialError::ReadError, "Cannot receive: port not open or invalid buffer");
        return -1;
    }
    
    // 읽을 데이터가 있는지 확인
    if (m_serialPort->bytesAvailable() == 0) {
        if (!m_serialPort->waitForReadyRead(100)) {
            // 타임아웃이지만 에러는 아님
            return 0;
        }
    }
    
    // 데이터 읽기
    qint64 bytesRead = m_serialPort->read(reinterpret_cast<char*>(pucaBuf), uiSize);
    
    if (bytesRead < 0) {
        setError(SerialError::ReadError, m_serialPort->errorString());
        return -1;
    }
    
    // 데이터 수신 처리
    if (bytesRead > 0) {
        processReceivedData(pucaBuf, bytesRead);
    }
    
    return bytesRead;
}

bool CIFSerialComm::IsOpen()
{
    return m_bOpened && m_serialPort && m_serialPort->isOpen();
}

bool CIFSerialComm::IsRecv()
{
    return m_bOpened && m_serialPort && m_serialPort->isOpen() && m_serialPort->bytesAvailable() > 0;
}

bool CIFSerialComm::IsSend(qint16 dwSendLen)
{
    if (!m_bOpened || !m_serialPort) return false;
    return m_serialPort->bytesToWrite() >= dwSendLen;
}

void CIFSerialComm::Log(const QString &strLog, const void *pPacket, unsigned int uiPacketLength)
{
    // Basic logging functionality for IFSerialComm
    if (m_hMediator) {
        // If mediator exists, you could use it for logging
        // Otherwise simply output to debug console
        qDebug() << "[IFSerialComm] " << strLog;
    }
    
    // If packet data is provided, log it as hex values
    if (pPacket && uiPacketLength > 0) {
        QString hexDump;
        const unsigned char* data = static_cast<const unsigned char*>(pPacket);
        
        for (unsigned int i = 0; i < uiPacketLength; ++i) {
            hexDump += QString("%1 ").arg(data[i], 2, 16, QChar('0')).toUpper();
            if ((i + 1) % 16 == 0) {
                hexDump += "\n";
            }
        }
        
        if (!hexDump.isEmpty()) {
            qDebug() << "Packet Data:" << hexDump;
        }
    }
}

bool CIFSerialComm::WriteCommByte(unsigned char ucByte)
{
    if (!m_bOpened || !m_serialPort) return false;

    qint64 bytesWritten = m_serialPort->write(reinterpret_cast<char*>(&ucByte), 1);
    return (bytesWritten == 1);
}

int CIFSerialComm::ReadDataWaiting()
{
    if (!m_bOpened || !m_serialPort) return 0;
    return static_cast<int>(m_serialPort->bytesAvailable());
}

int CIFSerialComm::EnumerateSerialPorts(int portNum[])
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    int count = 0;
    
    for (const QSerialPortInfo &info : ports) {
        QString portName = info.portName();
        if (portName.startsWith("ttyUSB")) {
            portName.remove(0, 6); // Remove "ttyUSB" prefix
            bool ok;
            int port = portName.toInt(&ok);
            if (ok) {
                portNum[count++] = port;
            }
        }
    }
    
    return count;
}

void CIFSerialComm::SetPort(int iPort)
{
    m_iPort = iPort;
}

void CIFSerialComm::SetBaudRate(int iBaud)
{
    m_iBaud = iBaud;
    if (m_serialPort && m_bOpened) {
        m_serialPort->setBaudRate(iBaud);
    }
}

void CIFSerialComm::SetInQueueSize(qint16 dwInQueueSize)
{
    m_dwInQueueSize = dwInQueueSize;
}

void CIFSerialComm::SetOutQueueSize(qint16 dwOutQueueSize)
{
    m_dwOutQueueSize = dwOutQueueSize;
}

void CIFSerialComm::SetDataBits(SerialParams dataBits)
{
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setDataBits(static_cast<QSerialPort::DataBits>(dataBits));
    }
}

void CIFSerialComm::SetStopBits(SerialParams stopBits)
{
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setStopBits(static_cast<QSerialPort::StopBits>(stopBits));
    }
}

void CIFSerialComm::SetParity(SerialParams parity)
{
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setParity(static_cast<QSerialPort::Parity>(parity));
    }
}

void CIFSerialComm::SetFlowControl(QSerialPort::FlowControl flowControl)
{
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->setFlowControl(flowControl);
    }
}

bool CIFSerialComm::openSerialPort(const QString& portName, int baudRate,
                                  QSerialPort::DataBits dataBits,
                                  QSerialPort::Parity parity,
                                  QSerialPort::StopBits stopBits,
                                  QSerialPort::FlowControl flowControl)
{
    // 이미 연결된 경우 닫기
    if (m_bOpened) {
        Close();
    }
    
    if (!m_serialPort) {
        setError(SerialError::NotOpenError, "Serial port object not initialized");
        return false;
    }
    
    // 시리얼 포트 설정
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(dataBits);
    m_serialPort->setParity(parity);
    m_serialPort->setStopBits(stopBits);
    m_serialPort->setFlowControl(flowControl);
    
    // 시리얼 포트 열기
    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        setError(SerialError::OpenError, m_serialPort->errorString());
        return false;
    }
    
    // 설정 업데이트
    m_iBaud = baudRate;
    
    // Extract port number from name if possible
    if (portName.startsWith("/dev/ttyUSB")) {
        QString numStr = portName.mid(11); // Get part after "/dev/ttyUSB"
        bool ok;
        int port = numStr.toInt(&ok);
        if (ok) {
            m_iPort = port;
        }
    }
    
    m_bOpened = true;
    emit serialPortConnected();
    emit connectionStatusChanged(CommStatus::Connected, "Serial port connected");
    
    return true;
}

QStringList CIFSerialComm::getAvailablePorts() const
{
    QStringList result;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    
    for (const QSerialPortInfo &info : ports) {
        result.append(info.portName());
    }
    
    return result;
}

QString CIFSerialComm::getErrorMessage(SerialError error)
{
    switch (error) {
        case SerialError::NoError:
            return "No error";
        case SerialError::OpenError:
            return "Failed to open serial port";
        case SerialError::ConfigError:
            return "Configuration error";
        case SerialError::WriteError:
            return "Write error";
        case SerialError::ReadError:
            return "Read error";
        case SerialError::NotOpenError:
            return "Port not open";
        case SerialError::TimeoutError:
            return "Timeout error";
        case SerialError::UnknownError:
        default:
            return "Unknown error";
    }
}

void CIFSerialComm::setError(SerialError error, const QString& message)
{
    m_lastError = error;
    
    QString errorMsg = message.isEmpty() ? getErrorMessage(error) : message;
    
    emit serialPortError(errorMsg);
    emit errorOccurred(errorMsg, static_cast<int>(error));
    emit connectionStatusChanged(CommStatus::Error, errorMsg);
    
    qDebug() << "Serial Error:" << errorMsg;
}

void CIFSerialComm::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }
    
    // Convert QSerialPort error to our error type
    SerialError ourError;
    switch (error) {
        case QSerialPort::OpenError:
            ourError = SerialError::OpenError;
            break;
        case QSerialPort::WriteError:
            ourError = SerialError::WriteError;
            break;
        case QSerialPort::ReadError:
            ourError = SerialError::ReadError;
            break;
        case QSerialPort::TimeoutError:
            ourError = SerialError::TimeoutError;
            break;
        case QSerialPort::NotOpenError:
            ourError = SerialError::NotOpenError;
            break;
        default:
            ourError = SerialError::UnknownError;
            break;
    }
    
    setError(ourError, m_serialPort->errorString());
    
    // If the error is critical, close the port
    if (error != QSerialPort::TimeoutError) {
        Close();
    }
}

void CIFSerialComm::handleReadyRead()
{
    if (!m_bOpened || !m_serialPort) {
        return;
    }
    
    // Get available data size
    qint64 bytesAvailable = m_serialPort->bytesAvailable();
    if (bytesAvailable <= 0) {
        return;
    }
    
    // Read data
    QByteArray data = m_serialPort->readAll();
    if (data.isEmpty()) {
        return;
    }
    
    // Process received data
    processReceivedData(reinterpret_cast<const unsigned char*>(data.constData()), data.size());
}
