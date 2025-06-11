#ifndef IFCOMM_H
#define IFCOMM_H

#include <QObject>
#include <QByteArray>
#include <QMap>
#include <functional>
#include <memory>
#include "IFService.h"
#include "DataTypeDefine.h"
#include "IFService.h"
#include "Service.h"


/**
 * @brief The IProtocolHandler class - Protocol handling interface
 *
 * Abstract interface for protocol handlers that can process and create packets
 * according to specific communication protocols.
 */
class IProtocolHandler {
public:
    virtual ~IProtocolHandler() = default;
    
    /**
     * @brief Process received packet according to protocol
     * @param data Raw data buffer
     * @param size Size of data
     * @return True if packet was valid and processed successfully
     */
    virtual bool processPacket(const unsigned char* data, unsigned int size) = 0;
    
    /**
     * @brief Create packet according to protocol
     * @param command Command identifier
     * @param payload Data payload
     * @return Formatted packet as byte array
     */
    virtual QByteArray createPacket(int command, const QByteArray& payload) = 0;
};

/**
 * @brief The CIFComm class - Base communication interface
 * 
 * Abstract base class defining the interface for all communication types.
 * Provides fundamental communication operations and logging.
 */
class CIFComm : public QObject, public CIFService
{
    Q_OBJECT
public:
    /**
     * @enum CommStatus
     * @brief Communication status enumeration
     */
    enum class CommStatus {
        Connected,      ///< Connection established
        Disconnected,   ///< Connection terminated
        Error,          ///< Error condition
        Reconnecting,   ///< Attempting to reconnect
        Initializing    ///< Initializing connection
    };

    /**
     * @enum CommType
     * @brief Communication type enumeration
     */
    enum class CommType {
        Serial,         ///< Serial communication
        TCP,            ///< TCP/IP communication
        UDP,            ///< UDP communication
        Bluetooth,      ///< Bluetooth communication
        Custom          ///< Custom communication
    };

    /**
     * @brief Constructor
     * @param hMediator Mediator for service coordination
     * @param strLogSource Source identifier for logs
     * @param type Communication type
     */
    CIFComm();
    CIFComm(CMediator *hMediator, const QString &strLogSource, CommType type = CommType::Serial)
    : CIFService(hMediator, strLogSource), m_commType(type), m_protocolHandler(nullptr)
    {
    }
    
    /**
     * @brief Virtual destructor
     */
    virtual ~CIFComm() = default;

    /**
     * @brief Log communication information
     * @param strLog Log message
     * @param pPacket Optional packet data
     * @param uiPacketLength Length of packet data
     */
    virtual void Log(const QString &strLog, const void *pPacket = nullptr, unsigned int uiPacketLength = 0) = 0;

    /**
     * @brief Establish connection
     * @return True if connection successful, false otherwise
     */
    virtual bool Connect() = 0;
    
    /**
     * @brief Close the connection
     * @return True if disconnection successful, false otherwise
     */
    virtual bool Close() = 0;

    /**
     * @brief Send data
     * @param pucaBuf Data buffer to send
     * @param uiSize Size of data to send
     * @return Number of bytes sent, negative on error
     */
    virtual int Send(unsigned char *pucaBuf, const unsigned int uiSize) = 0;
    
    /**
     * @brief Receive data
     * @param pucaBuf Buffer to receive data
     * @param uiSize Maximum size to receive
     * @return Number of bytes received, negative on error
     */
    virtual int Recv(unsigned char *pucaBuf, const unsigned int uiSize) = 0;

    /**
     * @brief Check if connection is open
     * @return True if open, false otherwise
     */
    virtual bool IsOpen() = 0;
    
    /**
     * @brief Check if data is available to receive
     * @return True if data is available, false otherwise
     */
    virtual bool IsRecv() = 0;
    
    /**
     * @brief Check if can send data of specified length
     * @param dwSendLen Length of data to send
     * @return True if can send, false otherwise
     */
    virtual bool IsSend(qint16 dwSendLen) = 0;
    
    /**
     * @brief Set protocol handler
     * @param handler Protocol handler instance
     */
    void setProtocolHandler(IProtocolHandler* handler) {
        m_protocolHandler = handler;
    }
    
