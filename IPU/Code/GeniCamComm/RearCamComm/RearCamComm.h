#ifndef CREARCAMCOMM_H
#define CREARCAMCOMM_H

#include <QObject>

class RearCamComm
{
public:
    RearCamComm();
    ~RearCamComm();

    void initialization(void);	//
    bool ServiceStart(void);	//
    bool ServiceStop(void);		//
};

#endif // CREARCAMCOMM_H
