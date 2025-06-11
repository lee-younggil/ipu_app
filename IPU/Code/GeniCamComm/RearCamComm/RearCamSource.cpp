#include <stdlib.h>
#include <stdio.h>

#include <QCoreApplication>
#include <QEvent>
#include <QThread>
#include <QDebug>

#include "RearCamSource.h"
#include "GeniCamComm/CamDefines.h"
#include "RearCamWidget.h"
#include "RearDisplayThread.h"

#include <PvDisplayWnd.h>
#include <PvGenStateStack.h>
#include <PvStreamInfo.h>

#include <assert.h>
#include <limits.h>

RearCamSource::RearCamSource()
    : mStream( NULL )
    , mPipeline( NULL )
    , mAcquisitionManager( NULL )
    , mDevice( NULL )
    , mWidget( NULL )
    , mDisplayThread( NULL )
    , mMultiSource( false )
    , mBuffersReallocated( false )
{
    // Owned by the source object
    mStream = new PvStreamGEV;
    mPipeline = new PvPipeline( mStream );
    mPipeline->RegisterEventSink( this );

    // Source index
    //mSourceIndex = aSourceIndex;
}

RearCamSource::~RearCamSource()
{
    assert( mAcquisitionManager == NULL );
    assert( mDisplayThread != NULL );
    assert( mPipeline != NULL );

    if ( mDisplayThread != NULL )
    {
        delete mDisplayThread;
        mDisplayThread = NULL;
    }

    if ( mPipeline != NULL )
    {
        mPipeline->UnregisterEventSink( this );
        SAFE_DELETE( mPipeline );
    }

    SAFE_DELETE( mStream );
}

///
/// \brief Sets the widget, display (and creates the acquisition thread)
///

void RearCamSource::SetWidget( RearCamWidget *aWidget )
{
    mWidget = aWidget;

    // Display thread
    mDisplayThread = new RearDisplayThread( mWidget->GetDisplay() );
}


///
/// \brief Returns the acquisition state for the source
///

PvAcquisitionState RearCamSource::GetAcquisitionState()
{
    if ( mAcquisitionManager == NULL )
    {
        return PvAcquisitionStateUnknown;
    }

    return mAcquisitionManager->GetState();
}



///
/// \brief Opens the source
///

PvResult RearCamSource::Open( PvDeviceGEV *aDevice, const PvString &aIPAddress, const PvString &aLocalIPAddress, uint16_t aChannel )
{
    // The device should already be connected
    mDevice = aDevice;

    // Is the device multisource?
    PvGenEnum *lSourceSelector = dynamic_cast<PvGenEnum *>( mDevice->GetParameters()->Get( "SourceSelector" ) );
    if ( lSourceSelector != NULL )
    {
        int64_t lCount;
        lSourceSelector->GetEntriesCount( lCount );

        mMultiSource = false; //( lCount > 1 ); //cjkim
    }

    // Register acquisition mode update notifications
    PvGenEnum *lMode = mDevice->GetParameters()->GetEnum( "AcquisitionMode" );
    if ( lMode != NULL )
    {
        lMode->RegisterEventSink( this );
    }

    // Open stream
    PvResult lResult = mStream->Open( aIPAddress, 0, aChannel, aLocalIPAddress );
    if ( !lResult.IsOK() )
    {
        return lResult;
    }

    // Negotiate packet size. On failure, assume the device is set to a valid packet size.
    mDevice->NegotiatePacketSize( aChannel, 1476 );

    // Set stream destination
    lResult = mDevice->SetStreamDestination( mStream->GetLocalIPAddress(), mStream->GetLocalPort(), aChannel );
    if ( !lResult.IsOK() )
    {
        return lResult;
    }

    QCoreApplication::sendEvent( mWidget, new QEvent( static_cast<QEvent::Type>( WM_UPDATEACQMODES ) ) );

    // Ready image reception
    lResult = StartStreaming();
    if ( !lResult.IsOK() )
    {
        return lResult;
    }

    return PvResult::Code::OK;
}


///
/// \brief Closes the source
///

