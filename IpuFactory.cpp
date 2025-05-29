#include "IpuFactory.h"

#include "Log/Log.h"
#include "Mediator.h"

///
/// \brief Constructor
///

CIpuFactory::CIpuFactory()
{
}


///
/// \brief Destructor
///

CIpuFactory::~CIpuFactory()
{
}


///
/// \brief Factory create method
///

CIpuMainDlg *CIpuFactory::CreateDlg()
{
    // In this factory, we simply create the default CIpuMainDlg
    return new CIpuMainDlg;
}


///
/// \brief Creates a device GenICam browser
///

//PvGenBrowserWnd *CIpuFactory::CreateDeviceBrowser()
//{
//    return new PvGenBrowserWnd;
//}
