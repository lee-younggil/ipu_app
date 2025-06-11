#ifndef CGENICAMCOMM_H
#define CGENICAMCOMM_H

#include <QObject>

class FrontCamComm
{
public:
    FrontCamComm();
    ~FrontCamComm();

    void initialization(void);	//
    bool ServiceStart(void);	//
    bool ServiceStop(void);		//

protected:
    bool CreateServer();
    static void* ClientAccept(void* data);

private:
    bool m_bServerState;
};

#endif // CGENICAMCOMM_H
