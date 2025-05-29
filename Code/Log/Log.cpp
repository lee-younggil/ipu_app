#include "Log.h"
#include "Util/Util.h"
#include "Code/Mediator.h"

#include <QDebug>
#include <stdio.h>
#include <iostream>
#include <QTime>


#define USE_LOG_COMPRESS


CLog::CLog(CMediator *hMediator, const QString &strLogSource)
    : CIFService(hMediator, strLogSource)
    , m_strLogPath("")
    , m_pqueBypass(NULL)
    , m_bBypassEnable(true)
{
    m_bLogThreadState = false;
}


CLog::~CLog(void)
{
    ServiceStop();
}

void CLog::initialization(void)
{
    // �ٸ� Ŭ���� initialization ���¿��� �α׿� �����ϱ� ���Ͽ� initialization �Լ������� ServiceStart ȣ��
    ServiceStart();
}

bool CLog::ServiceStart(void)
{
    int arg = 0, nstate = 0, t_id = 0;
    void * t_ret = nullptr;

    if( m_bLogThreadState == false ){

        nstate = pthread_create(&m_pThread, NULL, LogThread, (void *)&arg);

        if(nstate != 0){
            //error log
            qDebug() << "pthread_create error nstate : " << nstate;
        }

        //nstate = pthread_join(t_id, &t_ret);
        if(nstate != 0){
            //error log
            qDebug() << "log pthread_join error nstate : " << nstate << " t_id : " << t_id;
        }
    }

    return true;
}

bool CLog::ServiceStop(void)
{
    if( m_pThread == 0 )
    {
        return true;
    }

    if( m_pThread != 0 ){
        m_bLogThreadState = false;

        pthread_detach(m_pThread);
        //pause();

        m_pThread = 0;
    }

    return true;
}

void CLog::SetLogPath(const QString &strLogPath)
{
    m_strLogPath = strLogPath;
}


void CLog::Log(const QString &strLog, const void *pPacket /*= NULL*/, unsigned int uiPacketLength /*= 0 */)
{
    Log( m_strLogSource, strLog, pPacket, uiPacketLength );
}

void CLog::CompressLog()
{
#ifdef USE_LOG_COMPRESS
    //AfxBeginThread(CompressLogThread, this, THREAD_PRIORITY_NORMAL);
#endif
}

void * CLog::CompressLogThread( void * pParam )
{
    CLog *pThis = (CLog *)pParam;

    // 1000 MB �뷮 Ȯ�� �� ����.
    while( pThis->DeleteOldLogZipFile( 1000 ) ) ;
    while( pThis->DeleteOldLogFolder( 1000 ) ) ;

 #ifdef TODO
    //CTime tCompressDate = CTime::GetCurrentTime() - CTimeSpan( 1, 0, 0, 0 );
    QTime n(1, 0, 0);
    QTime tCompressDate = QTime::currentTime();

    QString strCompressFolder;
    int nLogPathLen = strlen(pThis->m_strLogPath.toStdString().c_str());
    if( pThis->m_strLogPath[nLogPathLen-1] == ('/') )
    {
        strCompressFolder.asprintf("%s%04d%02d%02d"), pThis->m_strLogPath, tCompressDate.GetYear(), tCompressDate.GetMonth(), tCompressDate.GetDay() );
    }
    else
    {
        strCompressFolder.asprintf( ("%s\\%04d%02d%02d"), pThis->m_strLogPath, tCompressDate.GetYear(), tCompressDate.GetMonth(), tCompressDate.GetDay() );
    }

    CUtil::CompressFolder( strCompressFolder );

    return 0;
#endif
}

bool CLog::IsDiskFull( const int iDiskFullLimit )
{
    char path[MAX_PATH];

    int nLen = strlen(m_strLogPath.toStdString().c_str());
    strncpy( path, m_strLogPath.toStdString().c_str(), nLen );
    //quint64 avail;
#if 0
    ULARGE_INTEGER avail;
    GetDiskFreeSpaceEx(path, &avail, NULL, NULL);

    // " >> 10 " KByte �� ���� ��ȯ
    // " >> 20 " MByte �� ���� ��ȯ
    // " >> 30 " GByte �� ���� ��ȯ
    if( ( avail.QuadPart >> 20 ) < iDiskFullLimit )	// ���� ��ũũ�Ⱑ iDiskFullLimit MB ����
        return true;
#endif
    return false;
}

