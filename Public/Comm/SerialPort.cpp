#include "SerialPort.h"
#include "Code/CdsLicComm/CdsLicDlg.h"
#include "Code/Mediator.h"
#include "Code/CdsLicComm/CdsLicComm.h"
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QRegularExpression>
#include <QElapsedTimer>
#include <QCoreApplication>

// SerialTransactionHandler implementation
SerialTransactionHandler::SerialTransactionHandler(QObject* parent)
    : QObject(parent), m_nextId(1)
{
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setInterval(100); // Check every 100ms
    connect(m_timeoutTimer, &QTimer::timeout, this, &SerialTransactionHandler::checkTimeouts);
    m_timeoutTimer->start();
}

int SerialTransactionHandler::addTransaction(const QByteArray& request, int timeout)
{
    if (request.isEmpty() || timeout <= 0) {
        return -1;
    }
    
    int id = m_nextId++;
    Transaction transaction;
    transaction.id = id;
    transaction.request = request;
    transaction.timestamp = QDateTime::currentMSecsSinceEpoch();
    transaction.timeout = timeout;
    transaction.completed = false;
    
    m_transactions.insert(id, transaction);
    return id;
}

bool SerialTransactionHandler::processResponse(const QByteArray& data)
{
    if (data.isEmpty() || m_transactions.isEmpty()) {
        return false;
    }
    
    // Process for all pending transactions
    bool handled = false;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    
    for (auto it = m_transactions.begin(); it != m_transactions.end(); ++it) {
        if (it.value().completed) {
            continue;
        }
        
        // Simple implementation: any response completes the transaction
        // In real-world scenarios, you'd match specific request-response pairs
        it.value().response = data;
        it.value().completed = true;
        handled = true;
        
        emit transactionCompleted(it.value().id, it.value().response);
    }
    
    return handled;
}

bool SerialTransactionHandler::isTransactionPending(int id) const
{
    auto it = m_transactions.find(id);
    return (it != m_transactions.end() && !it.value().completed);
}

QByteArray SerialTransactionHandler::getResponse(int id) const
{
    auto it = m_transactions.find(id);
    if (it != m_transactions.end() && it.value().completed) {
        return it.value().response;
    }
    return QByteArray();
}

void SerialTransactionHandler::clearTransactions()
{
    m_transactions.clear();
}

void SerialTransactionHandler::checkTimeouts()
{
    if (m_transactions.isEmpty()) {
        return;
    }
    
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    
    for (auto it = m_transactions.begin(); it != m_transactions.end();) {
        if (it.value().completed) {
            it = m_transactions.erase(it);
            continue;
        }
        
        qint64 elapsed = now - it.value().timestamp;
        if (elapsed > it.value().timeout) {
            int id = it.value().id;
            emit transactionTimeout(id);
            it = m_transactions.erase(it);
        } else {
            ++it;
        }
    }
}

CSerialPort::CSerialPort(CMediator *hMediator, const QString &strLogSource)
    : CIFSerialComm(hMediator, strLogSource)
    , m_dialog(nullptr)
    , m_readTimer(nullptr)
    , m_reconnectTimer(new QTimer(this))
    , m_errorMessage("")
    , m_reconnectAttempts(0)
    , m_dataFormat(DataFormat::Hex)
    , m_reconnectStrategy(ReconnectStrategy::SimpleRetry)
    , m_currentPortScanIndex(0)
    , m_readInterval(100)
    , m_maxReadSize(1024)
    , m_reconnectInterval(5000)
    , m_maxReconnectAttempts(5)
{
    // 재연결 타이머 설정
    m_reconnectTimer->setInterval(m_reconnectInterval);
    connect(m_reconnectTimer, &QTimer::timeout, this, &CSerialPort::tryReconnect);
    
    // Create transaction handler
    m_transactionHandler = std::make_unique<SerialTransactionHandler>();
    connect(m_transactionHandler.get(), &SerialTransactionHandler::transactionCompleted,
            this, &CSerialPort::onTransactionCompleted);
    connect(m_transactionHandler.get(), &SerialTransactionHandler::transactionTimeout,
            this, &CSerialPort::onTransactionTimeout);
}

