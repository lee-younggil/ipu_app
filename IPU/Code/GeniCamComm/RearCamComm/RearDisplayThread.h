#ifndef REARDISPLAYTHREAD_H
#define REARDISPLAYTHREAD_H

#pragma once

#include <PvBuffer.h>
#include <PvBufferConverter.h>
#include <PvFPSStabilizer.h>
#include <PvDisplayWnd.h>
#include <PvDisplayThread.h>

class ImageSaveDlg;
class FilteringDlg;

class RearDisplayThread : public PvDisplayThread
{
public:
    RearDisplayThread( PvDisplayWnd *aDisplayWnd );
    ~RearDisplayThread();

protected:
    void OnBufferRetrieved( PvBuffer *aBuffer );
    void OnBufferDisplay( PvBuffer *aBuffer );
    void OnBufferDone( PvBuffer *aBuffer );

private:

    PvDisplayWnd *mDisplayWnd;
};

#endif // REARDISPLAYTHREAD_H
