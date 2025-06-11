#include "RearCamDlg.h"
#include "RearCamSource.h"
#include "RearCamWidget.h"
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


RearCamDlg::RearCamDlg(QWidget *parent) : QWidget(parent)
  , mRearCamSource(NULL)
  , mCommunicationBrowser( NULL )
  , mDeviceBrowser(NULL)
  , StreamBrowser(NULL)
{
    StreamBrowser = new PvGenBrowserWnd;
    mDeviceBrowser = new PvGenBrowserWnd;
    mCommunicationBrowser = new PvGenBrowserWnd;

    CreateLayout();
}

RearCamDlg::~RearCamDlg()
{
    Disconnect();

    SAFE_DELETE( StreamBrowser );
    SAFE_DELETE( mDeviceBrowser );
    SAFE_DELETE( mCommunicationBrowser );

    SAFE_DELETE( mRearCamSource );

    // UI controls are supposed to be released by the UI
}

void RearCamDlg::CreateLayout()
{
    //mFrontCamSource = new FrontCamSource( 0 );
    //mFrontCamWidget = new FrontCamWidget( "Source", mFrontCamSource );
    mRearCamSource = new RearCamSource();
    mRearCamWidget = new RearCamWidget("Rear Camera", mRearCamSource );
    mRearCamSource->SetWidget( mRearCamWidget );

    QHBoxLayout *lLayout = new QHBoxLayout;
    lLayout->addWidget( mRearCamWidget );


    QFrame *lMainBox = new QFrame;
    lMainBox->setLayout( lLayout );

    setLayout(lLayout);

    EnableInterface();
}

void RearCamDlg::Connect(const PvDeviceInfoGEV *aDeviceInfo)
{
    Q_UNUSED(aDeviceInfo)
#if 0
    // Just in case we came here still connected...
    Disconnect();

    // Connect to device using device info
    PvResult lResult = mDevice.Connect( aDeviceInfo );
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
        return;
    }

    mDevice.RegisterEventSink( this );

    //FrontCamSource *lSources[ 2 ] = { mFrontCamSource, mSource2 };
    FrontCamSource *lSources = mFrontCamSource;

    // Open channels
    PvGenEnum *lSourceSelector = mDevice.GetParameters()->GetEnum( "SourceSelector" );
    PvGenInteger *lSourceChannel = mDevice.GetParameters()->GetInteger( "SourceStreamChannel" );
    if ( lSourceChannel == NULL )
    {
        lSourceChannel = mDevice.GetParameters()->GetInteger( "SourceIDValue" );
    }
    const PvNetworkAdapter *lNA = dynamic_cast<const PvNetworkAdapter *>( aDeviceInfo->GetInterface() );
    if ( lSourceSelector != NULL )
    {
        // Go through source selector enum entries
        int64_t lCount = 0;
        lSourceSelector->GetEntriesCount( lCount );
        lCount = std::min( lCount, static_cast<int64_t>( 2 ) );
        //for ( int64_t i = 0; i < lCount; i++ )
        {
            // Get enum entry from its index
            const PvGenEnumEntry *lEE = NULL;
            lSourceSelector->GetEntryByIndex( 0, &lEE );

            // Get enum entry name
            PvString lStr;
            lEE->GetName( lStr );

            // Set source selector
            lSourceSelector->SetValue( lStr );

            // With source selected, get its stream channel
            int64_t lChannel = 0;
            lSourceChannel->GetValue( lChannel );

            // Open source
            lResult = lSources->Open( &mDevice, aDeviceInfo->GetIPAddress(), lNA->GetIPAddress( 0 ),
                    static_cast<uint16_t>( lChannel ) );
            if ( !lResult.IsOK() )
            {
                PvMessageBox( this, lResult );
                return;
            }
        }
    }
    else
    {
        // No source selector, if transmitter assume 1 on channel 0
        lResult = mFrontCamSource->Open( &mDevice, aDeviceInfo->GetIPAddress(), lNA->GetIPAddress( 0 ), 0 );
        if ( !lResult.IsOK() )
        {
            PvMessageBox( this, lResult );
            return;
        }
    }

    // Sync the UI with our new status
    EnableInterface();
#endif
}

void RearCamDlg::Disconnect()
{
#if 0
    // Close all configuration child windows
    CloseGenWindow( mDeviceBrowser );
    CloseGenWindow( StreamBrowser );

    if ( mFrontCamSource->IsOpened() )
    {
        PvResult lResult = mFrontCamSource->Close();
        if ( !lResult.IsOK() )
        {
            PvMessageBox( this, lResult );
        }
    }

    if ( mDevice.IsConnected() )
    {
        mDevice.UnregisterEventSink( this );

        PvResult lResult = mDevice.Disconnect();
        if ( !lResult.IsOK() )
        {
            PvMessageBox( this, lResult );
        }
    }

    mFrontCamWidget->GetDisplay()->Clear();

    // Sync the UI with our new status
    EnableInterface();
#endif
}

void RearCamDlg::OnDeviceDisconnect()
{
    Disconnect();
}

void RearCamDlg::OnLinkDisconnected(PvDevice *aDevice)
{
    Q_UNUSED(aDevice)
    emit DeviceDisconnected();
}


void RearCamDlg::ShowGenWindow(PvGenBrowserWnd *aWnd, PvGenParameterArray *aArray, const QString &aTitle)
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

void RearCamDlg::CloseGenWindow(PvGenBrowserWnd *aWnd)
{
    if ( aWnd->GetQWidget()->isVisible() )
    {
        aWnd->Close();
    }
}



void RearCamDlg::EnableInterface()
{
    mRearCamWidget->EnableInterface();
}
