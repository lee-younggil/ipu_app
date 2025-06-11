#ifndef IFWATCHDOG_H
#define IFWATCHDOG_H

#pragma once

#include <QObject>
#include <vector>
#include "Public/IFService.h"

#include "unistd.h"
#include "pthread.h"


//class CWatchdog : public CService
class CWatchdog
{
#define WATCHDOG_SIZE 10
public:
    CWatchdog(void);
    virtual ~CWatchdog(void);

protected:
    virtual void Log( const QString &strLog, const void *pPacket = NULL, unsigned int uiPacketLength = 0 );
    virtual unsigned long GetMiliSecTimer() = 0;
    virtual void ResetWatchdog() = 0;

public:
    virtual void initialization(void);
    virtual bool ServiceStart(void);
    virtual bool ServiceStop(void);

    unsigned int RegisterWatchdogID(const QString &strThreadName, unsigned long ulRefreshTime_ms );

    void RefreshWatchdogID(unsigned int uiWatchdogID );

    void StopWatchdogID( unsigned int uiWatchdogID );

protected:
    unsigned int m_uiWatchdogTimeOut_sec;

private:
    class CWatchdogData
    {
    public:
        CWatchdogData();
        virtual ~CWatchdogData();

        QString strThreadName;
        unsigned long ulRefreshTime_ms;
        bool bEnableWatchdog;
        bool bRefreshState;
        unsigned long ulLastRefreshTime_ms;
    };

    bool m_bThreadState;
    unsigned int m_uiWatchdogSize;

    pthread_t m_pThread;
    pthread_mutex_t m_mutex_lock;

    CWatchdogData m_aWatchdog[WATCHDOG_SIZE];
    static void* WatchdogThread(void * pvParam);

    void CheckWatchdog();
    unsigned int GetWatchdogID();
};

#endif // IFWATCHDOG_H
