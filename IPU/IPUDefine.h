#ifndef IPUDEFINE_H
#define IPUDEFINE_H

#include <QObject>


class CIPUDefine : QObject
{
    Q_OBJECT
public:
    explicit CIPUDefine(QObject *parent = nullptr);

    enum ConnectState
    {
        DISCONNECT,
        CONNECTING,
        CONNECTED,
    };

    enum ComboBoxType{
        CB_NONE = 0,
        CB_CDS,
        CB_LIC,

        CB_ALL,

        CB_MAX
    };

    enum LogSource
    {
        LOG_INFO = 0,
        LOG_ISERVER,    //Intergrate Video Server
        LOG_RSERVER,    //Remote Control Server

        LOG_FCAM,
        LOG_RCAM,

        LOG_MAX
    } ;

    enum LogLevel
    {
        LOG_LEVEL_BASIC,		// error
        LOG_LEVEL_OPERATION,	//
        LOG_LEVEL_DETAIL,		//
        LOG_LEVEL_TEST,			//
    };

    Q_ENUM(ConnectState)
    Q_ENUM(ComboBoxType)

    Q_ENUM(LogSource)
    Q_ENUM(LogLevel)
};

#ifdef WIN32
#define STR_DRIVER_PATH "C:\\"
#define STR_APP_DIR "IPU"
#define STR_BIN_DIR "BIN"
#define STR_LOG_DIR "LOG"
#else
#define STR_DRIVER_PATH "/home"
#define STR_APP_DIR "IPU"
#define STR_BIN_DIR "BIN"
#define STR_LOG_DIR "LOG"
#define IMG_DIR_PATH "IMG"


#define IPU_PATH        ("/home/ipu/")
#define IPU_FIMAGE_PATH ("/home/ipu/vvp/image/front/")
#define IPU_REAR_PATH   ("/home/ipu/vvp/image/rear/")
#endif

#define MAX_PATH    255

#endif // IPUDEFINE_H