PvResult RearCamSource::Close()
{
    // Stop image reception
    PvResult lResult = StopStreaming();
    if ( !lResult.IsOK() )
    {
        return lResult;
    }

    // Close stream
    if ( mStream->IsOpen() )
    {
        lResult = mStream->Close();
        if ( !lResult.IsOK() )
        {
            return lResult;
        }
    }

    if ( ( mDevice != NULL ) && mDevice->IsConnected() )
    {
        // Unregister acquisition mode update notifications
        PvGenEnum *lMode = mDevice->GetParameters()->GetEnum( "AcquisitionMode" );
        if ( lMode != NULL )
        {
            lMode->UnregisterEventSink( this );
        }
    }

    // We don't own the device, let's just forget about it
    mDevice = NULL;

    return PvResult::Code::OK;
}


///
/// \brief Stars streaming
///

PvResult RearCamSource::StartStreaming()
{
    if ( !mStream->IsOpen() || // Not initialized yet
         mPipeline->IsStarted() ) // Already streaming
    {
        return PvResult::Code::NOT_INITIALIZED;
    }

    // Start threads
    mDisplayThread->Start( mPipeline, NULL );
    mDisplayThread->SetPriority( QThread::HighPriority );

    // Configure acquisition state manager
    mAcquisitionManager = new PvAcquisitionStateManager( mDevice, mStream, mSourceIndex );
    mAcquisitionManager->RegisterEventSink( this );

    // Start pipeline
    PvResult lResult = mPipeline->Start();
    if ( !lResult.IsOK() )
    {
        return lResult;
    }

    //PvResult RetrieveNextBuffer(PvBuffer ** aBuffer, uint32_t aTimeout = 0xFFFFFFFF, PvResult * aOperationResult = NULL );

    return PvResult::Code::OK;
}


///
/// \brief Stops the stream, pipeline.
///

PvResult RearCamSource::StopStreaming()
{
    if ( !mStream->IsOpen() || // Not initialized yet
         !mPipeline->IsStarted() ) // Not streaming
    {
        return PvResult::Code::NOT_INITIALIZED;
    }

    // Stop display thread
    if ( mDisplayThread->IsRunning() )
    {
        mDisplayThread->Stop( false );
    }

    // Release acquisition manager
    if ( mAcquisitionManager != NULL )
    {
        mAcquisitionManager->UnregisterEventSink( this );

        delete mAcquisitionManager;
        mAcquisitionManager = NULL;
    }

    // Stop pipeline
    PvResult lResult = mPipeline->Stop();
    if ( !lResult.IsOK() )
    {
        return lResult;
    }

    // Wait for the display thread to be completed
    if ( mDisplayThread != NULL )
    {
        mDisplayThread->WaitComplete();
    }


    return PvResult::Code::OK;
}


///
/// \brief Starts the acquisition
///

PvResult RearCamSource::StartAcquisition()
{
    if ( mDevice == NULL )
    {
        // No device bound to this source
        return PvResult::Code::NOT_INITIALIZED;
    }

    PvGenStateStack lState( mDevice->GetParameters() );
    if ( mMultiSource )
    {
        // Push change on stack, will be reset when lState goes out of scope
        lState.SetEnumValue( "SourceSelector", mSourceIndex );
    }

    // Get payload size
    uint32_t lPayloadSize = GetPayloadSize();
    if ( lPayloadSize > 0 )
    {
        // Propagate to pipeline to make sure buffers are big enough
        mPipeline->SetBufferSize( lPayloadSize );

        qDebug() << "[F::StartAcquisition] - lPayloadSize : " << lPayloadSize;
        qDebug() << "[F::StartAcquisition] - BufferSize : " << mPipeline->GetBufferSize();
    }

    // Reset pipeline
    mPipeline->Reset();
    mBuffersReallocated = false;

    // Reset stream statistics
    PvGenCommand *lResetStats = mStream->GetParameters()->GetCommand( "Reset" );
    lResetStats->Execute();

    // Reset dislay thread stats (mostly frames displayed per seconds)
    mDisplayThread->ResetStatistics();

    // Use acquisition manager to send the acquisition start command to the device
    PvResult lResult = mAcquisitionManager->Start();
    if ( !lResult.IsOK() )
    {
        return lResult;
    }

    return PvResult::Code::OK;
}


