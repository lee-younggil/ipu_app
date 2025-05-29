#include "RearDisplayThread.h"

#include <QCoreApplication>
#include <QTime>
#include <QDebug>

#include <assert.h>

RearDisplayThread::RearDisplayThread(PvDisplayWnd *aDisplayWnd)
    : mDisplayWnd( aDisplayWnd )
{

}

RearDisplayThread::~RearDisplayThread()
{

}

void RearDisplayThread::OnBufferRetrieved(PvBuffer *aBuffer)
{
    Q_UNUSED(aBuffer)
    //qDebug() << "[F::OnBufferRetrieved] - ";
}

void RearDisplayThread::OnBufferDisplay(PvBuffer *aBuffer)
{
    //qDebug() << "[F::OnBufferDisplay] - ";
    mDisplayWnd->Display( *aBuffer );
}

void RearDisplayThread::OnBufferDone(PvBuffer *aBuffer)
{
    Q_UNUSED(aBuffer)
    //qDebug() << "[F::OnBufferDone] - ";
}