CSerialPort::~CSerialPort()
{
    ServiceStop();
}

void CSerialPort::Log(const QString &strLog, const void *pPacket, unsigned int uiPacketLength)
{
    // Add timestamp for better logging
    QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
    QString formattedLog = QString("[%1] [Serial] %2").arg(timestamp).arg(strLog);
    
    // If packet data is provided, format it according to current format
    if (pPacket && uiPacketLength > 0) {
        QString formattedData = formatData(static_cast<const unsigned char*>(pPacket), uiPacketLength);
        formattedLog += QString(" Data: %1").arg(formattedData);
    }
    
    // Call parent's log function
    CIFSerialComm::Log(formattedLog, nullptr, 0);
}

void CSerialPort::initialization(void)
{
    qDebug() << "CSerialPort::initialization() start";
    
    // 시리얼 통신 큐 크기 설정
    SetInQueueSize(1024);
    SetOutQueueSize(1024);

    // 시리얼 포트 설정
    //SetPort(0);  // ttyUSB0 포트 사용
    //SetBaudRate(115200);  // 115200 baud rate 설정

    // Set defaults for data format
    m_dataFormat = DataFormat::Hex;

    // 시리얼 포트 시그널 연결
    connect(this, &CIFSerialComm::serialPortConnected, 
            this, &CSerialPort::onSerialPortConnected);
    connect(this, &CIFSerialComm::serialPortDisconnected, 
            this, &CSerialPort::onSerialPortDisconnected);
    connect(this, &CIFSerialComm::serialPortError, 
            this, &CSerialPort::onSerialPortError);
    
    // serialConnectionStatusChanged 시그널 연결
    connect(this, &CSerialPort::serialConnectionStatusChanged,
            this, &CSerialPort::onSerialConnectionStatusChanged);
            
    qDebug() << "CSerialPort::initialization() - Signal connections established";
}

bool CSerialPort::openSerialPort(bool isReconnect, int portToTry)
{
    qDebug() << "CSerialPort::openSerialPort() start";
    bool bState = false;
    
    // 이미 연결된 경우 실패 반환
    if (IsOpen()) {
        return false;
    }
    
    // If a specific port is requested, try only that port
    if (portToTry >= 0) {
        SetPort(portToTry);
        qDebug() << "Attempting to connect to port" << portToTry;
        bState = Connect();
    } else {
        // Try available ports in sequence
        // ttyUSB0 시도
        SetPort(0);
        qDebug() << "Attempting to connect to /dev/ttyUSB0";
        if (Connect()) {
            qDebug() << "Connection to ttyUSB0 successful";
            bState = true;
        } else {
            // ttyUSB1 시도
            SetPort(1);
            qDebug() << "Connection to ttyUSB0 failed, trying ttyUSB1";
            if (Connect()) {
                qDebug() << "Connection to ttyUSB1 successful";
                bState = true;
            } else {
                qDebug() << "Connection to ttyUSB1 failed";
                emit serialConnectionStatusChanged(false, "Failed to connect to any port");
                return false;
            }
        }
    }

    // 연결 성공 시 설정 정보 출력
    if (bState) {
        QSerialPort* port = getSerialPort();
        if (port) {
            Log(QString("Connected to %1 at %2 baud")
                .arg(port->portName())
                .arg(port->baudRate()));
            
            qDebug() << "Port:" << port->portName();
            qDebug() << "Baud Rate:" << port->baudRate();
            qDebug() << "Data Bits:" << port->dataBits();
            qDebug() << "Stop Bits:" << port->stopBits();
            qDebug() << "Parity:" << port->parity();
            qDebug() << "Flow Control:" << port->flowControl();
            
            // UI 업데이트
            if (m_dialog && !isReconnect) {
                m_dialog->updateSerialConnectionStatus(
                    true, 
                    QString("Connected to %1").arg(port->portName())
                );
                qDebug() << "Updated connection status";
            }
        }
    }
    
    return bState;
}

