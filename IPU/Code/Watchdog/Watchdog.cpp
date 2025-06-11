#include "Watchdog.h"
#include <QDebug>

CWatchdog::CWatchdogData::CWatchdogData()
    : strThreadName( "NULL" )
    , ulRefreshTime_ms( 1000 )
    , bEnableWatchdog( false )
    , bRefreshState( false )
    , ulLastRefreshTime_ms( 0 )
{
}

CWatchdog::CWatchdogData::~CWatchdogData()
{
}

CWatchdog::CWatchdog()
    : m_uiWatchdogTimeOut_sec( 60 )
    , m_uiWatchdogSize( 0 )
    , m_pThread(0)
{

}

CWatchdog::~CWatchdog( void )
{
    ServiceStop();
}

void CWatchdog::initialization( void )
{
    //Log("Watchdog initialization");
}

bool CWatchdog::ServiceStart( void )
{
    int arg = 0;
    int nstate = 0;
    int t_id = 0;
    void * t_ret = nullptr;

    nstate = pthread_create(&m_pThread, NULL, &WatchdogThread, (void *)&arg);

    if(nstate != 0){
        //error log
        qDebug() << "pthread_create error nstate : " << nstate;
    }

    nstate = pthread_join(t_id, &t_ret);
    if(nstate != 0){
        //error log
        qDebug() << "watchdog pthread_join error nstate : " << nstate << " t_id : " << t_id;
    }

    return true;
}

bool CWatchdog::ServiceStop( void )
{
    if( m_pThread == 0 ){
        return true;
    }

    //Log("Watchdog ServiceStop");

    for( unsigned int i = 0 ; i < m_uiWatchdogSize ; i++ ){
        if( m_aWatchdog[i].bEnableWatchdog == true ){
            StopWatchdogID(i);
        }
    }

    if( m_pThread != 0 ){
        m_bThreadState = false;

        pthread_detach(m_pThread);
        pause(); //TODO

        m_pThread = 0;
    }

    return true;
}

void* CWatchdog::WatchdogThread( void* pvParam )
{
    CWatchdog *pThis = (CWatchdog *)pvParam;
    while(pThis->m_bThreadState == true){
        sleep(1000);
        pThis->CheckWatchdog();
    }

    //return;
}

void CWatchdog::CheckWatchdog()
{
    QString strLog = ("Timeover : ");

    bool bResetWatchdog = true;
    bool bExistTimeout = false;

    unsigned long ulCurrentTime_ms = GetMiliSecTimer();

    for( unsigned int i = 0 ; i < m_uiWatchdogSize ; i++ )
    {
        if( m_aWatchdog[i].bEnableWatchdog == true )
        {
            if( m_aWatchdog[i].bRefreshState == false )
            {
                bResetWatchdog = false;

                unsigned long ulTime = ulCurrentTime_ms - m_aWatchdog[i].ulLastRefreshTime_ms;
                if( ulTime > m_aWatchdog[i].ulRefreshTime_ms )
                {
                    bExistTimeout = true;
                    //strLog.append( ("%s[%u]"), m_aWatchdog[i].strThreadName, ulTime ) ;
                }
            }
        }
    }

    if( bResetWatchdog == true )
    {
        ResetWatchdog();
        for( unsigned int i = 0 ; i < m_uiWatchdogSize ; i++ )
        {
            m_aWatchdog[i].bRefreshState = false;
        }
    }
    if( bExistTimeout == true )
    {
        //Log( strLog );
    }
}

unsigned int CWatchdog::GetWatchdogID()
{
    unsigned int uiWatchdogID = 0;

    pthread_mutex_init(&m_mutex_lock, NULL);

    pthread_mutex_lock(&m_mutex_lock);
    if( m_uiWatchdogSize < WATCHDOG_SIZE )
    {
        uiWatchdogID = m_uiWatchdogSize;
        m_uiWatchdogSize++;
    }
    pthread_mutex_unlock(&m_mutex_lock);

    return uiWatchdogID;
}

unsigned int CWatchdog::RegisterWatchdogID( const QString &strThreadName, unsigned long ulRefreshTime_ms )
{
    unsigned int uiWatchdogID = GetWatchdogID();

    m_aWatchdog[uiWatchdogID].strThreadName = strThreadName;
    m_aWatchdog[uiWatchdogID].ulRefreshTime_ms = ulRefreshTime_ms * 2;
    m_aWatchdog[uiWatchdogID].bEnableWatchdog = true;
    m_aWatchdog[uiWatchdogID].bRefreshState = true;
    m_aWatchdog[uiWatchdogID].ulLastRefreshTime_ms = GetMiliSecTimer();

    QString strLog;
    strLog.asprintf( ("Register : %s[%u] - %d"), strThreadName.toStdString().c_str(), ulRefreshTime_ms, uiWatchdogID );
    //Log( strLog ); //TOD

    return uiWatchdogID;
}

void CWatchdog::StopWatchdogID( unsigned int uiWatchdogID )
{
    if( m_uiWatchdogSize < uiWatchdogID )
    {
        QString strLog;
        strLog.asprintf( ("ERROR : StopWatchdogID : Not Register Watchdog ID : %d"), uiWatchdogID );
        //Log( strLog ); //TOD
    }
    else
    {
        m_aWatchdog[uiWatchdogID].bEnableWatchdog = false;
    }
}

void CWatchdog::RefreshWatchdogID( unsigned int uiWatchdogID )
{
    if( m_uiWatchdogSize < uiWatchdogID )
    {
        QString strLog;
        strLog.asprintf( ("ERROR : RefreshWatchdogID : Not Register Watchdog ID : %d"), uiWatchdogID );
        //Log( strLog ); //TOD
    }
    else
    {
        m_aWatchdog[uiWatchdogID].bEnableWatchdog = true;
        m_aWatchdog[uiWatchdogID].bRefreshState = true;
        m_aWatchdog[uiWatchdogID].ulLastRefreshTime_ms = GetMiliSecTimer();
    }
}

void CWatchdog::Log(const QString &strLog, const void *pPacket, unsigned int uiPacketLength )
{

}
