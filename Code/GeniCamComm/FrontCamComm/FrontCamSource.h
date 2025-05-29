#ifndef FRONTCAMSOURCE_H
#define FRONTCAMSOURCE_H

#pragma once

#include <PvAcquisitionStateManager.h>
#include <PvDeviceGEV.h>
#include <PvStreamGEV.h>
#include <PvPipeline.h>

#include <vector>
#include <QString>


class PvDisplayWnd;
class FrontCamWidget;
class FrontDisplayThread;


class FrontCamSource : protected PvAcquisitionStateEventSink, protected PvPipelineEventSink, protected PvGenEventSink
{
public:

    //FrontCamSource( uint16_t aSourceIndex );
    FrontCamSource();
    virtual ~FrontCamSource();

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

    void SetWidget(FrontCamWidget *aWidget );

    void GetStreamInfo( QString &aInfo );

    void SetSaveImage(bool bsave);

protected:

    uint32_t GetPayloadSize();

     // PvPipelineEventSink events
    void OnBufferTooSmall( PvPipeline *aPipeline, bool *aReallocAll, bool *aResetStats );

    // PvAcquisitionStateEventSink events
    void OnAcquisitionStateChanged( PvDevice* aDevice, PvStream* aStream, uint32_t aSource, PvAcquisitionState aState );

    // PvGenParameterEventSink events
    void OnParameterUpdate( PvGenParameter *aParameter );

    void SaveRawDataToImage(PvBuffer * pBuffer);

private:

    // Owned by the source object
    PvStreamGEV *mStream;
    PvPipeline *mPipeline;
    PvAcquisitionStateManager *mAcquisitionManager;

    // Referenced from outside
    PvDeviceGEV *mDevice;
    FrontCamWidget *mFrontCamWidget;

    FrontDisplayThread *mDisplayThread;

    uint16_t mSourceIndex;

    bool mMultiSource;
    bool mBuffersReallocated;

};

#endif // FRONTCAMSOURCE_H