///
/// \brief Stops the acquisition.
///

PvResult RearCamSource::StopAcquisition()
{
    PvGenStateStack lState( mDevice->GetParameters() );
    if ( mMultiSource )
    {
        // Push change on stack, will be reset when lState goes out of scope
        lState.SetEnumValue( "SourceSelector", mSourceIndex );
    }

    // Use acquisition manager to send the acquisition stop command to the device
    PvResult lResult = mAcquisitionManager->Stop();
    if ( !lResult.IsOK() )
    {
        return lResult;
    }

    return PvResult::Code::OK;
}


///
/// \brief Retrieve or guess the payload size
///

uint32_t RearCamSource::GetPayloadSize()
{
    PvGenStateStack lState( mDevice->GetParameters() );
    if ( mMultiSource )
    {
        // Push change on stack, will be reset when lState goes out of scope
        lState.SetEnumValue( "SourceSelector", mSourceIndex );
    }

    // Get parameters required
    PvGenInteger *lPayloadSize = mDevice->GetParameters()->GetInteger("PayloadSize" );
    PvGenInteger *lWidth = mDevice->GetParameters()->GetInteger( "Width" );
    PvGenInteger *lHeight = mDevice->GetParameters()->GetInteger( "Height" );
    PvGenEnum *lPixelFormat = mDevice->GetParameters()->GetEnum( "PixelFormat" );

    // Try getting the payload size from the PayloadSize mandatory parameter
    int64_t lPayloadSizeValue = 0;
    if ( lPayloadSize != NULL )
    {
        lPayloadSize->GetValue( lPayloadSizeValue );

        // Round up to make it mod 32 (works around an issue with some devices)
        if ((lPayloadSizeValue % 32) != 0)
        {
            lPayloadSizeValue = ((lPayloadSizeValue / 32) + 1) * 32;
        }
    }

    // Compute poor man's payload size - for devices not maintaining PayloadSize properly
    int64_t lPoorMansPayloadSize = 0;
    if ( (lWidth != NULL ) && ( lHeight != NULL ) && ( lPixelFormat != NULL ) )
    {
        int64_t lWidthValue, lHeightValue;
        lWidth->GetValue( lWidthValue );
        lHeight->GetValue( lHeightValue );

        qDebug() << "[F::GetPayloadSize] - lWidthValue : " << lWidthValue << ", lHeightValue : " << lHeightValue;

        int64_t lPixelFormatValue;
        lPixelFormat->GetValue( lPixelFormatValue );

        int64_t lPixelSizeInBits = PvImage::GetPixelSize( (PvPixelType)lPixelFormatValue );

        lPoorMansPayloadSize = ( lWidthValue * lHeightValue * lPixelSizeInBits) / 8;
    }

    // Take max, let pipeline know what the payload size is
    int64_t lBestPayloadSize = ( lPayloadSizeValue >= lPoorMansPayloadSize ) ? lPayloadSizeValue : lPoorMansPayloadSize;
    if ( ( lBestPayloadSize > 0 ) && ( lBestPayloadSize < LONG_MAX ) )
    {
        return static_cast<uint32_t>( lBestPayloadSize );
    }

    // Could not compute/retrieve payload size...
    return 0;
}


///
/// \brief PvPipelineEventSink events
///

void RearCamSource::OnBufferTooSmall( PvPipeline *aPipeline, bool *aReallocAll, bool *aResetStats )
{
    *aReallocAll = true;
    *aResetStats = true;

    mBuffersReallocated = true;
}


///
/// \brief PvAcquisitionStateEventSink events
///

void RearCamSource::OnAcquisitionStateChanged( PvDevice* aDevice, PvStream* aStream, uint32_t aSource, PvAcquisitionState aState )
{
    QCoreApplication::postEvent( mWidget, new QEvent( static_cast<QEvent::Type>( WM_UPDATEACQCONTROLS ) ) );
}