bool CLog::DeleteOldLogFolder( const int iDiskFullLimit )
{
    bool bState = false;

    char fname[MAX_PATH] = {0x00, };
    QString path = m_strLogPath;

#ifdef TODO
    HANDLE hSrch;
    WIN32_FIND_DATA wfd;

    QTime t = QTime::currentTime();
    QTime tOld = t;

    int nPathLen = strlen(path.toStdString().c_str());
    strncpy(fname, path.toStdString().c_str(), nPathLen);

    // c:\log\ ������ �������� '\' ���ڰ� �����ٸ� ����
    if( fname[strlen(fname)-1] == ('/') )
        fname[strlen(fname)-1] = 0;

    int nLen = strlen("/*.*");
    strncat(fname, ("/*.*"), nLen);
    hSrch=FindFirstFile( fname, &wfd );
    if( hSrch == 0 )
    {
        //TRACE( ("ERROR : FindFirstFile") );
        return bState;
    }

    char bResult = true;
    while(bResult)		// do-while ����
    {
        // ���丮 Ȯ��
        if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // "." �Ǵ� ".." ���� �ƴҰ��
            if(wfd.cFileName[0] != '.')
            {
                // YYYYMMDD ������ ���� �˻�
                if( strlen( wfd.cFileName ) != 8 )
                {
                    bResult=FindNextFile(hSrch, &wfd);
                    continue;
                }
                int j;
                for(j=0;j<8;j++)
                {
                    if ( wfd.cFileName[j] < '0' || wfd.cFileName[j] > '9')
                        break;
                }
                if(j < 8)			// YYYYMMDD ������ ������ �ƴ϶�� ����������
                {
                    bResult=FindNextFile(hSrch, &wfd);
                    continue;
                }

                char yy[5], mm[3], dd[3];
                memcpy(yy, wfd.cFileName, 4);		yy[4] = 0;
                memcpy(mm, wfd.cFileName+4, 2);		mm[2] = 0;
                memcpy(dd, wfd.cFileName+6, 2);		dd[2] = 0;

                // MM : 1��~12��, DD : 1��~31�� �� �ƴ϶�� ����������
                if( ( atoi(yy) < 1970 ) || ( atoi(yy) > 3000 ) ||
                    ( atoi(mm) < 1 )    || ( atoi(mm) > 12 )   ||
                    ( atoi(dd) < 1 )    || ( atoi(dd) > 31 )   )
                {
                    bResult=FindNextFile(hSrch, &wfd);
                    continue;
                }

                CTime t2( atoi(yy), atoi(mm), atoi(dd), 0, 0, 0);

                CTimeSpan ts = t - t2;

                int nPathLen = strlen(path.toStdString().c_str());
                if( path[nPathLen-1] == ('/') )
                {
                    wsprintf(fname, ("%s%s"), path, wfd.cFileName);
                }
                else
                {
                    wsprintf(fname, ("%s\\%s"), path, wfd.cFileName);
                }

                if( ts.GetTotalHours() >= ( MAX_LOG_FOLDER_LIVE * 24 ) )
                {
                    CUtil::deleteDir(fname);
                    bResult=FindNextFile(hSrch, &wfd);
                    continue;
                }
                if( tOld > t2 )	tOld = t2;
            }
        }
        bResult=FindNextFile(hSrch, &wfd);
    }
    FindClose(hSrch);

    if( IsDiskFull( iDiskFullLimit ) )
    {
        int nLen = strlen(path.toStdString().c_str());
        if( path[nLen-1] == ('\\') )
        {
            wsprintf(fname, ("%s%04d%02d%02d"), path, tOld.GetYear(), tOld.GetMonth(), tOld.GetDay() );
        }
        else
        {
            wsprintf(fname, ("%s\\%04d%02d%02d"), path, tOld.GetYear(), tOld.GetMonth(), tOld.GetDay() );
        }
        CUtil::deleteDir( fname );
        bState = true;
    }
#endif
    return bState;

}

