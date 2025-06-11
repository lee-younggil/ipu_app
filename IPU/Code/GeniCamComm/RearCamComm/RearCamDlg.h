#ifndef REARCAMDLG_H
#define REARCAMDLG_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMutex>
#include <QEvent>
#include <QTime>
#include <QTimer>

#include <PvDeviceInfoGEV.h>
#include <PvDeviceGEV.h>

class RearCamSource;
class RearCamWidget;
class PvGenBrowserWnd;


//class CRearCamDlg : public QWidget, protected PvDeviceEventSink
class RearCamDlg : public QWidget
{
    Q_OBJECT
public:
    //CRearCamDlg();
    explicit RearCamDlg(QWidget *parent = nullptr);
    ~RearCamDlg();

signals:
    void DeviceDisconnected();

protected slots:
    void OnDeviceDisconnect();

protected:
    // UI
    void CreateLayout();

    // Inherited from PvDeviceEventSink.
    void OnLinkDisconnected( PvDevice *aDevice );

    void EnableInterface();

    void ShowGenWindow( PvGenBrowserWnd *aWnd, PvGenParameterArray *aArray, const QString &aTitle );
    void CloseGenWindow( PvGenBrowserWnd *aWnd );

    void Connect( const PvDeviceInfoGEV *aDeviceInfo );
    void Disconnect();

private:
    RearCamWidget *mRearCamWidget;
    RearCamSource *mRearCamSource;

    //PvDeviceGEV mDevice;

    PvGenBrowserWnd *mCommunicationBrowser;
    PvGenBrowserWnd *mDeviceBrowser;
    PvGenBrowserWnd *StreamBrowser;
};

#endif // REARCAMDLG_H
