#ifndef FRONTDISPLAYTHREAD_H
#define FRONTDISPLAYTHREAD_H

#pragma once

//#include <PvBuffer.h>
#include <PvBufferConverter.h>
#include <PvFPSStabilizer.h>
#include <PvDisplayWnd.h>
#include <PvDisplayThread.h>

//class ImageSaveDlg;
//class FilteringDlg;

class PvBuffer;

//class FrontCamWidget;
//class FrontCamSource;

class FrontDisplayThread : public PvDisplayThread
{
public:
    FrontDisplayThread( PvDisplayWnd *aDisplayWnd );
    ~FrontDisplayThread();

    PvBuffer *RetrieveLatestBuffer();

    //PvBuffer *RetrieveLatestBuffer();
    void SetSaveImage(bool bsave) { mSaveImage = bsave; }
    bool GetSaveEnable() { return mSaveImage; }

    void SetPvBuffer(PvBuffer *pBuffer);
    PvBuffer *GetPvBuffer();

protected:
    void OnBufferRetrieved( PvBuffer *aBuffer );
    void OnBufferDisplay( PvBuffer *aBuffer );
    void OnBufferDone( PvBuffer *aBuffer );

private:

    PvDisplayWnd *mDisplayWnd;

    //FrontCamWidget *mFrontCamWidget;
    //FrontCamSource *mFrontCamSource;

    bool mSaveImage;
    PvBuffer *mPvBuffer;
};

#endif // FRONTDISPLAYTHREAD_H
