#ifndef CCDSCOMM_H
#define CCDSCOMM_H

#include "Comm/SerialPort.h"
#include "CompletionStruct.h"

/**
 * @brief The CCdsLicComm class - CDS Lic Communication class
 * Extends the Serial Port functionality with specific CDS lic handling
 */
class CCdsLicComm : public CSerialPort
{
    Q_OBJECT
public:
    /**
     * @brief Constructor
     * @param hMediator Mediator pointer for communication
     * @param strLogSource Log source identifier
     */
    //CCdsLicComm();
    CCdsLicComm(CMediator *hMediator, const QString &strLogSource);
    
    /**
     * @brief Destructor
     */
    virtual ~CCdsLicComm();

    /**
     * @brief Communication status enum
     */
    enum class Status {
        OK,                     ///< Everything is OK
        ERROR_PORT_UNAVAILABLE, ///< Serial port is not available
        ERROR_CONFIGURATION,    ///< Configuration error
        ERROR_COMMUNICATION     ///< Communication error
    };

    /**
     * @brief Initialize the communication
     */
    virtual void initialization(void) override;
    
    /**
     * @brief Start the communication service
     * @return true if successful, false otherwise
     */
    virtual bool ServiceStart(void);
    
    /**
     * @brief Stop the communication service
     * @return true if successful, false otherwise
     */
    virtual bool ServiceStop(void) override;

    /**
     * @brief Get the last error status
     * @return The last error status
     */
    Status getLastError() const { return m_lastError; }
    
    /**
     * @brief Get the error message
     * @return The error message
     */
    QString getErrorMessage() const { return m_errorMessage; }

signals:
    /**
     * @brief Signal emitted when an error occurs
     * @param status Error status
     * @param message Error message
     */
    void errorOccurred(Status status, const QString &message);
    
    /**
     * @brief Signal emitted when an event occurs
     * @param event Event message
     */
    void eventOccurred(const QString &event);

public:
//protected
    /**
     * @brief Log function override to format logs
     */
    virtual void Log(const QString &strLog, const void *pPacket = NULL, unsigned int uiPacketLength = 0) override;

public:
//private:
    Status m_lastError;     ///< Last error status
    QString m_errorMessage; ///< Last error message
    short m_usSeqNo;
    short testNum;
    
    /**
     * @brief Set the error status and message
     * @param status Error status
     * @param message Error message
     */
    void setError(Status status, const QString &message);

    void decode(const QByteArray& data);
    void decodeStatus(PACKET* data);
    void encodeControl(unsigned short usMsgID, char* pData, int flag, int value, char* reserved);
    void encodePacket(unsigned short usMsgID, char* pData);
    unsigned short GetSeqNo();
    QQueue<QByteArray> m_recvQueue; // Keep send queue if used elsewhere
};

#endif // CCDSCOMM_H