bool CLog::DeleteOldLogZipFile( const int iDiskFullLimit )
{
    bool bState = false;

    char fname[MAX_PATH];
    QString path = m_strLogPath;

#ifdef TODO
    HANDLE hSrch;
    WIN32_FIND_DATA wfd;

    CTime t = CTime::GetCurrentTime();
    CTime tOld = t;

    int nLen = strlen(path.toStdString().c_str());
    strncpy( fname, path.toStdString().c_str(), nLen );

    // c:\log\ ������ �������� '\' ���ڰ� �����ٸ� ����
    if( fname[strlen(fname)-1] == ('/') )
        fname[strlen(fname)-1] = 0;

    int nLen = strlen(fname);
    strncat( fname, ("/*.zip"), nLen );
    hSrch=FindFirstFile( fname, &wfd );
    if( hSrch == 0 )
    {
        //TRACE( ("ERROR : FindFirstFile") );
        return bState;
    }

    char bResult = true;
    while(bResult)		// do-while ����
    {
        // YYYYMMDD ������ ���� �˻�
        if( strlen( wfd.cFileName ) != 12 )
        {
            bResult=FindNextFile(hSrch, &wfd);
            continue;
        }
        int j;
        for(j=0;j<8;j++)
        {
            if ( wfd.cFileName[j] < '0' || wfd.cFileName[j] > '9')
                break;
        }
        if(j < 8)			// YYYYMMDD ������ ������ �ƴ϶�� ����������
        {
            bResult=FindNextFile(hSrch, &wfd);
            continue;
        }

        char yy[5], mm[3], dd[3];
        memcpy(yy, wfd.cFileName, 4);		yy[4] = 0;
        memcpy(mm, wfd.cFileName+4, 2);		mm[2] = 0;
        memcpy(dd, wfd.cFileName+6, 2);		dd[2] = 0;

        // MM : 1��~12��, DD : 1��~31�� �� �ƴ϶�� ����������
        if( ( atoi(yy) < 1970 ) || ( atoi(yy) > 3000 ) ||
            ( atoi(mm) < 1 )    || ( atoi(mm) > 12 )   ||
            ( atoi(dd) < 1 )    || ( atoi(dd) > 31 )   )
        {
            bResult=FindNextFile(hSrch, &wfd);
            continue;
        }

        CTime t2( atoi(yy), atoi(mm), atoi(dd), 0, 0, 0);

        CTimeSpan ts = t - t2;

        int nPathLen = strlen(path.toStdString().c_str());
        if( path[nPathLen-1] == ('/') )
        {
            wsprintf(fname, ("%s%s"), path, wfd.cFileName);
        }
        else
        {
            wsprintf(fname, ("%s/%s"), path, wfd.cFileName);
        }

        if( ts.GetTotalHours() >= ( MAX_LOG_FOLDER_LIVE * 24 ) )
        {
            DeleteFile(fname);
            bResult=FindNextFile(hSrch, &wfd);
            continue;
        }
        if( tOld > t2 )	tOld = t2;

        bResult=FindNextFile(hSrch, &wfd);
    }
    FindClose(hSrch);


    if( IsDiskFull( iDiskFullLimit ) )
    {
        int nPathLen = strlen(path.toStdString().c_str());
        if( path[nPathLen-1] == ('/') )
        {
            wsprintf(fname, ("%s%04d%02d%02d.zip"), path, tOld.GetYear(), tOld.GetMonth(), tOld.GetDay() );
        }
        else
        {
            wsprintf(fname, ("%s\\%04d%02d%02d.zip"), path, tOld.GetYear(), tOld.GetMonth(), tOld.GetDay() );
        }
        DeleteFile( fname );
        bState = true;
    }
#endif
    return bState;
}



