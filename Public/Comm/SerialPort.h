#ifndef CSERIALPORT_H
#define CSERIALPORT_H

#include <QObject>
#include <QTimer>
#include <QByteArray>
#include <QDateTime>
#include <QQueue>
#include <memory>
#include "Public/Comm/IFSerialComm.h"

class CdsLicDlg;

/**
 * @brief The SerialTransactionHandler class - Handles serial transactions
 * 
 * Provides functionality for managing request-response transactions
 * over serial communication.
 */
class SerialTransactionHandler : public QObject {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit SerialTransactionHandler(QObject* parent = nullptr);
    
    /**
     * @struct Transaction
     * @brief Represents a serial transaction
     */
    struct Transaction {
        int id;                 ///< Transaction ID
        QByteArray request;     ///< Request data
        QByteArray response;    ///< Response data
        qint64 timestamp;       ///< Transaction timestamp
        int timeout;            ///< Transaction timeout (ms)
        bool completed;         ///< Transaction completion flag
    };
    
    /**
     * @brief Add a transaction
     * @param request Request data
     * @param timeout Transaction timeout
     * @return Transaction ID
     */
    int addTransaction(const QByteArray& request, int timeout = 1000);
    
    /**
     * @brief Process received data
     * @param data Received data
     * @return True if data was handled as part of a transaction
     */
    bool processResponse(const QByteArray& data);
    
    /**
     * @brief Check if transaction is pending
     * @param id Transaction ID
     * @return True if transaction is pending
     */
    bool isTransactionPending(int id) const;
    
    /**
     * @brief Get transaction response
     * @param id Transaction ID
     * @return Response data
     */
    QByteArray getResponse(int id) const;
    
    /**
     * @brief Clear all transactions
     */
    void clearTransactions();

signals:
    /**
     * @brief Signal emitted when transaction is completed
     * @param id Transaction ID
     * @param response Response data
     */
    void transactionCompleted(int id, const QByteArray& response);
    
    /**
     * @brief Signal emitted when transaction times out
     * @param id Transaction ID
     */
    void transactionTimeout(int id);

private slots:
    /**
     * @brief Check for timed out transactions
     */
    void checkTimeouts();

private:
    QMap<int, Transaction> m_transactions;  ///< Transaction map
    QTimer* m_timeoutTimer;                 ///< Transaction timeout timer
    int m_nextId;                           ///< Next transaction ID
};

/**
 * @brief The CSerialPort class - Advanced serial port communication handler
 * 
 * Extends the base serial interface with additional features:
 * - Automatic reconnection
 * - UI integration 
 * - Extended error handling
 * - Data formatting support
 * - Transaction management
 */
class CSerialPort : public CIFSerialComm
{
    Q_OBJECT
public:
    /**
     * @enum DataFormat
     * @brief Format options for data representation
     */
    enum class DataFormat {
        Hex,      ///< Hexadecimal format
        Ascii,    ///< ASCII format
        Decimal,  ///< Decimal format
        Mixed     ///< Combined formats
    };
    
    /**
     * @enum ReconnectStrategy
     * @brief Reconnection strategies
     */
    enum class ReconnectStrategy {
        None,               ///< No automatic reconnection
        SimpleRetry,        ///< Simple retry with fixed interval
        ExponentialBackoff, ///< Exponential backoff between attempts
        MaxPortScan         ///< Try all available ports
    };

    /**
     * @brief Constructor
     * @param hMediator Mediator pointer
     * @param strLogSource Log source identifier
     */
    explicit CSerialPort();
    explicit CSerialPort(CMediator *hMediator, const QString &strLogSource);
    
    /**
     * @brief Destructor
     */
    virtual ~CSerialPort();

    /**
     * @brief Set dialog for UI updates
     * @param dialog Pointer to dialog
     */
    void setDialog(CdsLicDlg* dialog);
    
    /**
     * @brief Set data format for display and logging
     * @param format Data format
     */
    void setDataFormat(DataFormat format) { m_dataFormat = format; }
    
    /**
     * @brief Get current data format
     * @return Current data format
     */
    DataFormat getDataFormat() const { return m_dataFormat; }
    
    /**
     * @brief Set reconnection strategy
     * @param strategy Reconnection strategy
     */
    void setReconnectStrategy(ReconnectStrategy strategy) { m_reconnectStrategy = strategy; }
    
    /**
     * @brief Get current reconnection strategy
     * @return Current reconnection strategy
     */
    ReconnectStrategy getReconnectStrategy() const { return m_reconnectStrategy; }
    
    /**
     * @brief Get transaction handler
     * @return Transaction handler instance
     */
    SerialTransactionHandler* getTransactionHandler() { return m_transactionHandler.get(); }

    /**
     * @brief Initialize serial port with default settings
     */
    virtual void initialization(void) override;
    
    /**
     * @brief Open and configure serial port
     * @param isReconnect Whether this is a reconnection attempt
     * @param portToTry Specific port to try, -1 for auto-detection
     * @return true if successful, false otherwise
     */
    virtual bool openSerialPort(bool isReconnect = false, int portToTry = -1);
    
    /**
     * @brief Stop the service and close connection
     * @return true if successful, false otherwise
     */
    virtual bool ServiceStop(void) override;
    
    /**
     * @brief Configure the reconnection parameters
     * @param interval Reconnection interval in milliseconds
     * @param maxAttempts Maximum number of reconnection attempts
     */
    void configureReconnection(int interval, int maxAttempts);
    
    /**
     * @brief Format data for display/logging according to current format setting
     * @param data Raw data buffer
     * @param size Size of data
     * @return Formatted string
     */
    QString formatData(const unsigned char* data, unsigned int size) const;
    
