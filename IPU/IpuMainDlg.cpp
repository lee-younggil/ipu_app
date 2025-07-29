#include "IpuMainDlg.h"

#include <QGroupBox>
#include <QMessageBox>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>

#include <stdio.h>
#include <assert.h>
#include <limits>


#include "Mediator.h"
#include "Config/IPUConfig.h"
#include "Log/Log.h"

#include "IpuInfoDlg.h"
#include "CdsLicComm/CdsLicDlg.h"
#include "GeniCamComm/FrontCamComm/FrontCamDlg.h"
#include "GeniCamComm/RearCamComm/RearCamDlg.h"
#include "VDUComm/VDUDlg.h"
#include "VVPComm/VVPDlg.h"


//CMediator g_Mediator; //ACU 구조로 변경해야함.


#define DEFAULT_TAB_INDEX    0 //1
#define DEFAULT_TAB_WIDTH    1600
#define DEFAULT_TAB_HEIGHT   1400


//CIpuMainDlg::CIpuMainDlg(QWidget *aParent, const QString &aAppName)
//CIpuMainDlg::CIpuMainDlg(QWidget *parent) : QDialog( parent )
CIpuMainDlg::CIpuMainDlg( QWidget *parent, Qt::WindowFlags flags ) : QMainWindow( parent, flags )
{
    CreateLayout();

    //g_Mediator.initialization();
    //g_Mediator.ServiceStart();
}

CIpuMainDlg::~CIpuMainDlg()
{

}

void CIpuMainDlg::closeEvent(QCloseEvent *event)
{
    //qDebug() << __func__;
#if 1
    event->accept();
#else
    QMessageBox msgBox;
    msgBox.setWindowTitle("위반차량 촬영장치");
    msgBox.setText("프로그램을 종료할까요?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
    msgBox.setIcon(msgBox.Warning);
    msgBox.setDefaultButton(QMessageBox::No);
    int nRet = msgBox.exec();

    if(nRet == QMessageBox::No){
        event->ignore();
    }
    else{
        //g_Mediator.ServiceStop();

        event->accept();
    }
#endif
}

void CIpuMainDlg::CreateLayout()
{
    qDebug() << __func__;

    // Mediator 생성
    
    // 서비스 초기화 및 시작
    //mediator->initialization();
    
	//MainWindow resize
    resize(DEFAULT_TAB_WIDTH+10, DEFAULT_TAB_HEIGHT+10);

    m_pTabWidget = new QTabWidget(this);


    //Create tab
    m_pTabWidget->addTab(new CIpuInfoDlg(), tr("IPU Info"));
    m_pTabWidget->addTab(new FrontCamDlg(), tr("Front Cam"));
    m_pTabWidget->addTab(new RearCamDlg(),  tr("Rear Cam"));
    m_pTabWidget->addTab(new CdsLicDlg(),   tr("Lens Control"));
    m_pTabWidget->addTab(new CVVPDlg(),     "MCU");
    // VDU 탭은 차로 분류가 multi일 때만 추가
    if (CMediator::getInstance()->getConfig()->getLaneClassification() == "multi") {
        m_pTabWidget->addTab(new CVDUDlg(),     "VDU");
    }
    m_pTabWidget->addTab(CMediator::getInstance()->getConfig(),  tr("Net Config"));

    CMediator::getInstance()->ServiceStart();

    /*CdsLicDlg* cdsLicDlg = new CdsLicDlg();
    if (!cdsLicDlg) return;
    // 통신 설정
    if (CCdsLicComm* comm = mediator->getCdsLicComm()) {
        cdsLicDlg->setSerialPort(comm);
        comm->setDialog(cdsLicDlg);
    }
    mediator->ServiceStart();
    */
    //m_pTabWidget->addTab(cdsLicDlg, "Serial");
    //m_pTabWidget->addTab(new CIpuInfoDlg(), tr("Info3"));

    

    //m_pTabWidget->setFixedSize(DEFAULT_TAB_WIDTH, DEFAULT_TAB_HEIGHT);
    m_pTabWidget->setMinimumWidth(DEFAULT_TAB_WIDTH);
    m_pTabWidget->setMinimumHeight(DEFAULT_TAB_HEIGHT);
    m_pTabWidget->setTabShape(QTabWidget::Rounded);

    //set tab index after change
    //QObject::connect( m_pTabWidget, SIGNAL(currentChanged(int)), this, SLOT(SetCurrentTab(int) ) );


    m_pTabWidget->setCurrentIndex(DEFAULT_TAB_INDEX); //Tab index change


    // Layout
    m_lTabVLayout = new QVBoxLayout(this);
    m_lTabVLayout->addWidget(m_pTabWidget);
    setLayout(m_lTabVLayout);

    setWindowTitle( tr( "위반차량 촬영장치" ) );

    // 간단한 소켓 서버 추가
    /*QTcpServer* server = new QTcpServer(this);
    quint16 port = 12345; // 사용할 포트 번호

    if (!server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server failed to start on port" << port << ":" << server->errorString();
    } else {
        qDebug() << "Server started on port" << port;
    }

    connect(server, &QTcpServer::newConnection, this, [server]() {
        QTcpSocket* clientSocket = server->nextPendingConnection();
        qDebug() << "New client connected from" << clientSocket->peerAddress().toString()
                 << ":" << clientSocket->peerPort();

        QObject::connect(clientSocket, &QTcpSocket::readyRead, clientSocket, [clientSocket]() {
            QByteArray data = clientSocket->readAll();
            qDebug() << "Received data:" << data;

            // Echo the data back to the client
            clientSocket->write("Server received: " + data);
        });

        QObject::connect(clientSocket, &QTcpSocket::disconnected, clientSocket, [clientSocket]() {
            qDebug() << "Client disconnected.";
            clientSocket->deleteLater();
        });
    });*/
}

void CIpuMainDlg::SetCurrentTab(int nidx)
{
    //qDebug() << __func__ << "- nidx: " << nidx;
    m_nCurrentTabIndex = nidx;
}

int CIpuMainDlg::GetCurrentTabIndex()
{
    return m_nCurrentTabIndex;
};
