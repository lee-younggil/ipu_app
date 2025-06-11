#include "FrontCamWidget.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QGridLayout>
#include <QMessageBox>
#include <QBitmap>
#include <QEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QStackedWidget>

#include <PvDisplayWnd.h>
#include <PvGenBrowserWnd.h>
#include <PvNetworkAdapter.h>

#include <assert.h>

#include "GeniCamComm/CamDefines.h"
#include "FrontCamSource.h"

FrontCamWidget::FrontCamWidget()
    : mTimer( NULL )
    , mFrontCamSource( NULL )
{
    mbSaveImage = false;

    CreateLayout();
}

FrontCamWidget::FrontCamWidget( const QString &aTitle, FrontCamSource *aSource, QWidget *parent, Qt::WindowFlags flags )
    : QWidget( parent, flags )
    , mTimer( NULL )
    , mTitle( aTitle )
    , mFrontCamSource( aSource )
{
    mbSaveImage = false;

    CreateLayout();
}

FrontCamWidget::~FrontCamWidget()
{
    SAFE_DELETE( mTimer );
}

void FrontCamWidget::UpdateCaptureImage(cv::Mat image, std::string filepath)
{
    //qDebug() << "[UpdateCaptureImage] - ";

    if(image.data){
        mCaptureLabel->clear();
        QPixmap capture_img;
        capture_img.load( QString( filepath.c_str() ) );
        mCaptureLabel->setPixmap(capture_img.scaled(800, 600, Qt::KeepAspectRatio));

        qDebug() << "[UpdateCaptureImage] - cols : " << image.cols << ", rows : " << image.rows;
        //QImage img(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
        //mCaptureLabel->setPixmap(QPixmap::fromImage(img));

#if 1 // 번호판 추출 이미지 표시 일단 보류
        mPlateLabel->clear();
        QPixmap plate_img;
        plate_img.load( QString( filepath.c_str() ) );
        mPlateLabel->setPixmap( plate_img.scaled(240, 80, Qt::KeepAspectRatio));
#endif
    }
    else{
        mCaptureLabel->setText("No Image");
        qDebug() << "[UpdateCaptureImage] - No Image";
    }
}

void FrontCamWidget::CreateLayout()
{
    mDisplay = new PvDisplayWnd;

    QVBoxLayout *lVLayout = new QVBoxLayout();
    lVLayout->addLayout( CreateDisplayLayout() );
    lVLayout->addLayout( CreateCaptureLayout() );
    lVLayout->addLayout( CreateControlsLayout() );


    QHBoxLayout *lHLayout = new QHBoxLayout();
    lHLayout->addLayout(CreateLogLayout());

    QGroupBox *lGroupBox = new QGroupBox( mTitle );
    lGroupBox->setLayout( lVLayout );
    lGroupBox->setMaximumWidth( 800 );
    lGroupBox->setMaximumHeight( 1400 );

    QHBoxLayout *lLayout = new QHBoxLayout();
    lLayout->addWidget( lGroupBox );
    lLayout->addLayout(lHLayout);

    setLayout( lLayout );

    mTimer = new QTimer( this );
    mTimer->setInterval( 250 );
    connect(mTimer, SIGNAL( timeout() ), this, SLOT( OnTimer() ) );
    mTimer->start();
}

QLayout *FrontCamWidget::CreateDisplayLayout()
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
    mStatusLine->setVisible(false); //To be invisible

    QVBoxLayout *lDisplayLayout = new QVBoxLayout;
    lDisplayLayout->addWidget( mDisplay->GetQWidget() );
    //lDisplayLayout->addWidget( mStatusLine, Qt::AlignBottom );

    return lDisplayLayout;
}