bool CSerialPort::ServiceStop(void)
{
    // 재연결 타이머 중지
    if (m_reconnectTimer && m_reconnectTimer->isActive()) {
        m_reconnectTimer->stop();
    }
    m_reconnectAttempts = 0;
    
    // 연결 종료
    if (IsOpen()) {
        Close();
        qDebug() << "Serial port connection closed";
    }
    
    return true;
}

void CSerialPort::configureReconnection(int interval, int maxAttempts)
{
    if (interval > 0) {
        m_reconnectInterval = interval;
        if (m_reconnectTimer) {
            m_reconnectTimer->setInterval(m_reconnectInterval);
        }
    }
    
    if (maxAttempts > 0) {
        m_maxReconnectAttempts = maxAttempts;
    }
    
    qDebug() << "Reconnection configured: interval=" << m_reconnectInterval 
             << "ms, maxAttempts=" << m_maxReconnectAttempts;
}

QString CSerialPort::formatData(const unsigned char* data, unsigned int size) const
{
    if (!data || size == 0) {
        return QString();
    }
    
    QString result;
    
    switch (m_dataFormat) {
        case DataFormat::Hex:
            for (unsigned int i = 0; i < size; ++i) {
                result += QString("%1 ").arg(data[i], 2, 16, QChar('0')).toUpper();
            }
            break;
            
        case DataFormat::Ascii:
            for (unsigned int i = 0; i < size; ++i) {
                if (data[i] >= 32 && data[i] <= 126) {
                    // Printable ASCII
                    result += QChar(data[i]);
                } else {
                    // Non-printable
                    result += ".";
                }
            }
            break;
            
        case DataFormat::Decimal:
            for (unsigned int i = 0; i < size; ++i) {
                result += QString("%1 ").arg(data[i]);
            }
            break;
            
        case DataFormat::Mixed:
            for (unsigned int i = 0; i < size; ++i) {
                result += QString("%1(").arg(data[i], 2, 16, QChar('0')).toUpper();
                
                if (data[i] >= 32 && data[i] <= 126) {
                    result += QChar(data[i]);
                } else {
                    result += ".";
                }
                
                result += ") ";
            }
            break;
    }
    
    return result.trimmed();
}

void CSerialPort::setDialog(CdsLicDlg* dialog)
{ 
    m_dialog = dialog; 
    qDebug() << "Dialog set for serial port communication";
}

void CSerialPort::onSerialPortConnected()
{
    qDebug() << "Serial port connected";
    
    // 재연결 타이머 중지 및 시도 횟수 초기화
    m_reconnectTimer->stop();
    m_reconnectAttempts = 0;
    
    emit serialConnectionStatusChanged(true, "Connected");
    
    // 시리얼 포트의 readyRead 시그널 연결
    QSerialPort* serialPort = getSerialPort();
    if (serialPort) {
        connect(serialPort, &QSerialPort::readyRead,
                this, &CSerialPort::onSerialDataReady);
        qDebug() << "Connected readyRead signal";
    }

    if (m_dialog) {
        m_dialog->updateSerialConnectionStatus(true, "Connected");
    }
}

void CSerialPort::onSerialPortDisconnected()
{
    qDebug() << "Serial port disconnected";
    
    // readyRead 시그널 연결 해제
    QSerialPort* serialPort = getSerialPort();
    if (serialPort) {
        disconnect(serialPort, &QSerialPort::readyRead,
                  this, &CSerialPort::onSerialDataReady);
    }
    
    emit serialConnectionStatusChanged(false, "Disconnected");
    
    // 재연결 타이머 시작
    m_reconnectAttempts = 0;
    m_reconnectTimer->start();
    qDebug() << "Starting reconnection timer...";
}

