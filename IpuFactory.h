#ifndef CIPUFACTORY_H
#define CIPUFACTORY_H
#pragma once


#include <QWidget>
#include "IpuMainDlg.h"




//class CIpuFactory
//{
//    Q_OBJECT
//public:
//    CIpuFactory();
//};


class CIpuFactory
{
public:

    CIpuFactory();
    virtual ~CIpuFactory();

    CIpuMainDlg *CreateDlg();
    //PvGenBrowserWnd *CreateDeviceBrowser();

};

#endif // CIPUFACTORY_H
