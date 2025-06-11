#include "CdsLicDlg.h"
#include "Code/Mediator.h"
#include "Public/Comm/SerialPort.h"
#include "CdsLicComm.h"
#include <QDebug>
#include <QScrollBar>

CdsLicDlg::CdsLicDlg(QWidget *parent) : QWidget(parent)
    , m_serialPort(nullptr)
    , m_cdsLicComm(nullptr)
    , m_textEdit(nullptr)
    , m_textEdit2(nullptr)
    , m_lineEdit(nullptr)
    , m_statusLabel(nullptr)
    , m_sendButton(nullptr)
{
    CSerialPort* port = CMediator::getInstance()->getCdsLicComm();
    setSerialPort(port);
    port->setDialog(this);
    CreateLayout();
}

CdsLicDlg::~CdsLicDlg()
{
    // Nothing to clean up - the parent owns all QObjects
}

void CdsLicDlg::CreateLayout()
{
    // Create tab page for serial communication
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *layouth = new QHBoxLayout();
    
    // Create text editors for displaying communication
    m_textEdit = new QTextEdit();
    if (!m_textEdit) {
        qDebug() << "Failed to create text editor";
        return;
    }
    m_textEdit->setLineWrapMode(QTextEdit::NoWrap);
    m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_textEdit->setReadOnly(true);
    m_textEdit->setFont(QFont("Vadana", 9));
    m_textEdit->document()->setMaximumBlockCount(1000); // Limit stored blocks for performance
    
    m_textEdit2 = new QTextEdit();
    if (!m_textEdit2) {
        qDebug() << "Failed to create second text editor";
        return;
    }
    m_textEdit2->setLineWrapMode(QTextEdit::NoWrap);
    m_textEdit2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_textEdit2->setReadOnly(true);
    m_textEdit2->setFont(QFont("Vadana", 9));
    m_textEdit2->document()->setMaximumBlockCount(500); // Limit stored blocks for performance
    
    // Create send controls
    QHBoxLayout *layoutHSendCtrl = new QHBoxLayout();
    if (!layoutHSendCtrl) {
        qDebug() << "Failed to create horizontal layout";
        return;
    }
    m_funcLineEdit = new QLineEdit();
    if (!m_funcLineEdit) {
        qDebug() << "Failed to create line editor";
        return;
    }
    m_func1Button = new QPushButton(tr("Zoom"));
    if (!m_func1Button) {
        qDebug() << "Failed to create button";
        return;
    }
    m_func2Button = new QPushButton(tr("Focus"));
    if (!m_func2Button) {
        qDebug() << "Failed to create button";
        return;
    }
    m_func3Button = new QPushButton(tr("Iris"));
    if (!m_func3Button) {
        qDebug() << "Failed to create button";
        return;
    }
    
    m_lineEdit = new QLineEdit();
    if (!m_lineEdit) {
        qDebug() << "Failed to create line editor";
        return;
    }
    
    m_sendButton = new QPushButton(tr("Send"));
    if (!m_sendButton) {
        qDebug() << "Failed to create button";
        return;
    }

    // Create status label
    m_statusLabel = new QLabel(tr("Status: Not connected"));
    m_statusLabel->setStyleSheet("color: red;");
    
    layoutHSendCtrl->addWidget(m_funcLineEdit, 0.2);
    layoutHSendCtrl->addWidget(m_func1Button, 0.3);
    layoutHSendCtrl->addWidget(m_func2Button, 0.3);
    layoutHSendCtrl->addWidget(m_func3Button, 0.3);
    layoutHSendCtrl->addWidget(m_lineEdit, 1);
    layoutHSendCtrl->addWidget(m_statusLabel, 1);
    layoutHSendCtrl->addWidget(m_sendButton, 1);
    layout->addLayout(layoutHSendCtrl);
    
    // Add widgets to layouts
    layouth->addWidget(m_textEdit, 3);
    layouth->addWidget(m_textEdit2, 1);
    layout->addLayout(layouth);
    

    // Connect signals and slots
    connect(m_func1Button, &QPushButton::clicked, this, &CdsLicDlg::onFunc1ButtonClicked);
    connect(m_func2Button, &QPushButton::clicked, this, &CdsLicDlg::onFunc2ButtonClicked);
    connect(m_func3Button, &QPushButton::clicked, this, &CdsLicDlg::onFunc3ButtonClicked);
    connect(m_sendButton, &QPushButton::clicked, this, &CdsLicDlg::onSendButtonClicked);
    connect(m_lineEdit, &QLineEdit::returnPressed, this, &CdsLicDlg::onLineEditReturnPressed);
}

void CdsLicDlg::updateSerialConnectionStatus(bool connected, const QString &message)
{
    if (m_statusLabel) {
        QString statusText = QString("Status: %1").arg(message);
        m_statusLabel->setText(statusText);
        m_statusLabel->setStyleSheet(connected ? "color: green;" : "color: red;");
        
        // Log status change to second text edit
        if (m_textEdit2) {
            //QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
            //m_textEdit2->append(QString("[%1] Connection: %2").arg(timestamp).arg(message));
            m_textEdit2->append(QString(message));
        }
    }
}

