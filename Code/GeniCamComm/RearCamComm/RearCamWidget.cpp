#include "RearCamWidget.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QDockWidget>
#include <QGridLayout>
#include <QMessageBox>
#include <QBitmap>
#include <QEvent>
#include <QCoreApplication>

#include <PvDisplayWnd.h>
#include <PvGenBrowserWnd.h>
#include <PvNetworkAdapter.h>

#include <assert.h>

#include "GeniCamComm/CamDefines.h"
#include "RearCamSource.h"


RearCamWidget::RearCamWidget()
    : mTimer( NULL )
    , mRearCamSource( NULL )
{
    CreateLayout();
}

RearCamWidget::RearCamWidget(const QString &aTitle, RearCamSource *aSource, QWidget *parent, Qt::WindowFlags flags)
    : QWidget( parent, flags )
    , mTimer( NULL )
    , mTitle( aTitle )
    , mRearCamSource( aSource )
{
    CreateLayout();
}

RearCamWidget::~RearCamWidget()
{
    SAFE_DELETE( mTimer );
}

void RearCamWidget::CreateLayout()
{
    mDisplay = new PvDisplayWnd;

    QVBoxLayout *lVLayout = new QVBoxLayout;
    lVLayout->addLayout( CreateDisplayLayout() );
    lVLayout->addLayout( CreateControlsLayout() );

    QGroupBox *lGroupBox = new QGroupBox( mTitle );
    lGroupBox->setLayout( lVLayout );
    lGroupBox->setMinimumWidth( 480 );
    lGroupBox->setMinimumHeight( 420 );

    QHBoxLayout *lLayout = new QHBoxLayout;
    lLayout->addWidget( lGroupBox );

    setLayout( lLayout );

    mTimer = new QTimer( this );
    mTimer->setInterval( 250 );
    connect(mTimer, SIGNAL( timeout() ), this, SLOT( OnTimer() ) );
    mTimer->start();
}

QLayout *RearCamWidget::CreateDisplayLayout()
{
    mStatusLine = new QTextEdit;
    mStatusLine->setReadOnly( true );
    mStatusLine->setEnabled( false );
    mStatusLine->setReadOnly( true );
    mStatusLine->setBackgroundRole( QPalette::Base );
    mStatusLine->setWordWrapMode( QTextOption::NoWrap );
    mStatusLine->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    mStatusLine->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    QFontMetrics lFM( mStatusLine->font() );
    mStatusLine->setMaximumHeight( lFM.height() * 3 + lFM.lineSpacing() );

    QVBoxLayout *lDisplayLayout = new QVBoxLayout;
    lDisplayLayout->addWidget( mDisplay->GetQWidget() );
    lDisplayLayout->addWidget( mStatusLine, Qt::AlignBottom );

    return lDisplayLayout;
}

QLayout *RearCamWidget::CreateControlsLayout()
{
    mModeComboBox = new QComboBox;
    mModeComboBox->setMinimumWidth( 150 );

    QObject::connect(
        mModeComboBox, SIGNAL( activated( int ) ),
        this, SLOT( OnCbnSelchangeMode( int ) ) );

    QVBoxLayout *lModeLayout = new QVBoxLayout;
    lModeLayout->addWidget( new QLabel( "Acquisition mode" ) );
    lModeLayout->addWidget( mModeComboBox );

    mConnectButton = new QPushButton( tr( "Select/&Connect" ) );
    //mConnectButton->setMinimumHeight( 27 );
    mConnectButton->setMinimumHeight( 50 );
    mConnectButton->setMinimumWidth( 100 );
    QObject::connect( mConnectButton, SIGNAL( clicked() ), this, SLOT( OnConnectButton() ) );

    mDisconnectButton = new QPushButton( tr( "&Disconnect" ) );
    //mDisconnectButton->setMinimumHeight( 27 );
    mDisconnectButton->setMinimumHeight( 50 );
    mDisconnectButton->setMinimumWidth( 100 );
    mDisconnectButton->setEnabled(false);
    QObject::connect( mDisconnectButton, SIGNAL( clicked() ), this, SLOT( OnDisconnectButton() ) );

    mPlayButton = new QPushButton( tr( "Play" ) );
    mPlayButton->setMinimumHeight( 50 );
    mPlayButton->setMinimumWidth( 100 );
    mPlayButton->setEnabled( false );

    QPixmap lPlayPixmap(":GeniCamComm/res/play.bmp");
    lPlayPixmap.setMask( lPlayPixmap.createMaskFromColor( QColor( 0xFF, 0xFF, 0xFF ) ) );
    mPlayButton->setIcon(QIcon(lPlayPixmap));

    mStopButton = new QPushButton( tr( "Stop" ) );
    mStopButton->setMinimumHeight( 50 );
    mStopButton->setMinimumWidth( 100 );
    mStopButton->setEnabled( false );

    QPixmap lStopPixmap( ":GeniCamComm/res/stop.bmp" );
    lStopPixmap.setMask( lStopPixmap.createMaskFromColor( QColor( 0xFF, 0xFF, 0xFF ) ) );
    mStopButton->setIcon( QIcon( lStopPixmap ) );

    QObject::connect(mPlayButton, SIGNAL( clicked() ), this, SLOT( OnStart() ) );
    QObject::connect(mStopButton, SIGNAL( clicked() ), this, SLOT( OnStop() ) );

    QHBoxLayout *lLayout = new QHBoxLayout;
    lLayout->addLayout( lModeLayout );
    lLayout->addStretch();
    lLayout->addWidget( mConnectButton );
    lLayout->addWidget( mDisconnectButton );
    lLayout->addStretch();
    lLayout->addWidget( mPlayButton );
    lLayout->addWidget( mStopButton );

    return lLayout;
}

