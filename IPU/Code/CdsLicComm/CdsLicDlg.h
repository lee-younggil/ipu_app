#ifndef CDSLICDLG_H
#define CDSLICDLG_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

class CSerialPort;  // Forward declaration
class CCdsLicComm;  // Forward declaration

/**
 * @brief The CdsLicDlg class - Dialog for CDS License Communication
 * Provides UI for serial communication and license management
 */
class CdsLicDlg : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit CdsLicDlg(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~CdsLicDlg();

    /**
     * @brief Set the serial port for communication
     * @param port Pointer to the serial port
     */
    void setSerialPort(CSerialPort* port) { m_serialPort = port; }
    
    /**
     * @brief Set the CDS license communication handler
     * @param comm Pointer to the communication handler
     */
    void setCdsLicComm(CCdsLicComm* comm);

public slots:
    /**
     * @brief Update the serial connection status in the UI
     * @param connected Connection status
     * @param message Status message
     */
    void updateSerialConnectionStatus(bool connected, const QString &message);
    
    /**
     * @brief Append received serial data to the text display
     * @param timestamp Timestamp of reception
     * @param data Received data
     */
    void appendSerialData(const QString &timestamp, const QString &data);
    
    /**
     * @brief Handle CDS license communication events
     * @param event Event message
     */
    void onCdsLicCommEvent(const QString &event);
    
    /**
     * @brief Handle CDS license communication errors
     * @param error Error message
     */
    void onCdsLicCommError(const QString &error);

protected:
    /**
     * @brief Create the UI layout
     */
    void CreateLayout();

private slots:    
    void onFunc1ButtonClicked();
    void onFunc2ButtonClicked();
    void onFunc3ButtonClicked();
    /**
     * @brief Handle send button click
     */
    void onSendButtonClicked();
    
    /**
     * @brief Handle return key press in line edit
     */
    void onLineEditReturnPressed();

private:
    CSerialPort* m_serialPort;  ///< Serial port for communication
    CCdsLicComm* m_cdsLicComm;  ///< CDS license communication handler
    QTextEdit* m_textEdit;      ///< Text display for communication log
    QTextEdit* m_textEdit2;     ///< Secondary text display (for events/status)
    QLineEdit* m_lineEdit;      ///< Line edit for sending data
    QLabel* m_statusLabel;      ///< Label for connection status
    QPushButton* m_sendButton;  ///< Button for sending data
    QPushButton* m_func1Button;
    QPushButton* m_func2Button;
    QPushButton* m_func3Button;
    QLineEdit* m_funcLineEdit;
};

#endif // CDSLICDLG_H