    /**
     * @brief Get protocol handler
     * @return Current protocol handler
     */
    IProtocolHandler* getProtocolHandler() const {
        return m_protocolHandler;
    }
    
    /**
     * @brief Get communication type
     * @return Communication type
     */
    CommType getCommType() const {
        return m_commType;
    }
    
    /**
     * @brief Set data reception callback
     * @param callback Function to call when data is received
     */
    void setDataReceivedCallback(std::function<void(const unsigned char*, unsigned int)> callback) {
        m_dataReceivedCallback = callback;
    }

signals:
    /**
     * @brief Signal emitted when connection status changes
     * @param status New connection status
     * @param message Additional status message
     */
    void connectionStatusChanged(CommStatus status, const QString &message);
    
    /**
     * @brief Signal emitted when data is received
     * @param data Received data
     * @param size Size of received data
     */
    void dataReceived(const unsigned char *data, unsigned int size);
    
    /**
     * @brief Signal emitted when data is sent
     * @param bytesSent Number of bytes sent
     */
    void dataSent(int bytesSent);
    
    /**
     * @brief Signal emitted when error occurs
     * @param errorMessage Error message
     * @param errorCode Error code (if available)
     */
    void errorOccurred(const QString &errorMessage, int errorCode = 0);

protected:
    CommType m_commType;                                           ///< Communication type
    IProtocolHandler* m_protocolHandler;                           ///< Protocol handler
    std::function<void(const unsigned char*, unsigned int)> m_dataReceivedCallback; ///< Data reception callback
    
    /**
     * @brief Process received data through protocol handler or callback
     * @param data Received data buffer
     * @param size Size of data
     */
    void processReceivedData(const unsigned char* data, unsigned int size) {
        // Emit the data received signal
        emit dataReceived(data, size);
        
        // Process through protocol handler if available
        if (m_protocolHandler) {
            m_protocolHandler->processPacket(data, size);
        }
        
        // Call the callback if set
        if (m_dataReceivedCallback) {
            m_dataReceivedCallback(data, size);
        }
    }
};

/**
 * @brief The CommPluginManager class - Communication plugin manager
 *
 * Manages communication plugins and provides a central point for accessing
 * different communication implementations.
 */
class CommPluginManager : public QObject {
    Q_OBJECT
    
public:
    /**
     * @brief Get singleton instance
     * @return Reference to singleton instance
     */
    static CommPluginManager& instance() {
        static CommPluginManager instance;
        return instance;
    }
    
    /**
     * @brief Register a communication plugin
     * @param plugin Plugin to register
     * @param name Plugin name/identifier
     */
    void registerCommPlugin(CIFComm* plugin, const QString& name) {
        if (plugin) {
            m_plugins[name] = plugin;
            emit pluginRegistered(name);
        }
    }

#if 1
    /**
     * @brief Get plugin by name
     * @param name Plugin name/identifier
     * @return Pointer to plugin or nullptr if not found
     */
    CIFComm* getPlugin(const QString& name) {
        auto it = m_plugins.find(name);
        return (it != m_plugins.end()) ? it.value() : nullptr;
    }
    
    /**
     * @brief Get plugin by type
     * @param type Communication type
     * @return First plugin of specified type or nullptr if not found
     */
    CIFComm* getPluginByType(CIFComm::CommType type) {
        for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it) {
            if (it.value()->getCommType() == type) {
                return it.value();
            }
        }
        return nullptr;
    }
    
    /**
     * @brief Get all plugins of specified type
     * @param type Communication type
     * @return List of plugins of specified type
     */
    QList<CIFComm*> getPluginsByType(CIFComm::CommType type) {
        QList<CIFComm*> result;
        for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it) {
            if (it.value()->getCommType() == type) {
                result.append(it.value());
            }
        }
        return result;
    }
#endif

signals:
    /**
     * @brief Signal emitted when a plugin is registered
     * @param name Plugin name/identifier
     */
    void pluginRegistered(const QString& name);
    
private:
    CommPluginManager() {} // Private constructor for singleton
    QMap<QString, CIFComm*> m_plugins; ///< Plugin registry
};

#endif // IFCOMM_H