void RearCamWidget::EnableInterface()
{
    bool lModeEnabled = false;
    bool lPlayEnabled = false;
    bool lStopEnabled = false;

    // Read current acquisition state
    bool lLocked = mRearCamSource->GetAcquisitionState() == PvAcquisitionStateLocked;
    bool lOpened = mRearCamSource->IsOpened();

    // Mode and play are enabled if not locked
    lModeEnabled = !lLocked && lOpened;
    lPlayEnabled = !lLocked && lOpened;

    // Stop is enabled only if locked
    lStopEnabled = lLocked;

    mModeComboBox->setEnabled( lModeEnabled );
    mPlayButton->setEnabled( lPlayEnabled );
    mStopButton->setEnabled( lStopEnabled );

    mDisconnectButton->setEnabled(lPlayEnabled);
}

void RearCamWidget::OnConnectButton()
{
    // create a device finder wnd and open the select device dialog
    PvDeviceFinderWnd lWnd;
    lWnd.SetTitle( "GEV Device Selection" );

    // Show device finder
    if ( ( !lWnd.ShowModal().IsOK() ) ||
         ( lWnd.GetSelected() == NULL ) )
    {
        return;
    }

    // Show hourglass
    QCursor lOldCursor = cursor();
    setCursor( Qt::WaitCursor );
    QCoreApplication::processEvents();

    // Connect to device
    const PvDeviceInfoGEV *lDIG = dynamic_cast<const PvDeviceInfoGEV *>( lWnd.GetSelected() );
    if ( lDIG != NULL )
    {
        Connect( lDIG );
    }
    else
    {
        QMessageBox::warning( this, "Error", "This application does not support USB3 Vision devices." );
    }

    // Restore cursor
    setCursor( lOldCursor );
}

void RearCamWidget::OnDisconnectButton()
{
    // Show hourglass
    QCursor lOldCursor = cursor();
    setCursor( Qt::WaitCursor );
    QCoreApplication::processEvents();

    // Disconnect from device
    Disconnect();

    // Restore cursor
    setCursor( lOldCursor );
}

void RearCamWidget::Connect(const PvDeviceInfoGEV *aDeviceInfo)
{
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
    RearCamSource *lSources = mRearCamSource;

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
        //{
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
        //}
    }
    else
    {
        // No source selector, if transmitter assume 1 on channel 0
        lResult = mRearCamSource->Open( &mDevice, aDeviceInfo->GetIPAddress(), lNA->GetIPAddress( 0 ), 0 );
        if ( !lResult.IsOK() )
        {
            PvMessageBox( this, lResult );
            return;
        }
    }

    // Sync the UI with our new status
    EnableInterface();
}

void RearCamWidget::Disconnect()
{
    // Close all configuration child windows
    //CloseGenWindow( mDeviceBrowser );
    //CloseGenWindow( StreamBrowser );

    if ( mRearCamSource->IsOpened() )
    {
        PvResult lResult = mRearCamSource->Close();
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

    GetDisplay()->Clear();

    // Sync the UI with our new status
    EnableInterface();
}


///
/// \brief Play button handler
///

void RearCamWidget::OnStart()
{
    PvResult lResult = mRearCamSource->StartAcquisition();
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}


///
/// \brief Stop button handler
///

void RearCamWidget::OnStop()
{
    PvResult lResult = mRearCamSource->StopAcquisition();
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}


///
/// \brief Timer event handler
///

void RearCamWidget::OnTimer()
{
    QString lInfo;
    mRearCamSource->GetStreamInfo( lInfo );

    mStatusLine->setText( lInfo );
}


///
/// \brief Mode combobox changed handler
///

void RearCamWidget::OnCbnSelchangeMode( int aIndex )
{
    if ( aIndex < 0 )
    {
        return;
    }

    QString lMode = mModeComboBox->itemText( aIndex );
    PvResult lResult = mRearCamSource->SetAcquisitionMode( lMode );
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}


///
/// \brief Generic event handler
///

bool RearCamWidget::event( QEvent *aEvent )
{
    switch ( static_cast<int>( aEvent->type() ) )
    {
        case WM_UPDATEACQCONTROLS:
            EnableInterface();
            break;

        case WM_UPDATEACQMODE:
            UpdateAcquisitionMode();
            break;

        case WM_UPDATEACQMODES:
            UpdateAcquisitionModes();
            break;

        default:
            break;
    }

    return QWidget::event( aEvent );
}


///
/// \brief Updates the content of the acquisition mode combo box
///

void RearCamWidget::UpdateAcquisitionModes()
{
    // Get current mode
    QString lCurrentMode;
    mRearCamSource->GetAcquisitionMode( true, lCurrentMode );

    // Get all possible modes
    std::vector<QString> lModes;
    mRearCamSource->GetAcquisitionModes( lModes );

    // Fill combo box
    mModeComboBox->clear();
    std::vector<QString>::iterator lIt = lModes.begin();
    while ( lIt != lModes.end() )
    {
        mModeComboBox->addItem( *lIt );
        if ( *lIt == lCurrentMode )
        {
            mModeComboBox->setCurrentIndex( mModeComboBox->count() - 1 );
        }

        lIt++;
    }
}


///
/// \brief Updates the acquisition mode from the device value
///

void RearCamWidget::UpdateAcquisitionMode()
{
    QString lMode;
    mRearCamSource->GetAcquisitionMode( false, lMode );
    if ( lMode.length() > 0 )
    {
        for ( int i = 0; i < mModeComboBox->count(); i++ )
        {
            if ( lMode == mModeComboBox->itemText( i ) )
            {
                mModeComboBox->setCurrentIndex( i );
                return;
            }
        }
    }
}
