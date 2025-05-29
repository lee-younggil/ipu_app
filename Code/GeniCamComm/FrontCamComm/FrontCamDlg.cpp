#include "FrontCamDlg.h"
#include "FrontCamSource.h"
#include "FrontCamWidget.h"
#include "GeniCamComm/CamDefines.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDockWidget>
#include <QMenuBar>
#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QBitmap>
#include <QPainter>
#include <QShortcut>
#include <QSettings>
#include <QCoreApplication>

#include <PvDeviceFinderWnd.h>
#include <PvDisplayWnd.h>
#include <PvGenBrowserWnd.h>
#include <PvNetworkAdapter.h>

#include <algorithm>



#define LEFT_WIDTH ( 400 )
#define WM_IMAGEDISPLAYED ( WM_USER + 0x4431 )
#define BUFFER_COUNT (16) //Ref : PvStreamSample



FrontCamDlg::FrontCamDlg(QWidget *parent) : QWidget(parent)
  , mFrontCamSource(NULL)
  , mCommunicationBrowser( NULL )
  , mDeviceBrowser(NULL)
  , StreamBrowser(NULL)
{
    StreamBrowser = new PvGenBrowserWnd;
    mDeviceBrowser = new PvGenBrowserWnd;
    mCommunicationBrowser = new PvGenBrowserWnd;

    CreateLayout();
}

FrontCamDlg::~FrontCamDlg()
{
    SAFE_DELETE( StreamBrowser );
    SAFE_DELETE( mDeviceBrowser );
    SAFE_DELETE( mCommunicationBrowser );

    SAFE_DELETE( mFrontCamSource );

    // UI controls are supposed to be released by the UI
}

void FrontCamDlg::CreateLayout()
{
    //mFrontCamSource = new FrontCamSource( 0 );
    //mFrontCamWidget = new FrontCamWidget( "Source", mFrontCamSource );
    mFrontCamSource = new FrontCamSource();
    mFrontCamWidget = new FrontCamWidget("Front Camera", mFrontCamSource );
    mFrontCamSource->SetWidget( mFrontCamWidget );

    QHBoxLayout *lLayout = new QHBoxLayout;
    lLayout->addWidget( mFrontCamWidget );

    QFrame *lMainBox = new QFrame;
    lMainBox->setLayout( lLayout );

    setLayout(lLayout);

    EnableInterface();
}

void FrontCamDlg::OnLinkDisconnected(PvDevice *aDevice)
{
    Q_UNUSED(aDevice)
}


void FrontCamDlg::ShowGenWindow(PvGenBrowserWnd *aWnd, PvGenParameterArray *aArray, const QString &aTitle)
{
    if ( aWnd->GetQWidget()->isVisible() )
    {
        // If already open, just toggle to closed...
        CloseGenWindow( aWnd );
        return;
    }

    // Create, assign parameters, set title and show modeless
    aWnd->SetTitle( aTitle.toUtf8().constData() );

    aWnd->ShowModeless( this );
    aWnd->SetGenParameterArray( aArray );
}

void FrontCamDlg::CloseGenWindow(PvGenBrowserWnd *aWnd)
{
    if ( aWnd->GetQWidget()->isVisible() )
    {
        aWnd->Close();
    }
}

void FrontCamDlg::EnableInterface()
{
    mFrontCamWidget->EnableInterface();
}
