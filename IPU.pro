QT       += core gui network serialport testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
           _UNIX_ _LINUX_ QT_GUI_LIB PV_ENABLE_MP4



#USE_SDK_PLAYER \
#PV_ENABLE_MP4


#Custom define for include & library path
#PUREGEV_ROOT    = $$PWD/ebus_sdk/Ubuntu-22.04-x86_64/
PUREGEV_ROOT    = /opt/jai/ebus_sdk/Ubuntu-22.04-x86_64/
PV_LIBRARY_PATH = $$PUREGEV_ROOT/lib
CODEC_LIB_PATH  = /usr/lib/x86_64-linux-gnu

INCLUDEPATH += \
    Code/ \
    Public/

INCLUDEPATH += \
    /usr/local/include \
    /usr/local/include/boost \
    /usr/include/x86_64-linux-gnu/libavcodec \
    /usr/include/x86_64-linux-gnu/libavformat \
    /usr/include/x86_64-linux-gnu/libswscale \
    $$PUREGEV_ROOT/include \
    $$PV_LIBRARY_PATH \
    $$PV_LIBRARY_PATH/genicam/bin \
    /usr/lib/x86_64-linux-gnu \
    /usr/local/include/opencv4


LIBS += \
#    -L/usr/local/lib \
    -L$$PV_LIBRARY_PATH -lPvAppUtils -lPtConvertersLib -lPvBase -lPvBuffer -lPvGenICam -lPvStream -lPvDevice \
                        -lPvTransmitter -lPvVirtualDevice -lPvPersistence -lPvSerial -lPvSystem -lPvCameraBridge -lPvGUI -lPvCodec \
                        -lswscale -lavcodec -lavformat -lavutil


#    -L$$CODEC_LIB_PATH -lswscale -lavcodec -lavformat -lavutil


DEPENDPATH += $$PV_LIBRARY_PATH

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Code/CdsLicComm/CdsLicComm.cpp \
    Code/CdsLicComm/CdsLicDlg.cpp \
    Code/CdsLicComm/test_CCdsLicComm.cpp \
    Code/CdsLicComm/test_QString.cpp \
    Code/GeniCamComm/FrontCamComm/FrontCamComm.cpp \
    Code/GeniCamComm/FrontCamComm/FrontCamDlg.cpp \
    Code/GeniCamComm/FrontCamComm/FrontCamSource.cpp \
    Code/GeniCamComm/FrontCamComm/FrontCamWidget.cpp \
    Code/GeniCamComm/FrontCamComm/FrontDisplayThread.cpp \
    Code/GeniCamComm/RearCamComm/RearCamComm.cpp \
    Code/GeniCamComm/RearCamComm/RearCamDlg.cpp \
    Code/GeniCamComm/RearCamComm/RearCamSource.cpp \
    Code/GeniCamComm/RearCamComm/RearCamWidget.cpp \
    Code/GeniCamComm/RearCamComm/RearDisplayThread.cpp \
    Code/ICamServerComm/CICamServerComm.cpp \
    Code/RCServerComm/RCServerComm.cpp \
    Code/VDUComm/VDUComm.cpp \
    Code/VDUComm/VDUDlg.cpp \
    Code/Log/Log.cpp \
    Code/Util/Util.cpp \
    Code/VVPComm/VVPComm.cpp \
    Code/Watchdog/Watchdog.cpp \
    Code/Mediator.cpp \
    Code/VVPComm/VVPDlg.cpp \
    IpuFactory.cpp \
    IpuInfoDlg.cpp \
    IpuMainDlg.cpp \
    Public/CircularQueue/CircularQueue.cpp \
    Public/Comm/IFSerialComm.cpp \
    Public/Comm/IFTcpComm.cpp \
    Public/Comm/SerialPort.cpp \
    Public/Config/IPUConfig.cpp \
    QIpuApplication.cpp \
    main.cpp

HEADERS += \
    Code/CdsLicComm/CdsLicComm.h \
    Code/CdsLicComm/CdsLicDlg.h \
    Code/GeniCamComm/CamDefines.h \
    Code/GeniCamComm/FrontCamComm/FrontCamComm.h \
    Code/GeniCamComm/FrontCamComm/FrontCamDlg.h \
    Code/GeniCamComm/FrontCamComm/FrontCamSource.h \
    Code/GeniCamComm/FrontCamComm/FrontCamWidget.h \
    Code/GeniCamComm/FrontCamComm/FrontDisplayThread.h \
    Code/GeniCamComm/RearCamComm/RearCamComm.h \
    Code/GeniCamComm/RearCamComm/RearCamDlg.h \
    Code/GeniCamComm/RearCamComm/RearCamSource.h \
    Code/GeniCamComm/RearCamComm/RearCamWidget.h \
    Code/GeniCamComm/RearCamComm/RearDisplayThread.h \
    Code/HandleManager.h \
    Code/ICamServerComm/CICamServerComm.h \
    Code/RCServerComm/RCServerComm.h \
    Code/Service.h \
    Code/VDUComm/VDUComm.h \
    Code/VDUComm/VDUDlg.h \
    Code/Mediator.h \
    Code/HandleManager.h \
    Code/VVPComm/VVPComm.h \
    Code/VVPComm/VVPDlg.h \
    Code/Log/Log.h \
    Code/Util/Util.h \
    Code/Watchdog/Watchdog.h \
    CompletionStruct.h \
    IpuFactory.h \
    IpuInfoDlg.h \
    IpuMainDlg.h \
    Public/CircularQueue/CircularQueue.h \
    Public/Comm/IFComm.h \
    Public/Comm/IFSerialComm.h \
    Public/Comm/IFTcpComm.h \
    Public/Comm/SerialPort.h \
    Public/Config/IPUConfig.h \
    Public/DataTypeDefine.h \
    Public/IFService.h \
    IPUDefine.h \
    QIpuApplication.h

FORMS += \
    Public/Config/IPUConfig.ui \
    QIpuApplication.ui

TRANSLATIONS += \
    IPU_ko_KR.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Code/GeniCamComm/GeniCam.qrc

DISTFILES += \
    Code/GeniCamComm/res/CamSource.ico \
    Code/GeniCamComm/res/play.bmp \
    Code/GeniCamComm/res/stop.bmp



