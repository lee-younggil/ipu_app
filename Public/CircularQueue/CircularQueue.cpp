#include "CircularQueue.h"



CircularQueue::CircularQueue(void)
    : m_uiFrontIdx(0), m_uiRearIdx(0)
    , m_uiQueueSize(0), m_pcaCircularQueue(NULL)
{
}

CircularQueue::~CircularQueue(void)
{
    deleteQueue ();
}

void CircularQueue::initQueue(unsigned int uiSize)
{
    if(m_pcaCircularQueue != NULL)
        delete [] m_pcaCircularQueue;

    m_uiQueueSize = uiSize + 1;

    m_pcaCircularQueue = new unsigned char[m_uiQueueSize];
    memset(m_pcaCircularQueue, 0, m_uiQueueSize);

    m_uiFrontIdx = m_uiRearIdx = 0;
}

void CircularQueue::deleteQueue(void)
{
    if(m_pcaCircularQueue != NULL)
        delete [] m_pcaCircularQueue;
    m_pcaCircularQueue = NULL;

    m_uiQueueSize = 0;
    m_uiFrontIdx = m_uiRearIdx = 0;
}

bool CircularQueue::push(const unsigned char *pucaBuf, unsigned int uiSize)
{
    if( ( m_uiQueueSize - GetQueueSize() ) <= uiSize )
    {
        //printf( ("ERROR : Queue Push ����! : %d\n"), uiSize );
        return false;
    }

    if( (m_uiRearIdx + uiSize) > m_uiQueueSize )
    {
        memcpy( ( m_pcaCircularQueue + m_uiRearIdx ) , pucaBuf, m_uiQueueSize - m_uiRearIdx);
        memcpy( m_pcaCircularQueue, ( pucaBuf + m_uiQueueSize - m_uiRearIdx ), uiSize -(  m_uiQueueSize - m_uiRearIdx ) );
    }
    else
    {
        memcpy( ( m_pcaCircularQueue + m_uiRearIdx ), pucaBuf, uiSize);
    }

    m_uiRearIdx += uiSize;
    m_uiRearIdx %= m_uiQueueSize;
    return true;
}

void CircularQueue::pop( unsigned int size)
{
    if( GetQueueSize() < size )
    {
        //printf( ("ERROR : CircularQueue pop(), size=%d"), size);
        return;
    }

    m_uiFrontIdx += size;
    m_uiFrontIdx %= m_uiQueueSize;
};

void CircularQueue::clear()
{
    m_uiFrontIdx = m_uiRearIdx;
}

void CircularQueue::Get( unsigned char * pucaDst, unsigned int iLength)
{
    if(GetQueueSize() < iLength)
    {
        //printf( ("ERROR : CircularQueue Get()") );
        return;
    }
    else
    {
        if( iLength < ( m_uiQueueSize - m_uiFrontIdx ) )
        {
            memcpy( pucaDst, ( m_pcaCircularQueue + m_uiFrontIdx ), iLength );
        }
        else
        {
            memcpy( pucaDst, ( m_pcaCircularQueue + m_uiFrontIdx ), m_uiQueueSize - m_uiFrontIdx );
            memcpy( ( pucaDst + m_uiQueueSize - m_uiFrontIdx ) , m_pcaCircularQueue, iLength - ( m_uiQueueSize - m_uiFrontIdx ) );
        }
    }
}

// iPosition : 1-base index
bool CircularQueue::GetByte( unsigned char *pucCh, unsigned int iPosition)
{
    int iPos;
    if( GetQueueSize() < iPosition )
        return false;
    iPos = m_uiFrontIdx + iPosition - 1;
    iPos %= m_uiQueueSize;

    *pucCh = m_pcaCircularQueue[iPos];

    return true;
}

bool CircularQueue::IsEmpty()
{
    return (m_uiFrontIdx == m_uiRearIdx);
}

unsigned int CircularQueue::GetQueueSize()
{
    return ((m_uiFrontIdx > m_uiRearIdx) ? m_uiQueueSize : 0) + m_uiRearIdx - m_uiFrontIdx;
}