void CLog::Log(const QString &strLogSource, const QString &strLog, const void *pPacket /*= NULL*/, unsigned int uiPacketLength /*= 0 */)
{
    if( m_bLogThreadState == false )
    {
        return;
    }

    if( m_queLog.size() > 2000 )	// �α� ���ϸ� ���δ�.
    {
        return;
    }

    LOG_DATA *pLogData = new LOG_DATA;
    pLogData->src = strLogSource;

    if( strLogSource == "" ||
        strLogSource == "IRTU_CONTROL" ||
        strLogSource == "RFTU_CONTROL" )
    {
        pLogData->strLog = strLog;
    }
    else
    {
#ifdef TODO
        SYSTEMTIME st;
        GetLocalTime(&st);
        pLogData->strLog.asprintf( ("%02d-%02d-%02d\t%02d:%02d:%02d\t%03d\t"),
            st.wYear%100, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        pLogData->strLog += strLog;
        if( ( pPacket != NULL ) && ( uiPacketLength > 0 ) )
        {
            for( unsigned int i = 0 ; i < uiPacketLength ; i++ )
            {
                pLogData->strLog.AppendFormat( ("\t%02X"), ((UCHAR *)pPacket)[i] );
                if( i > 1024 )
                {
                    pLogData->strLog.AppendFormat( ("\t...") );
                    break;
                }
            }
        }
        pLogData->strLog.AppendFormat( ("\r\n") );
#endif
    }

    pthread_mutex_init(&m_mutex_file, NULL);

    pthread_mutex_lock(&m_mutex_file);
    m_queLog.push(pLogData);
    pthread_mutex_unlock(&m_mutex_file);
}

void * CLog::LogThread(void * pParam)
{
    CLog* pThis = reinterpret_cast<CLog*>( pParam );
    Q_ASSERT( pThis != NULL );

#ifdef TODO
    bool bGo = false;
    LOG_DATA *pBuf;
    QString dir;
    QString strLog, strTemp;
    char logFileName[MAX_PATH];
    HANDLE hFile;
    bool bSuccessWriteFile = false;
    bool bCompress = false;

    pthread_mutex_init(&pThis->m_mutex_file, NULL);

    while(pThis->m_bLogThreadState == true)
    {
        if ( bGo == false )	sleep(LOG_THREAD_SLEEP_TIME);
        else				sleep(1000);

        bGo = false;

        pthread_mutex_lock(&pThis->m_mutex_file);

        if( pThis->m_queLog.empty() == false )
        {
            bGo = true;
            pBuf = pThis->m_queLog.front();
        }
        pthread_mutex_unlock(&pThis->m_mutex_file);

        if( bGo == true )
        {
            CTime t = CTime::GetCurrentTime();

#ifdef USE_LOG_COMPRESS
            // �α� ����
            if( t.GetHour() == 2 )
            {
                if( bCompress == false )
                {
                    bCompress = true;
                    pThis->CompressLog();
                }
            }
            else
            {
                if( bCompress == true )
                {
                    bCompress = false;
                }
            }
#endif

            int iDiskFullLimit = 500;
            // ��ũ �뷮�� ���ٸ� ������ ���� ����
            if( pThis->IsDiskFull( iDiskFullLimit ) == true )
            {
                pThis->Log( "Disk Full Del Start" );
                pThis->DeleteOldLogFolder( iDiskFullLimit );
#ifdef USE_LOG_COMPRESS
                pThis->DeleteOldLogZipFile( iDiskFullLimit );
#endif
                pThis->Log( "Disk Full Del End" );
            }

            int iTimeDir  = t.GetYear() * 10000 + t.GetMonth() * 100 + t.GetDay();
            dir.asprintf( "%s%08d", pThis->m_strLogPath.toStdString().c_str(), iTimeDir );

            // ���� ���� ������ ������ ���� ����
            if( CUtil::IsDirectoryExist(dir) == false )
            {
                CUtil::CreateDir(dir);
            }

            // ���ϸ� ����
            sprintf_s( logFileName, MAX_PATH, ("%s\\%04d%02d%02d-%02d.%s"), dir, t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), pBuf->src );

            // �α� ����
#if 1
            do
            {
                bSuccessWriteFile = false;

                hFile = CreateFile(	logFileName,
                    GENERIC_WRITE,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);

                if( hFile == 0 )
                {
                    strLog.asprintf( ("1- ERROR : CreateFile()") );
                    //TRACE( ("ERROR : CreateFile()") );
                    break;
                }

                if( SetFilePointer( hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER )
                {
                    strLog.asprintf( ("2- ERROR : SetFilePointer()") );
                    //TRACE( ("ERROR : SetFilePointer()") );

                    if( CloseHandle(hFile) == FALSE )
                    {
                        //strTemp.asprintf( ("3- ERROR : %d"), GetLastError() );
                        //TRACE( ("ERROR : %d"), GetLastError() );
                    }
                    break;
                }

                DWORD WriteByte;
                if( WriteFile(hFile, pBuf->strLog, pBuf->strLog.GetLength(), &WriteByte, NULL) == FALSE )
                {
                    strLog.asprintf( ("4 - ERROR : WriteFile()") );
                    //TRACE( ("ERROR : WriteFile()") );

                    if( CloseHandle(hFile) == false )
                    {
                        //strLog.AppendFormat( ("5 - ERROR : %d"), GetLastError() );
                        //TRACE( ("ERROR : %d"), GetLastError() );
                    }
                    break;
                }

                if( CloseHandle(hFile) == false )
                {
                    //strLog.asprintf( ("6 - ERROR : %d"), GetLastError() );
                    //TRACE( ("ERROR : %d"), GetLastError() );
                    break;
                }

                bSuccessWriteFile = true;
            } while ( 0 );
#endif

            if( bSuccessWriteFile || ( pThis->m_queLog.size() > LOG_QUE_MAX_SIZE ) )
            {
                pthread_mutex_lock(&pThis->m_mutex_file);
                pThis->m_queLog.pop();
                pthread_mutex_unlock(&pThis->m_mutex_file);

                pThis->m_hMediator->SetLogData( pBuf );
            }
        }
    }

    pthread_mutex_lock(&pThis->m_mutex_file);
    while( pThis->m_queLog.empty() == false)
    {
        pBuf = pThis->m_queLog.front();
        pThis->m_queLog.pop();

        delete pBuf;
    }
    pthread_mutex_unlock(&pThis->m_mutex_file);
#endif

    //return 0;
}
