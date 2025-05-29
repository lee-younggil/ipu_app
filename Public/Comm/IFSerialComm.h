#ifndef IFSERIALCOMM_H
#define IFSERIALCOMM_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <QDebug>

#include "IFComm.h"

/**
 * @brief Common baud rates array size
 */
#define NUM_BAUDRATE 15

/**
 * @brief External array of supported baud rates
 */
extern int g_iaBaudRate[NUM_BAUDRATE];

//#define IN_QUE_MAX_SIZE		( 1024 )
//#define OUT_QUE_MAX_SIZE	( 1024 )

/**
 * @brief Flow control constants
 */
#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

/**
 * @brief ASCII control characters
 */
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

/**
 * @brief The SerialProtocolHandler class - Basic serial protocol handler
 *
 * Implements IProtocolHandler for basic serial communication protocols
 */
class SerialProtocolHandler : public IProtocolHandler {
public:
    /**
     * @brief Process received packet 
     * @param data Raw data
     * @param size Data size
     * @return True if processed successfully
     */
    virtual bool processPacket(const unsigned char* data, unsigned int size) override {
        // Basic implementation - can be overridden by derived classes
        return (data != nullptr && size > 0);
    }
    
    /**
     * @brief Create a packet for serial communication
     * @param command Command code
     * @param payload Data payload
     * @return Formatted packet
     */
    virtual QByteArray createPacket(int command, const QByteArray& payload) override {
        // Basic packet format: [Command(1)][Length(1)][Payload(n)][Checksum(1)]
        QByteArray packet;
        packet.append(static_cast<char>(command & 0xFF));
        packet.append(static_cast<char>(payload.size() & 0xFF));
        packet.append(payload);
        
        // Calculate simple checksum
        unsigned char checksum = 0;
        for (int i = 0; i < packet.size(); i++) {
            checksum ^= static_cast<unsigned char>(packet[i]);
        }
        packet.append(static_cast<char>(checksum));
        
        return packet;
    }
};

/**
 * @brief The CIFSerialComm class - Serial communication interface
 * 
 * Implements the communication interface for serial ports using QSerialPort.
 * Provides functionality for configuring and communicating over serial connections.
 */
class CIFSerialComm : public CIFComm
{
    Q_OBJECT

public:
    /**
     * @enum SerialParams
     * @brief Serial port parameters
     */
    enum class SerialParams {
        DataBits5 = QSerialPort::Data5,
        DataBits6 = QSerialPort::Data6,
        DataBits7 = QSerialPort::Data7,
        DataBits8 = QSerialPort::Data8,
        
        StopBits1 = QSerialPort::OneStop,
        StopBits2 = QSerialPort::TwoStop,
        
        ParityNone = QSerialPort::NoParity,
        ParityOdd = QSerialPort::OddParity,
        ParityEven = QSerialPort::EvenParity
    };

    /**
     * @enum SerialError
     * @brief Serial communication error codes
     */
    enum class SerialError {
        NoError = 0,
        OpenError,
        ConfigError,
        WriteError,
        ReadError,
        NotOpenError,
        TimeoutError,
        UnknownError
    };

    /**
     * @brief Constructor
     * @param hMediator Mediator pointer
     * @param strLogSource Log source identifier
     */
    //CIFSerialComm();
    CIFSerialComm(CMediator *hMediator, const QString &strLogSource);
    
    /**
     * @brief Destructor
     */
    virtual ~CIFSerialComm();

    // CIFComm interface implementation
    virtual bool Connect() override;
    virtual bool Close() override;
    virtual int Send(unsigned char *pucaBuf, const unsigned int uiSize) override;
    virtual int Recv(unsigned char *pucaBuf, const unsigned int uiSize) override;
    virtual bool IsOpen() override;
    virtual bool IsRecv() override;
    virtual bool IsSend(qint16 dwSendLen) override;
    virtual void Log(const QString &strLog, const void *pPacket = nullptr, unsigned int uiPacketLength = 0) override;

    /**
     * @brief Set the serial port number
     * @param iPort Port number
     */
    void SetPort(int iPort);
    
    /**
     * @brief Set the baud rate
     * @param iBaud Baud rate value
     */
    void SetBaudRate(int iBaud);
    
