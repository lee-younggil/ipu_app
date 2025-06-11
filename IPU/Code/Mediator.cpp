#include "Mediator.h"
#include "IPUDefine.h"
#include <QObject>
#include "IFService.h"
#include "Service.h"

CMediator* CMediator::m_pInstance = nullptr;

//CMediator::CMediator() : CService()
//{

//}

CMediator::CMediator(CMediator *hMediator, const QString &strLogSource)
    : CIFService(hMediator, strLogSource)
    , m_CdsLicComm(hMediator, "LIC")
    , m_strDrive(STR_DRIVER_PATH)
    , m_strAppFolder(STR_APP_DIR)
    , m_strBinFolder(STR_BIN_DIR)
    , m_strLogFolder(STR_LOG_DIR)
    , m_pConfig(nullptr)
{
    m_pConfig = new CIPUConfig();
    // 持失 貢 去系 授辞 掻推!!!
    m_hLog = new CLog(this, "LOG");
    m_hLog->SetLogPath(m_strDrive+m_strAppFolder+"/"+m_strLogFolder+"/");
    m_hLog->initialization();
    m_hLog->ServiceStart();
}

CMediator::~CMediator()
{
    m_handleManager.DeleteHandle();

    if (m_pConfig)
    {
        delete m_pConfig;
        m_pConfig = nullptr;
    }

    m_hLog->ServiceStop();
    delete m_hLog;

    pthread_mutex_init(&m_mutex_log, NULL);
    pthread_mutex_lock(&m_mutex_log);
    while( m_queLogData.empty() == false )
    {
        LOG_DATA *pLogData = m_queLogData.front();
        m_queLogData.pop();
        delete pLogData;
    }
    pthread_mutex_unlock(&m_mutex_log);
}

void CMediator::Log(const QString &strLog, const void *pPacket, unsigned int uiPacketLength)
{

}

void CMediator::initialization()
{
    // CCdsLicComm 초기화
    m_CdsLicComm.initialization();
    //m_CdsLicComm.SetPort(0);
    //m_CdsLicComm.SetBaudRate(115200);
    m_CdsLicComm.SetPort(m_pConfig->GetPortLic().toInt());  // IPUConfig에서 설정한 포트 사용
    m_CdsLicComm.SetBaudRate(m_pConfig->GetPortBaud().toInt());  // IPUConfig에서 설정한 baudrate 사용

    // CVVPComm 초기화
    m_VvpComm.initialization();
    m_VvpComm.SetDeviceIP(m_pConfig->GetVVPDeviceIP());
    m_VvpComm.SetPort(m_pConfig->GetVVPDevicePort().toInt());

    // CVduComm 초기화
    m_VduComm.initialization();
    m_VduComm.SetDeviceIP(m_pConfig->GetVduDeviceIP());
    m_VduComm.SetPort(m_pConfig->GetVduDevicePort().toInt());

}

bool CMediator::ServiceStart()
{
    // CCdsLicComm 서비스 시작
    if (!m_CdsLicComm.ServiceStart())
    {
        Log("CdsLicComm", "Failed to start CdsLicComm service");
        return false;
    }

    if (!m_VvpComm.ServiceStart())
    {
        Log("VvpComm", "Failed to start VvpComm service");
        return false;
    }

    if (!m_VduComm.ServiceStart())
    {
        Log("VduComm", "Failed to start VduComm service");
        return false;
    }
    
    return true;
}

bool CMediator::ServiceStop()
{
    // CCdsLicComm 서비스 중지
    m_CdsLicComm.ServiceStop();
    m_CdsLicComm.Close();
    
    return true;
}

VEHICLE_INFO* CMediator::GetVehicleInfo(unsigned int uiTriggerNo)
{
    VEHICLE_INFO *ptVehicleInfo = nullptr;
    
    QMutexLocker locker(&m_cslistVehicleInfo);
    for (auto it = m_listVehicleInfo.begin(); it != m_listVehicleInfo.end(); ++it)
    {
        if ((*it)->pktEntryTriggerNum.uiTriggerNo == uiTriggerNo)
        {
            ptVehicleInfo = *it;
            break;
        }
    }
    
    return ptVehicleInfo;
}

void CMediator::PushBackVehicle( VEHICLE_INFO *ptVehicleInfo )
{
	if( ptVehicleInfo == NULL ) return;

	QMutexLocker locker(&m_cslistVehicleInfo);
	m_listVehicleInfo.push_back( ptVehicleInfo );
}

void CMediator::NotifyTrigger(unsigned int uiTriggerNo)
{
    // Notify the trigger number to the appropriate handler
}

void CMediator::SendImage(unsigned int uiTriggerNo)
{
    qDebug() << "SendImage called with TriggerNo:" << uiTriggerNo;
    VEHICLE_INFO* ptVehicleInfo = GetVehicleInfo(uiTriggerNo);
    PACKET_IMAGE tImage;
    memset(&tImage, 0, sizeof(PACKET_IMAGE));
    tImage.byteVehiclePosition = ptVehicleInfo->pktEntryTriggerNum.byteVehiclePosition;
    tImage.dateData = ptVehicleInfo->pktEntryTriggerNum.dateData;
    tImage.byteVDSType = ptVehicleInfo->pktEntryTriggerNum.byteVDSType;
    tImage.uiTriggerNo = ptVehicleInfo->pktEntryTriggerNum.uiTriggerNo;
    //차량코드? 우선 byteVehiclePosition 의 마지막 값으로 사용하자.
    tImage.taVehicelID[0].byteLaneCode = ptVehicleInfo->pktEntryTriggerNum.byteVehiclePosition % 10;
    tImage.taVehicelID[0].baVehicleID[0] = 0x81; // 190하3288
    tImage.taVehicelID[0].baVehicleID[1] = 0x90;
    tImage.taVehicelID[0].baVehicleID[2] = 0x13;
    tImage.taVehicelID[0].baVehicleID[3] = 0x32;
    tImage.taVehicelID[0].baVehicleID[4] = 0x88;

    tImage.taVehicelID[1].byteLaneCode = 0xff;
    // mcu 전송
    m_VvpComm.SendPacket(DEVICE_CODE_IPU, MsgID_Image, (unsigned char *)&tImage, sizeof(PACKET_IMAGE));
}

unsigned int CMediator::RegisterWatchdogID(const QString &strThreadName, unsigned long ulRefreshTime_ms)
{

}
