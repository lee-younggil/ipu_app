#ifndef VVPDLG_H
#define VVPDLG_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

class CVVPComm;  // Forward declaration

class CVVPDlg : public QWidget
{
    Q_OBJECT
public:
    explicit CVVPDlg(QWidget *parent = nullptr);
    ~CVVPDlg();

    void setVvpComm(CVVPComm* comm) { m_vvpComm = comm; }

public slots: // Add public slots section
    void logPacket(const QByteArray& data, const QString& prefix);

protected:
    void CreateLayout();

private slots:
    void Func1();
    void SendAck();
    void LineCmd();


private:
    QTextEdit* m_textEdit;
    QTextEdit* m_textEdit2;
    QLineEdit* m_lineEdit;
    QPushButton* m_func1Button;
    QPushButton* m_func2Button;
    QPushButton* m_sendButton;
    CVVPComm* m_vvpComm;

};

#endif // VVPDLG_H