    /**
     * @brief Set input queue size
     * @param dwInQueueSize Input queue size
     */
    void SetInQueueSize(qint16 dwInQueueSize);
    
    /**
     * @brief Set output queue size
     * @param dwOutQueueSize Output queue size
     */
    void SetOutQueueSize(qint16 dwOutQueueSize);
    
    /**
     * @brief Set shared device (legacy function, not used with QSerialPort)
     * @param hShareDevice Shared device pointer
     */
    void SetShareDevice(void* hShareDevice);
    
    /**
     * @brief Set data bits (5-8)
     * @param dataBits Data bits value
     */
    void SetDataBits(SerialParams dataBits);
    
    /**
     * @brief Set stop bits (1 or 2)
     * @param stopBits Stop bits value
     */
    void SetStopBits(SerialParams stopBits);
    
    /**
     * @brief Set parity (None, Even, Odd)
     * @param parity Parity value
     */
    void SetParity(SerialParams parity);
    
    /**
     * @brief Set flow control
     * @param flowControl Flow control value
     */
    void SetFlowControl(QSerialPort::FlowControl flowControl);

    /**
     * @brief Open serial port with specific settings
     * @param portName Serial port name
     * @param baudRate Baud rate
     * @param dataBits Data bits
     * @param parity Parity
     * @param stopBits Stop bits
     * @param flowControl Flow control
     * @return True if successful, false otherwise
     */
    bool openSerialPort(const QString& portName, int baudRate = 115200,
                        QSerialPort::DataBits dataBits = QSerialPort::Data8,
                        QSerialPort::Parity parity = QSerialPort::NoParity,
                        QSerialPort::StopBits stopBits = QSerialPort::OneStop,
                        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl);

    /**
     * @brief Get available serial ports
     * @return List of available port names
     */
    QStringList getAvailablePorts() const;

    /**
     * @brief Get the current baud rate
     * @return Current baud rate
     */
    int GetBaudRate() const { return m_iBaud; }
    
    /**
     * @brief Get the current port number
     * @return Current port number
     */
    int getPort() const { return m_iPort; }
    
    /**
     * @brief Get the QSerialPort instance
     * @return Pointer to QSerialPort object
     */
    QSerialPort* getSerialPort() { return m_serialPort; }

    /**
     * @brief Get the last error
     * @return Last error code
     */
    SerialError getLastError() const { return m_lastError; }

    /**
     * @brief Get error message for error code
     * @param error Error code
     * @return Error message
     */
    static QString getErrorMessage(SerialError error);

signals:
    /**
     * @brief Signal emitted when serial port is connected
     */
    void serialPortConnected();
    
    /**
     * @brief Signal emitted when serial port is disconnected
     */
    void serialPortDisconnected();
    
    /**
     * @brief Signal emitted when a serial port error occurs
     * @param error Error message
     */
    void serialPortError(const QString &error);

protected:
    /**
     * @brief Check how many bytes are waiting to be read
     * @return Number of bytes available
     */
    int ReadDataWaiting();
    
    /**
     * @brief List available serial ports
     * @param portNum Array to fill with port numbers
     * @return Number of ports found
     */
    int EnumerateSerialPorts(int portNum[]);
    
    /**
     * @brief Write a single byte to the serial port
     * @param ucByte Byte to write
     * @return True if successful, false otherwise
     */
    bool WriteCommByte(unsigned char ucByte);

    /**
     * @brief Set last error and emit error signal
     * @param error Error code
     * @param message Custom error message (optional)
     */
    void setError(SerialError error, const QString& message = QString());

private slots:
    /**
     * @brief Handle serial port errors
     * @param error Serial port error
     */
    void handleSerialError(QSerialPort::SerialPortError error);

    /**
     * @brief Handle data ready to read
     */
    void handleReadyRead();

private:
    int m_iPort;                  ///< Port number
    int m_iBaud;                  ///< Baud rate
    bool m_bOpened;               ///< Open status flag
    void* m_hIDComDev;            ///< Legacy handle
    qint16 m_dwInQueueSize;       ///< Input queue size
    qint16 m_dwOutQueueSize;      ///< Output queue size
    QSerialPort* m_serialPort;    ///< Qt serial port object
    SerialError m_lastError;      ///< Last error code
};

#endif // IFSERIALCOMM_H
