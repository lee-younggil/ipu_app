#ifndef LOG_H
#define LOG_H

#pragma once

#include <QMainWindow>
#include <QObject>
#include <queue>
#include <QString>

#include "unistd.h"
#include "pthread.h"

#include "Public/IFService.h"
#include "IPUDefine.h"


#define LOG_THREAD_SLEEP_TIME 100

// �ִ� �α� ��� ����
#define MAX_LOG_FOLDER_LIVE		( 60 )

// �α� ���� ���н� �α� ť�� LOG_QUE_MAX_SIZE ���� ���� ����Ǿ� ������ �� ��(������ �ϴ� ����)�� �����Ѵ�.
#define LOG_QUE_MAX_SIZE		( 1000 )



class LOG_DATA
{
public:
    LOG_DATA() {};
    LOG_DATA(QString src, QString strLog) { this->src = src; this->strLog = strLog; };
public:
    QString src;
    QString strLog;
};

class CLog : public CIFService
{
public:
    CLog(CMediator *hMediator, const QString &strLogSource);
    virtual ~CLog(void);

protected:
    virtual void Log( const QString &strLog, const void *pPacket = NULL, unsigned int uiPacketLength = 0 );

public:
    virtual void initialization(void);
    virtual bool ServiceStart(void);
    virtual bool ServiceStop(void);

public:
    void SetLogPath(const QString &strLogPath);
    void Log( const QString &strLogSource, const QString &strLog, const void *pPacket = NULL, unsigned int uiPacketLength = 0 );

private:
#if 0
    QString m_strLogPath;

    CWinThread *m_hLogThread; //TO DO
    bool m_bLogThreadState;

    std::queue<LOG_DATA *> m_queLog;
    CCriticalSection m_csFile; //TO DO

    std::queue<LOG_DATA *> *	m_pqueBypass;
    CCriticalSection *			m_pcsBypass; //TO DO
    bool						m_bBypassEnable;

    static unsigned int LogThread(LPVOID pParam);
#else
    QString m_strLogPath;
    bool m_bLogThreadState;

    std::queue<LOG_DATA *> m_queLog;
    pthread_mutex_t m_mutex_file;

    std::queue<LOG_DATA *> *m_pqueBypass;
    pthread_mutex_t m_mutex_bypass;

    bool m_bBypassEnable;

    pthread_t m_pThread;

    static void * LogThread(void * pvParam);
#endif
    void CompressLog();
    static void * CompressLogThread( void * pParam ); //TO DO

    bool IsDiskFull( const int iDiskFullLimit );
    bool DeleteOldLogFolder( const int iDiskFullLimit );
    bool DeleteOldLogZipFile( const int iDiskFullLimit );
};

#endif // LOG_H