void CdsLicDlg::appendSerialData(const QString &timestamp, const QString &data)
{
    if (m_textEdit) {
        // Format and append received data
        QByteArray byteArray = m_serialPort->textToBytes(data);
        QString formattedData = QString("[%1] [RX] %2").arg(timestamp).arg(data);
        m_textEdit->append(formattedData);
        
        // Scroll to bottom
        QScrollBar *scrollBar = m_textEdit->verticalScrollBar();
        if (scrollBar) {
            scrollBar->setValue(scrollBar->maximum());
        }
    }
}

void CdsLicDlg::onCdsLicCommEvent(const QString &event)
{
    if (m_textEdit2) {
        QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
        m_textEdit2->append(QString("[%1] Event: %2").arg(timestamp).arg(event));
        
        // Scroll to bottom
        QScrollBar *scrollBar = m_textEdit2->verticalScrollBar();
        if (scrollBar) {
            scrollBar->setValue(scrollBar->maximum());
        }
    }
}

void CdsLicDlg::onCdsLicCommError(const QString &error)
{
    if (m_textEdit2) {
        QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
        QString errorMessage = QString("[%1] ERROR: %2").arg(timestamp).arg(error);
        
        // Use HTML to colorize error message
        m_textEdit2->append(QString("<span style='color:red;'>%1</span>").arg(errorMessage));
        
        // Scroll to bottom
        QScrollBar *scrollBar = m_textEdit2->verticalScrollBar();
        if (scrollBar) {
            scrollBar->setValue(scrollBar->maximum());
        }
    }
}

void CdsLicDlg::onFunc1ButtonClicked()
{
    char flag = 1;
    char value = 0;
    if (m_funcLineEdit && !m_funcLineEdit->text().isEmpty() && m_serialPort) {
        QString data = m_funcLineEdit->text();
        qDebug() << "size:" << data.size();
        if (data.size() > 0 && data.size() < 3){
            if (data.at(data.size() - 1) > "0" && data.at(data.size() - 1) <= "9"){
                if (data.size() == 2 && data.at(0) == "-"){
                    flag = 0;
                    value = data.at(1).toLatin1() - '0';
                }else if (data.size() == 1){
                    flag = 1;
                    value = data.at(0).toLatin1() - '0';
                }else {
                    qDebug() << "invalid value";
                }
            }
            else{
                qDebug() << "invalid value";
            }
            
        }else{
            qDebug() << "invlid value";
        }
    }

    if (CMediator::getInstance()->getCdsLicComm()) {
        char message[PACKET_LENGTH_MAX];
        char *pMess = (char *)message;

        memset(message, 0, PACKET_LENGTH_MAX);
        m_cdsLicComm->encodeControl(MsgID_LicCamZoom, pMess, flag, value, nullptr);

        m_cdsLicComm->encodePacket(MsgID_LicCamZoom, pMess);

        QByteArray byteArray(reinterpret_cast<const char*>(pMess), 19);
        qDebug() << "CCdsLicComm::onFunc1ButtonClicked - Hex Data:" << byteArray.toHex(' ').toUpper(); 

        // Display sent data in main text edit
        QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");

        //QByteArray byteArray = m_serialPort->textToBytes(packetEncode);
        QString formattedData = QString("[%1] [TX] %2").arg(timestamp).arg(m_serialPort->formatData(reinterpret_cast<const unsigned char*>(byteArray.constData()), byteArray.size()));
        
        m_textEdit->append(formattedData);
        //m_serialPort->sendSerialData(byteArray);
        m_serialPort->sendSerialData(reinterpret_cast<unsigned char*>(pMess), 19);
    } else {
        qDebug() << "CCdsLicComm instance is null!";
    }

    //qDebug() << "flag:" << static_cast<byte>(flag) << "value:" << static_cast<int>(value);
}

void CdsLicDlg::onFunc2ButtonClicked()
{

}

void CdsLicDlg::onFunc3ButtonClicked()
{

}

void CdsLicDlg::onSendButtonClicked()
{
    if (m_lineEdit && !m_lineEdit->text().isEmpty() && m_serialPort) {
        QString data = m_lineEdit->text();
        
        // Display sent data in main text edit
        QString timestamp = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz");
        
        QByteArray byteArray = m_serialPort->textToBytes(data);
        QString formattedData = QString("[%1] [TX] %2").arg(timestamp).arg(m_serialPort->formatData(reinterpret_cast<const unsigned char*>(byteArray.constData()), byteArray.size()));
        m_textEdit->append(formattedData);
        
        // Send data via serial port
        m_serialPort->sendSerialData(data);
        
        // Clear line edit after sending
        m_lineEdit->clear();
    }
}

void CdsLicDlg::onLineEditReturnPressed()
{
    onSendButtonClicked();
}

void CdsLicDlg::setCdsLicComm(CCdsLicComm* comm)
{
    m_cdsLicComm = comm;
    if (m_cdsLicComm) {
        // Connect CCdsLicComm signals to CdsLicDlg slots
        connect(m_cdsLicComm, &CCdsLicComm::eventOccurred, 
                this, &CdsLicDlg::onCdsLicCommEvent);
        connect(m_cdsLicComm, &CCdsLicComm::errorOccurred, 
                this, [this](CCdsLicComm::Status status, const QString &error) {
                    this->onCdsLicCommError(error);
                });
                
        // Set dialog for serial port
        if (m_serialPort == nullptr) {
            m_serialPort = m_cdsLicComm;
        }
    }
}
