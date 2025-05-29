#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>

#include "HandleManager.h"
#include <queue>
#include <QMutex>

#include "CdsLicComm/CdsLicComm.h"
#include "Config/IPUConfig.h"
#include "VVPComm/VVPComm.h"
#include "VDUComm/VDUComm.h"
#include "Watchdog/Watchdog.h"
#include "Log/Log.h"



class CLog;
class LOG_DATA;

class CCdsLicComm;
class CGeniCamComm;
class CICamServerComm;
class cVVPComm;
class CVDUComm;
class CWatchdog;

//class CMediator : public CService
class CMediator : public CIFService
{
private:
    //CMediator();
    CMediator(CMediator *hMediator, const QString &strLogSource);
    virtual ~CMediator(void);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 가상함수
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    void Log( const QString &strLog, const void *pPacket = NULL, unsigned int uiPacketLength = 0 );

public:
    virtual void initialization(void);
    virtual bool ServiceStart(void);
    virtual bool ServiceStop(void);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public 함수
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    static CMediator* getInstance() {
        if (m_pInstance == nullptr) {
            m_pInstance = new CMediator(nullptr, "MEDIATOR");
            //m_pInstance = new CMediator();
        }
        return m_pInstance;
    }
    void Log( const QString &strLogSource, const QString &strLog, const void *pPacket = NULL, unsigned int uiPacketLength = 0 );

    unsigned int RegisterWatchdogID( const QString &strThreadName, unsigned long ulRefreshTime_ms );
    void RefreshWatchdogID( unsigned int uiWatchdogID );
    void StopWatchdogID( unsigned int uiWatchdogID );
    void ResetWatchdog( unsigned int uiWatchdogTimeOut_min );

    void NotifyScannerConnection(bool bConnected);
    void NotifyDstConnection(bool bConnected);

    void NotifyCarDetect( bool bHigh );

    void SetLogData(LOG_DATA *pLogData);
    
    LOG_DATA *GetLogData();

    CCdsLicComm* getCdsLicComm() { return &m_CdsLicComm; }
    CVVPComm* getVvpComm() { return &m_VvpComm; }
    CVDUComm* getVduComm() { return &m_VduComm; }
    void PushBackVehicle( VEHICLE_INFO *ptVehicleInfo );
    void NotifyTrigger(unsigned int uiTriggerNo);
    void SendImage(unsigned int uiTriggerNo);
    VEHICLE_INFO* GetVehicleInfo(unsigned int uiTriggerNo);

    QMutex m_cslistVehicleInfo;
    std::list <VEHICLE_INFO *> m_listVehicleInfo;
    
private:
    CCdsLicComm m_CdsLicComm;
    CVVPComm m_VvpComm;
    CVDUComm m_VduComm;
    QString	m_strDrive;
    QString	m_strAppFolder;
    QString	m_strBinFolder;
    QString	m_strLogFolder;

    CLog *m_hLog;
    CWatchdog *m_hWatchdog;

    CHandleManager m_handleManager;

    pthread_mutex_t m_mutex_log, m_mutex_mdt;
    std::queue<LOG_DATA *> m_queLogData; 

    CIPUConfig* m_pConfig;

    static CMediator* m_pInstance;
};

#endif // MEDIATOR_H