void CSerialPort::onSerialPortError(const QString &error)
{
    // 중복 오류 방지
    if(error != m_errorMessage) {
        qDebug() << "Serial port error:" << error;
        emit serialConnectionStatusChanged(false, "Error: " + error);
        m_errorMessage = error;
    }
    
    // 연결 종료
    Close();
    
    // 재연결 타이머 시작
    if (!m_reconnectTimer->isActive()) {
        m_reconnectAttempts = 0;
        m_reconnectTimer->start();
        qDebug() << "Starting reconnection timer due to error...";
    }
}

void CSerialPort::onSerialConnectionStatusChanged(bool connected, const QString &message)
{
    qDebug() << "Serial connection status changed: " << message;
    if (m_dialog) {
        m_dialog->updateSerialConnectionStatus(connected, message);
    }
}

void CSerialPort::onSerialDataReady()
{
    QSerialPort* serialPort = getSerialPort();
    if (!serialPort) {
        return;
    }
    
    // 시리얼 포트에서 데이터 읽기
    QByteArray data = serialPort->readAll();
    if (data.isEmpty()) {
        return;
    }

    CCdsLicComm* cdsLicComm = CMediator::getInstance()->getCdsLicComm();
    if (cdsLicComm) {
        cdsLicComm->decode(data); // CCdsLicComm의 decode 호출
    }

    //qDebug() << "Received data:" << data.toHex() << "(" << data << ")";
    
    // Process the received data
    processReceivedData(data);
}

void CSerialPort::processReceivedData(const QByteArray &data)
{
    // 데이터 형식화
    QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
    QString formattedData = formatData(reinterpret_cast<const unsigned char*>(data.constData()), data.size());
    
    Log(QString("Received: %1").arg(formattedData));
    
    // Process through transaction handler first
    bool handled = false;
    if (m_transactionHandler) {
        handled = m_transactionHandler->processResponse(data);
    }
    
    // 데이터 수신 시그널 발생
    emit serialDataReceived(timestamp, formattedData);
    
    // UI 업데이트
    if (m_dialog) {
        m_dialog->appendSerialData(timestamp, formattedData);
    }
    
    // Emit the raw data received signal from base class
    emit dataReceived(reinterpret_cast<const unsigned char*>(data.constData()), data.size());
}

void CSerialPort::tryReconnect()
{
    // 최대 재연결 시도 횟수 체크
    /*if (m_reconnectStrategy == ReconnectStrategy::None || 
        (m_reconnectAttempts >= m_maxReconnectAttempts && m_reconnectStrategy != ReconnectStrategy::MaxPortScan)) {
        qDebug() << "Maximum reconnection attempts reached. Stopping reconnection timer.";
        m_reconnectTimer->stop();
        emit serialConnectionStatusChanged(false, "Failed to reconnect after maximum attempts");
        emit reconnectionStatusChanged(m_reconnectAttempts, m_maxReconnectAttempts, false);
        return;
    }*/
    
    m_reconnectAttempts++;
    
    bool success = false;
    emit reconnectionStatusChanged(m_reconnectAttempts, m_maxReconnectAttempts, false);
    
    // Different reconnection strategy
    if (m_reconnectStrategy == ReconnectStrategy::MaxPortScan) {
        QString portToTry = getNextPortToScan();
        if (portToTry.isEmpty()) {
            // Tried all ports, stop or wait and try again
            if (m_reconnectAttempts >= m_maxReconnectAttempts) {
                m_reconnectTimer->stop();
                emit serialConnectionStatusChanged(false, "Failed to reconnect to any available port");
                emit reconnectionStatusChanged(m_reconnectAttempts, m_maxReconnectAttempts, false);
                return;
            } else {
                // Wait longer before next scan cycle
                m_reconnectTimer->setInterval(5000);
                return;
            }
        }
        
        qDebug() << "Attempting to reconnect to port" << portToTry;
        
        // Try to open this port
        if (openSerialPort(true, portToTry.toInt())) {
            success = true;
        }
    } else {
        // Standard reconnection
        qDebug() << "Attempting to reconnect... (Attempt" << m_reconnectAttempts << "of" << m_maxReconnectAttempts << ")";
        if (openSerialPort(true)) {
            success = true;
        }
    }
    
    if (success) {
        qDebug() << "Reconnection successful!";
        m_reconnectTimer->stop();
        emit serialConnectionStatusChanged(true, "Reconnected successfully");
        emit reconnectionStatusChanged(m_reconnectAttempts, m_maxReconnectAttempts, true);
    } else {
        qDebug() << "Reconnection attempt failed. Will try again...";
        emit serialConnectionStatusChanged(false, QString("Reconnection attempt %1 failed").arg(m_reconnectAttempts));
        
        // Calculate next interval based on strategy
        int nextInterval = calculateNextReconnectInterval();
        if (nextInterval > 0 && nextInterval != m_reconnectTimer->interval()) {
            m_reconnectTimer->setInterval(nextInterval);
        }
    }
}

