#ifndef REARCAMSOURCE_H
#define REARCAMSOURCE_H

#pragma once

#include <PvAcquisitionStateManager.h>
#include <PvDeviceGEV.h>
#include <PvStreamGEV.h>
#include <PvPipeline.h>

#include <vector>
#include <QString>


class PvDisplayWnd;
class RearCamWidget;
class RearDisplayThread;

class RearCamSource : protected PvAcquisitionStateEventSink, protected PvPipelineEventSink, protected PvGenEventSink
{
public:
    RearCamSource();
    virtual ~RearCamSource();

    PvResult Open( PvDeviceGEV *aDevice, const PvString &aIPAddress, const PvString &aLocalIPAddress, uint16_t aChannel );
    PvResult Close();

    PvResult StartStreaming();
    PvResult StopStreaming();

    PvResult StartAcquisition();
    PvResult StopAcquisition();

    bool IsOpened() { return mStream->IsOpen(); }
    PvGenParameterArray *GetParameters() { return mStream->GetParameters(); }
    PvAcquisitionState GetAcquisitionState();

    void GetAcquisitionMode( bool aSelectSourceIfNeeded, QString &aAcquisitionMode );
    void GetAcquisitionModes( std::vector<QString> &aVector );
    PvResult SetAcquisitionMode( const QString &aAcquisitionMode );

    void SetWidget(RearCamWidget *aWidget );

    void GetStreamInfo( QString &aInfo );

protected:

    uint32_t GetPayloadSize();

     // PvPipelineEventSink events
    void OnBufferTooSmall( PvPipeline *aPipeline, bool *aReallocAll, bool *aResetStats );

    // PvAcquisitionStateEventSink events
    void OnAcquisitionStateChanged( PvDevice* aDevice, PvStream* aStream, uint32_t aSource, PvAcquisitionState aState );

    // PvGenParameterEventSink events
    void OnParameterUpdate( PvGenParameter *aParameter );

private:

    // Owned by the source object
    PvStreamGEV *mStream;
    PvPipeline *mPipeline;
    PvAcquisitionStateManager *mAcquisitionManager;

    // Referenced from outside
    PvDeviceGEV *mDevice;
    RearCamWidget *mWidget;

    RearDisplayThread *mDisplayThread;

    uint16_t mSourceIndex;

    bool mMultiSource;
    bool mBuffersReallocated;
};

#endif // REARCAMSOURCE_H
