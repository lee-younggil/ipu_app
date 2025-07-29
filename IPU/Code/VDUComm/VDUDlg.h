#ifndef VDUDLG_H
#define VDUDLG_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QTcpServer>      // Include for server
#include <QTcpSocket>      // Include for client socket
#include <QNetworkInterface> // Include for IP address info

class CVDUComm;  // Forward declaration
class QTcpServer; // Forward declaration
class QTcpSocket; // Forward declaration

class CVDUDlg : public QWidget
{
    Q_OBJECT
public:
    explicit CVDUDlg(QWidget *parent = nullptr);
    ~CVDUDlg();

    void setVduComm(CVDUComm* comm) { m_vduComm = comm; }

public slots: // Add public slots section
    void logPacket(const QByteArray& data, const QString& prefix);

protected:
    void CreateLayout();

private slots:
    void Func1();
    void TestTriggerNum();
    void LineCmd();

private:
    QTextEdit* m_textEdit;      // Original log display
    QTextEdit* m_textEdit2;     // Original log display 2
    QLineEdit* m_lineEdit;
    QPushButton* m_func1Button;
    QPushButton* m_func2Button;
    QPushButton* m_sendButton;
    CVDUComm* m_vduComm;        // Original comm object

};

#endif // VDUDLG_H