int CSerialPort::sendSerialData(const QString &data)
{
    if (!IsOpen() || data.isEmpty()) {
        return -1;
    }
    
    QByteArray byteArray = textToBytes(data);
    qDebug() << "sendSerialData" << byteArray.size();
    int bytesSent = Send(reinterpret_cast<unsigned char*>(byteArray.data()), byteArray.size());
    
    if (bytesSent > 0) {
        // 로그 기록
        QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
        QString formattedData = formatData(reinterpret_cast<const unsigned char*>(byteArray.constData()), byteArray.size());
        
        Log(QString("Sent: %1").arg(formattedData));
        
        // Emit the base class signal
        emit dataSent(bytesSent);
    }
    
    return bytesSent;
}


int CSerialPort::sendSerialData(unsigned char *data, int length)
{
    if (!IsOpen() || data == nullptr) {
        return -1;
    }
    
    int bytesSent = Send(data, length);
    
    if (bytesSent > 0) {
        // 로그 기록
        QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
        QString formattedData = formatData(data, length);
        
        Log(QString("Sent: %1").arg(formattedData));
        
        // Emit the base class signal
        emit dataSent(bytesSent);
    }
    
    return bytesSent;
}

int CSerialPort::sendRawData(const QByteArray &data)
{
    if (!IsOpen() || data.isEmpty()) {
        return -1;
    }
    
    int bytesSent = Send(reinterpret_cast<unsigned char*>(const_cast<char*>(data.constData())), data.size());
    
    if (bytesSent > 0) {
        // 로그 기록
        QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
        QString formattedData = formatData(reinterpret_cast<const unsigned char*>(data.constData()), data.size());
        
        Log(QString("Sent raw: %1").arg(formattedData));
        
        // Emit the base class signal
        emit dataSent(bytesSent);
    }
    
    return bytesSent;
}

QByteArray CSerialPort::textToBytes(const QString &text) const
{
    return text.toUtf8();
}

QByteArray CSerialPort::hexToBytes(const QString &hexString) const
{
    QByteArray result;
    
    // Remove any spaces and split into pairs
    QString cleanHex = hexString.simplified().replace(" ", "");
    
    // Check if string length is even
    if (cleanHex.length() % 2 != 0) {
        cleanHex = cleanHex.left(cleanHex.length() - 1);
    }
    
    // Convert hex pairs to bytes
    for (int i = 0; i < cleanHex.length(); i += 2) {
        bool ok;
        quint8 byte = cleanHex.mid(i, 2).toUInt(&ok, 16);
        if (ok) {
            result.append(byte);
        }
    }
    
    return result;
}