QLayout *FrontCamWidget::CreateCaptureLayout()
{

    mCaptureLabel = new QLabel;
    mCaptureLabel->setPixmap(QPixmap());
    mCaptureLabel->setMinimumHeight(600);

#if 1
    mPlateLabel = new QLabel;
    mPlateLabel->setPixmap(QPixmap());
    mPlateLabel->setMinimumHeight(80);
#endif

    QStackedWidget *stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(mCaptureLabel);
    stackedWidget->addWidget(mPlateLabel);
    stackedWidget->setCurrentIndex(0);


    QVBoxLayout *lCaptureLayout = new QVBoxLayout;
    //lCaptureLayout->addWidget(mCaptureLabel);
    //lCaptureLayout->addWidget(mPlateLabel);
    lCaptureLayout->addWidget(stackedWidget);

    return lCaptureLayout;
}

QLayout *FrontCamWidget::CreateControlsLayout()
{
    mModeComboBox = new QComboBox;
    mModeComboBox->setMinimumWidth( 150 );
    QObject::connect( mModeComboBox, SIGNAL( activated( int ) ), this, SLOT( OnCbnSelchangeMode( int ) ) );

    QVBoxLayout *lModeLayout = new QVBoxLayout;
    lModeLayout->addWidget( new QLabel( "Acquisition mode" ) );
    lModeLayout->addWidget( mModeComboBox );

    //Save test checkbox
    mSaveImgCheckBox = new QCheckBox;
    QObject::connect( mSaveImgCheckBox, SIGNAL( clicked() ), this, SLOT( OnSaveImagecheck() ) );


    mConnectButton = new QPushButton( tr( "Select/&Connect" ) );
    mConnectButton->setMinimumHeight( 50 );
    mConnectButton->setMinimumWidth( 100 );
    QObject::connect( mConnectButton, SIGNAL( clicked() ), this, SLOT( OnConnectButton() ) );

    mDisconnectButton = new QPushButton( tr( "&Disconnect" ) );
    mDisconnectButton->setMinimumHeight( 50 );
    mDisconnectButton->setMinimumWidth( 100 );
    mDisconnectButton->setEnabled(false);
    QObject::connect( mDisconnectButton, SIGNAL( clicked() ), this, SLOT( OnDisconnectButton() ) );

    mPlayButton = new QPushButton( tr( "Play" ) );
    mPlayButton->setMinimumHeight( 50 );
    mPlayButton->setMinimumWidth( 80 );
    mPlayButton->setEnabled( false );

    QPixmap lPlayPixmap(":GeniCamComm/res/play.bmp");
    lPlayPixmap.setMask( lPlayPixmap.createMaskFromColor( QColor( 0xFF, 0xFF, 0xFF ) ) );
    mPlayButton->setIcon(QIcon(lPlayPixmap));

    mStopButton = new QPushButton( tr( "Stop" ) );
    mStopButton->setMinimumHeight( 50 );
    mStopButton->setMinimumWidth( 80 );
    mStopButton->setEnabled( false );

    QPixmap lStopPixmap( ":GeniCamComm/res/stop.bmp" );
    lStopPixmap.setMask( lStopPixmap.createMaskFromColor( QColor( 0xFF, 0xFF, 0xFF ) ) );
    mStopButton->setIcon( QIcon( lStopPixmap ) );

    QObject::connect( mPlayButton, SIGNAL( clicked() ), this, SLOT( OnStart() ) );
    QObject::connect( mStopButton, SIGNAL( clicked() ), this, SLOT( OnStop() ) );

    QHBoxLayout *lLayout = new QHBoxLayout;
    lLayout->addLayout( lModeLayout );
    lLayout->addStretch();

    lLayout->addWidget(mSaveImgCheckBox);
    lLayout->addStretch();

    lLayout->addWidget( mConnectButton );
    lLayout->addWidget( mDisconnectButton );
    lLayout->addStretch();
    lLayout->addWidget( mPlayButton );
    lLayout->addWidget( mStopButton );

    return lLayout;
}

