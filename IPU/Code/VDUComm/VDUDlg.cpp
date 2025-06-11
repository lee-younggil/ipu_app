#include "VDUDlg.h"
#include "VDUComm.h"
#include "Code/Mediator.h"
#include <QDebug>
#include <QScrollBar>
#include <QByteArray> // Include for QByteArray
#include <QString>    // Include for QString

CVDUDlg::CVDUDlg(QWidget *parent) : QWidget(parent)
{
    CVDUComm* comm = CMediator::getInstance()->getVduComm();
    setVduComm(comm); // Assuming setVduComm stores it in m_vduComm
    CreateLayout();

    if (m_vduComm) {
        // Use Qt::QueuedConnection for thread safety as signals might come from different threads
        connect(m_vduComm, &CVDUComm::packetSent, this, [this](const QByteArray& data){
            logPacket(data, "T");
        }, Qt::QueuedConnection);
        connect(m_vduComm, &CVDUComm::packetRecv, this, [this](const QByteArray& data){
            logPacket(data, "R");
        }, Qt::QueuedConnection);

    } else {
        qDebug() << "CVDUDlg: VDUComm instance is null, cannot connect signals.";
    }
}

CVDUDlg::~CVDUDlg()
{

}

void CVDUDlg::CreateLayout()
{
    // Create tab page for VDU communication
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
    
    m_lineEdit = new QLineEdit();
    if (!m_lineEdit) {
        qDebug() << "Failed to create line editor";
        return;
    }

    m_func1Button = new QPushButton(tr("Func1"));
    if (!m_func1Button) {
        qDebug() << "Failed to create button";
        return;
    }

    m_func2Button = new QPushButton(tr("Test Trigger"));
    if (!m_func2Button) {
        qDebug() << "Failed to create button";
        return;
    }

    m_sendButton = new QPushButton(tr("Line Cmd"));
    if (!m_sendButton) {
        qDebug() << "Failed to create button";
        return;
    }
    
    // Add widgets to layouts
    layoutHSendCtrl->addWidget(m_func1Button, 1);
    layoutHSendCtrl->addWidget(m_func2Button, 1);
    layoutHSendCtrl->addWidget(m_lineEdit, 1);
    layoutHSendCtrl->addWidget(m_sendButton, 1);
    layout->addLayout(layoutHSendCtrl);

    layouth->addWidget(m_textEdit, 3);
    layouth->addWidget(m_textEdit2, 1);
    layout->addLayout(layouth);

    // Connect signals and slots
    connect(m_func1Button, &QPushButton::clicked, this, &CVDUDlg::Func1);
    connect(m_func2Button, &QPushButton::clicked, this, &CVDUDlg::TestTriggerNum);
    connect(m_sendButton, &QPushButton::clicked, this, &CVDUDlg::LineCmd);

    // Set initial state
    m_func1Button->setEnabled(true);
    m_func2Button->setEnabled(true);
    m_sendButton->setEnabled(true);
    m_lineEdit->setEnabled(true);
}

void CVDUDlg::Func1()
{
    if (m_vduComm) {
        //m_vduComm->Func1();
    }
}

void CVDUDlg::TestTriggerNum()
{
    if (m_vduComm) {
        m_vduComm->TestTriggerNum(m_vduComm->GetSeqNo(0x01));
    }
}

void CVDUDlg::LineCmd()
{
    if (m_vduComm) {
        //m_vduComm->LineCmd();
    }
}

// Implementation of the logPacket slot
void CVDUDlg::logPacket(const QByteArray& data, const QString& prefix)
{
    //qDebug() << ">>> CVDUDlg::logPacket start" << prefix << " <<<";
    if (!m_textEdit) {
        qDebug() << ">>> m_textEdit이 nullptr입니다! <<<";
        return;
    }

    QString hexString = data.toHex(' ').toUpper();
    
    // Add timestamp for clarity
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logEntry = QString("%1 %2[%3] %4").arg(timestamp).arg(prefix).arg(hexString.mid(3 * 10, 5).replace(" ", "")).arg(hexString); 
    //qDebug() << "CVDUDlg::logPacket" << logEntry; // Debug output for the log entry


    // Append text (QTextEdit::append is thread-safe)
    m_textEdit->append(logEntry);

    // Optional: Auto-scroll to the bottom
    QScrollBar *sb = m_textEdit->verticalScrollBar();
    if (sb) {
        // Scroll only if the scrollbar is already at the bottom
        bool scroll = (sb->value() >= (sb->maximum() - 4));
        if (scroll) {
            sb->setValue(sb->maximum());
        }
    } else {
        qDebug() << ">>> 스크롤바가 nullptr입니다! <<<";
    }
    
}
