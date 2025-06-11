#pragma once

#ifndef IFSERVICE_H
#define IFSERVICE_H

#include <QString>
#include "Public/DataTypeDefine.h"

class CMediator;

class CIFService
{
public:
    CIFService(CMediator *hMediator, const QString &strLogSource)
    {
        m_hMediator = hMediator;
        m_strLogSource = strLogSource;

    }
    virtual ~CIFService(void)
    {

    }

protected:
    virtual void Log( const QString &strLog, const void *pPacket = NULL, unsigned int uiPacketLength = 0 ) = 0;

public:
    virtual void initialization(void)=0;	// �ʱ�ȭ
    virtual bool ServiceStart(void)=0;		// ���� ����
    virtual bool ServiceStop(void)=0;		// ���� ����

protected:
    CMediator *m_hMediator;
    QString m_strLogSource;
};
#endif // IFSERVICE_H