///
/// \brief Changes the acquisition mode
///

PvResult RearCamSource::SetAcquisitionMode( const QString &aAcquisitionMode )
{
    PvGenStateStack lState( mDevice->GetParameters() );
    if ( mMultiSource )
    {
        // Push change on stack, will be reset when lState goes out of scope
        lState.SetEnumValue( "SourceSelector", mSourceIndex );
    }

    PvGenEnum *lMode = mDevice->GetParameters()->GetEnum( "AcquisitionMode" );
    if ( lMode != NULL )
    {
        PvResult lResult = lMode->SetValue( aAcquisitionMode.toUtf8().constData() );
        if ( !lResult.IsOK() )
        {
            return lResult;
        }
    }

    return PvResult::Code::OK;
}


///
/// \brief Returns the current acquisition mode.
///
/// If aSelectSourceIfNeeded is false and the correct source is not selected
/// and empty string is returned. If it is true the source will be selected
/// if needed.
///

void RearCamSource::GetAcquisitionMode( bool aSelectSourceIfNeeded, QString &aAcquisitionMode )
{
    aAcquisitionMode = "";

    PvGenStateStack lState( mDevice->GetParameters() );
    if ( mMultiSource )
    {
        int64_t lCurrentSource = 0;
        mDevice->GetParameters()->GetEnumValue( "SourceSelector", lCurrentSource );

        if ( lCurrentSource != mSourceIndex )
        {
            if ( aSelectSourceIfNeeded )
            {
                // Push change on stack, will be reset when lState goes out of scope
                lState.SetEnumValue( "SourceSelector", mSourceIndex );
            }
            else
            {
                return;
            }
        }
    }

    PvGenEnum *lMode = mDevice->GetParameters()->GetEnum( "AcquisitionMode" );
    if ( lMode != NULL )
    {
        PvString lValue;
        lMode->GetValue( lValue );

        aAcquisitionMode = lValue.GetAscii();
    }
}


///
/// \brief Parameter update notification.
///

void RearCamSource::OnParameterUpdate( PvGenParameter *aParameter )
{
    if ( aParameter->GetName() == "AcquisitionMode" )
    {
        QCoreApplication::postEvent( mWidget, new QEvent( static_cast<QEvent::Type>( WM_UPDATEACQMODE ) ) );
    }
}


///
/// \brief Returns the list of valid acquisition modes for the source
///

void RearCamSource::GetAcquisitionModes( std::vector<QString> &aVector )
{
    aVector.clear();

    PvGenStateStack lState( mDevice->GetParameters() );
    if ( mMultiSource )
    {
        // Push change on stack, will be reset when lState goes out of scope
        lState.SetEnumValue( "SourceSelector", mSourceIndex );
    }

    PvGenEnum *lMode = mDevice->GetParameters()->GetEnum( "AcquisitionMode" );
    if ( lMode != NULL )
    {
        int64_t lCount;
        lMode->GetEntriesCount( lCount );

        for ( int64_t i = 0; i < lCount; i++ )
        {
            const PvGenEnumEntry *lEE = NULL;
            lMode->GetEntryByIndex( i, &lEE );
            if ( ( lEE != NULL ) && lEE->IsAvailable() )
            {
                PvString lName;
                lEE->GetName( lName );

                aVector.push_back( lName.GetAscii() );
            }
        }
    }
}


///
/// \brief Returns a string containing info about the stream.
///

void RearCamSource::GetStreamInfo( QString &aInfo )
{
    int lFrameRate = 0;
    if ( mDisplayThread->IsRunning() )
    {
        lFrameRate = mDisplayThread->GetFPS();
    }

    PvStreamInfo lInfo( mStream );
    QString lStatistics = lInfo.GetStatistics( lFrameRate ).GetAscii();
    QString lErrors = lInfo.GetErrors().GetAscii();
    QString lWarnings = lInfo.GetWarnings( mBuffersReallocated ).GetAscii();

    aInfo = lStatistics + "\r\n" + lErrors + "\r\n" + lWarnings;
}