    /**
     * @brief Perform a synchronous serial transaction
     * @param request Request data
     * @param timeout Timeout in milliseconds
     * @return Response data, empty on error or timeout
     */
    QByteArray performTransaction(const QByteArray& request, int timeout = 1000);
    
    /**
     * @brief Start an asynchronous serial transaction
     * @param request Request data
     * @param timeout Timeout in milliseconds
     * @return Transaction ID, negative on error
     */
    int startTransaction(const QByteArray& request, int timeout = 1000);

    

    /**
     * @brief Convert ASCII string to bytes
     * @param text ASCII text
     * @return Byte array
     */
    QByteArray textToBytes(const QString &text) const;

signals:
    /**
     * @brief Signal emitted when connection status changes
     * @param connected Connection status
     * @param message Status message
     */
    void serialConnectionStatusChanged(bool connected, const QString &message);
    
    /**
     * @brief Signal emitted when data is received
     * @param timestamp Timestamp of reception
     * @param data Received data
     */
    void serialDataReceived(const QString &timestamp, const QString &data);
    
    /**
     * @brief Signal emitted when reconnection process changes state
     * @param attempt Current attempt number
     * @param maxAttempts Maximum attempts
     * @param success Whether reconnection was successful
     */
    void reconnectionStatusChanged(int attempt, int maxAttempts, bool success);
    
    /**
     * @brief Signal emitted when a transaction is completed
     * @param id Transaction ID
     * @param response Response data
     */
    void transactionCompleted(int id, const QByteArray& response);
    
    /**
     * @brief Signal emitted when a transaction times out
     * @param id Transaction ID
     */
    void transactionTimeout(int id);
    

public slots:

    int sendSerialData(unsigned char *data, int length);
    /**
     * @brief Send data over serial port
     * @param data Data to send
     * @return Number of bytes sent, negative on error
     */
    int sendSerialData(const QString &data);
    
    /**
     * @brief Send raw data over serial port
     * @param data Raw data to send
     * @return Number of bytes sent, negative on error
     */
    int sendRawData(const QByteArray &data);

private slots:
    void onSerialPortConnected();
    void onSerialPortDisconnected();
    void onSerialPortError(const QString &error);
    void onSerialConnectionStatusChanged(bool connected, const QString &message);
    void onSerialDataReady();
    void tryReconnect();
    void onTransactionCompleted(int id, const QByteArray& response);
    void onTransactionTimeout(int id);

protected:
    /**
     * @brief Format log messages
     * @param strLog Log message
     * @param pPacket Optional packet data
     * @param uiPacketLength Length of packet data
     */
    virtual void Log(const QString &strLog, const void *pPacket = nullptr, unsigned int uiPacketLength = 0) override;

private:
    CdsLicDlg* m_dialog;              ///< Dialog for UI updates
    QTimer* m_readTimer;              ///< Timer for reading data
    QTimer* m_reconnectTimer;         ///< Timer for reconnection attempts
    QString m_errorMessage;           ///< Last error message
    int m_reconnectAttempts;          ///< Number of reconnection attempts
    DataFormat m_dataFormat;          ///< Data format for display/logging
    ReconnectStrategy m_reconnectStrategy; ///< Reconnection strategy
    QStringList m_portScanList;       ///< Ports to try during reconnection
    int m_currentPortScanIndex;       ///< Current port scan index
    std::unique_ptr<SerialTransactionHandler> m_transactionHandler; ///< Transaction handler
    
    // Configuration constants
    int m_readInterval;              ///< Interval for reading data (ms)
    int m_maxReadSize;               ///< Maximum size of read buffer
    int m_reconnectInterval;         ///< Interval between reconnection attempts (ms)
    int m_maxReconnectAttempts;      ///< Maximum number of reconnection attempts
    
    /**
     * @brief Process received data
     * @param data Received data
     */
    void processReceivedData(const QByteArray &data);
    
    /**
     * @brief Convert hexadecimal string to bytes
     * @param hexString Hex string
     * @return Byte array
     */
    QByteArray hexToBytes(const QString &hexString) const;
    
    /**
     * @brief Calculate next reconnect interval based on strategy
     * @return Next reconnect interval in milliseconds
     */
    int calculateNextReconnectInterval() const;
    
    /**
     * @brief Get next port to try during port scanning
     * @return Port to try, empty string if no more ports
     */
    QString getNextPortToScan();
    
    /**
     * @brief Reset port scan list
     */
    void resetPortScanList();
};

/**
 * @brief The SerialPortManager class - Manages multiple serial ports
 * 
 * Singleton class that manages multiple serial port instances.
 */
class SerialPortManager : public QObject {
    Q_OBJECT
    
public:
    /**
     * @brief Get instance of the manager
     * @return Manager instance
     */
    static SerialPortManager& instance();
    
    /**
     * @brief Add a serial port
     * @param port Serial port to add
     * @param name Port name/identifier
     */
    void addSerialPort(CSerialPort* port, const QString& name);
    
    /**
     * @brief Get serial port by name
     * @param name Port name/identifier
     * @return Serial port instance or nullptr if not found
     */
    CSerialPort* getSerialPort(const QString& name);
    
    /**
     * @brief Get all registered serial ports
     * @return List of serial ports
     */
    QList<CSerialPort*> getAllSerialPorts() const;
    
    /**
     * @brief Start all serial ports
     * @return Number of successfully started ports
     */
    int startAllPorts();
    
    /**
     * @brief Stop all serial ports
     */
    void stopAllPorts();

signals:
    /**
     * @brief Signal emitted when a port is added
     * @param name Port name/identifier
     */
    void portAdded(const QString& name);
    
private:
    SerialPortManager() {} // Private constructor for singleton
    QMap<QString, CSerialPort*> m_ports; ///< Registered ports
};

#endif // CSERIALPORT_H 