void CSerialPort::onTransactionCompleted(int id, const QByteArray& response)
{
    QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
    QString formattedData = formatData(reinterpret_cast<const unsigned char*>(response.constData()), response.size());
    
    Log(QString("Transaction %1 completed: %2").arg(id).arg(formattedData));
    
    // Forward the signal
    emit transactionCompleted(id, response);
}

void CSerialPort::onTransactionTimeout(int id)
{
    Log(QString("Transaction %1 timed out").arg(id));
    
    // Forward the signal
    emit transactionTimeout(id);
}

QByteArray CSerialPort::performTransaction(const QByteArray& request, int timeout)
{
    if (!IsOpen() || request.isEmpty() || timeout <= 0) {
        return QByteArray();
    }
    
    // Start transaction
    int id = startTransaction(request, timeout);
    if (id < 0) {
        return QByteArray();
    }
    
    // Wait for completion or timeout
    QElapsedTimer timer;
    timer.start();
    
    while (timer.elapsed() < timeout && m_transactionHandler->isTransactionPending(id)) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    }
    
    // Get response
    return m_transactionHandler->getResponse(id);
}

int CSerialPort::startTransaction(const QByteArray& request, int timeout)
{
    if (!IsOpen() || !m_transactionHandler || request.isEmpty() || timeout <= 0) {
        return -1;
    }
    
    // Add transaction
    int id = m_transactionHandler->addTransaction(request, timeout);
    if (id < 0) {
        return -1;
    }
    
    // Send request
    int bytesSent = Send(reinterpret_cast<unsigned char*>(const_cast<char*>(request.constData())), request.size());
    if (bytesSent < 0) {
        return -1;
    }
    
    return id;
}

int CSerialPort::calculateNextReconnectInterval() const
{
    switch (m_reconnectStrategy) {
        case ReconnectStrategy::None:
            return -1; // No reconnection
            
        case ReconnectStrategy::SimpleRetry:
            return m_reconnectInterval; // Fixed interval
            
        case ReconnectStrategy::ExponentialBackoff:
            // Base interval with exponential backoff (up to 1 minute)
            return qMin(m_reconnectInterval * (1 << qMin(m_reconnectAttempts, 10)), 60000);
            
        case ReconnectStrategy::MaxPortScan:
            // Fixed interval for port scanning
            return 1000; // Try next port after 1 second
            
        default:
            return m_reconnectInterval;
    }
}

void CSerialPort::resetPortScanList()
{
    m_portScanList = getAvailablePorts();
    m_currentPortScanIndex = 0;
}

QString CSerialPort::getNextPortToScan()
{
    if (m_portScanList.isEmpty()) {
        resetPortScanList();
    }
    
    if (m_currentPortScanIndex >= m_portScanList.size()) {
        m_currentPortScanIndex = 0;
        return QString();
    }
    
    return m_portScanList.at(m_currentPortScanIndex++);
}

// SerialPortManager implementation
SerialPortManager& SerialPortManager::instance()
{
    static SerialPortManager instance;
    return instance;
}

void SerialPortManager::addSerialPort(CSerialPort* port, const QString& name)
{
    if (port && !name.isEmpty()) {
        m_ports[name] = port;
        emit portAdded(name);
    }
}

CSerialPort* SerialPortManager::getSerialPort(const QString& name)
{
    auto it = m_ports.find(name);
    return (it != m_ports.end()) ? it.value() : nullptr;
}

QList<CSerialPort*> SerialPortManager::getAllSerialPorts() const
{
    return m_ports.values();
}

int SerialPortManager::startAllPorts()
{
    int count = 0;
    for (auto port : m_ports.values()) {
        if (port->openSerialPort()) {
            count++;
        }
    }
    return count;
}

void SerialPortManager::stopAllPorts()
{
    for (auto port : m_ports.values()) {
        port->ServiceStop();
    }
} 