QLayout *FrontCamWidget::CreateLogLayout()
{
    mLogText = new QTextEdit;

    //mLogDisplay->setReadOnly( true );
    mLogText->setEnabled( true );
    mLogText->setReadOnly( true );
    mLogText->setBackgroundRole( QPalette::Base );
    mLogText->setWordWrapMode( QTextOption::NoWrap );
    mLogText->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    mLogText->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

    QFontMetrics lFM( mLogText->font() );
    //mLogDisplay->setMaximumHeight( lFM.height() * 3 + lFM.lineSpacing() );
    mLogText->setMinimumWidth(400);
    mLogText->setMinimumHeight(600);

    QVBoxLayout *lLogLayout = new QVBoxLayout;
    lLogLayout->addWidget( new QLabel( "Log" ) );
    lLogLayout->addWidget( mLogText, Qt::AlignRight);

    return lLogLayout;
}

void FrontCamWidget::EnableInterface()
{
    bool lModeEnabled = false;
    bool lPlayEnabled = false;
    bool lStopEnabled = false;

    // Read current acquisition state
    bool lLocked = mFrontCamSource->GetAcquisitionState() == PvAcquisitionStateLocked;
    bool lOpened = mFrontCamSource->IsOpened();

    // Mode and play are enabled if not locked
    lModeEnabled = !lLocked && lOpened;
    lPlayEnabled = !lLocked && lOpened;

    // Stop is enabled only if locked
    lStopEnabled = lLocked;

    mModeComboBox->setEnabled( lModeEnabled );
    mPlayButton->setEnabled( lPlayEnabled );
    mStopButton->setEnabled( lStopEnabled );

    mDisconnectButton->setEnabled(lPlayEnabled);
    mConnectButton->setEnabled(!lOpened);
}

void FrontCamWidget::Log_FrontCam(QString log)
{
    mLogText->setText(log);
}

void FrontCamWidget::OnConnectButton()
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

void FrontCamWidget::OnDisconnectButton()
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

void FrontCamWidget::Connect(const PvDeviceInfoGEV *aDeviceInfo)
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
        qDebug() << "[FrontCamWidget::Connect] - lSourceSelector is not NULL";

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
        qDebug() << "[FrontCamWidget::Connect] - lSourceSelector is NULL";

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
}

void FrontCamWidget::Disconnect()
{
    // Close all configuration child windows
    //CloseGenWindow( mDeviceBrowser );
    //CloseGenWindow( StreamBrowser );

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

    GetDisplay()->Clear();

    // Sync the UI with our new status
    EnableInterface();
}


///
/// \brief Play button handler
///

void FrontCamWidget::OnStart()
{
    PvResult lResult = mFrontCamSource->StartAcquisition();
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}


///
/// \brief Stop button handler
///

void FrontCamWidget::OnStop()
{
    PvResult lResult = mFrontCamSource->StopAcquisition();
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}


///
/// \brief Timer event handler
///

void FrontCamWidget::OnTimer()
{
    QString lInfo;
    mFrontCamSource->GetStreamInfo( lInfo );

    mStatusLine->setText( lInfo );
}


///
/// \brief Mode combobox changed handler
///

void FrontCamWidget::OnCbnSelchangeMode( int aIndex )
{
    if ( aIndex < 0 )
    {
        return;
    }

    QString lMode = mModeComboBox->itemText( aIndex );
    PvResult lResult = mFrontCamSource->SetAcquisitionMode( lMode );
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}

void FrontCamWidget::OnSaveImagecheck()
{
    mbSaveImage = !mbSaveImage;
    //qDebug() << "mbSaveImage : " << mbSaveImage;
    mFrontCamSource->SetSaveImage(mbSaveImage);
}


///
/// \brief Generic event handler
///

bool FrontCamWidget::event( QEvent *aEvent )
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

void FrontCamWidget::UpdateAcquisitionModes()
{
    // Get current mode
    QString lCurrentMode;
    mFrontCamSource->GetAcquisitionMode( true, lCurrentMode );

    // Get all possible modes
    std::vector<QString> lModes;
    mFrontCamSource->GetAcquisitionModes( lModes );

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

void FrontCamWidget::UpdateAcquisitionMode()
{
    QString lMode;
    mFrontCamSource->GetAcquisitionMode( false, lMode );
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
