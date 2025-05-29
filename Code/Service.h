#pragma once

#ifndef SERVICE_H
#define SERVICE_H

#include <QString>
#include "Public/DataTypeDefine.h"

class CMediator;

class CService
{
public:
    CService()
    {

    }
    virtual ~CService(void)
    {

    }
//protected:
//    virtual void Log( const QString &strLog, const void *pPacket = NULL, unsigned int uiPacketLength = 0 ) = 0;

public:
    virtual void initialization(void)=0;	//
    virtual bool ServiceStart(void)=0;		//
    virtual bool ServiceStop(void)=0;		//
};

#endif // SERVICE_H
