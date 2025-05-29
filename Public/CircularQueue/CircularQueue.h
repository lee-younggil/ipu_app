#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#pragma once

#include <QObject>

class CircularQueue
{
public:
    CircularQueue(void);
    virtual ~CircularQueue(void);

    void initQueue(unsigned int uiSize);
    void deleteQueue(void);

    bool push(const unsigned char *pucaBuf, unsigned int uiSize = 1 );
    void pop( unsigned int size = 1);
    void Get( unsigned char * pucaDst, unsigned int iLength);
    bool GetByte( unsigned char *pucCh, unsigned int iPosition);
    void clear(void);
    bool IsEmpty(void);
    unsigned int GetQueueSize(void);
    unsigned char Front(void){return m_pcaCircularQueue[m_uiFrontIdx];};

private:
    unsigned int m_uiFrontIdx;
    unsigned int m_uiRearIdx;
    unsigned int m_uiQueueSize;
    unsigned char *m_pcaCircularQueue;
};

#endif // CIRCULARQUEUE_H
