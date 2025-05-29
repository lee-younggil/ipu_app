#ifndef FRONTCAMDLG_H
#define FRONTCAMDLG_H

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


class FrontCamSource;
class FrontCamWidget;
class PvGenBrowserWnd;



//class FrontCamDlg : public QWidget, protected PvDeviceEventSink
class FrontCamDlg : public QWidget
{
    Q_OBJECT
public:
    //CRearCamDlg();
    explicit FrontCamDlg(QWidget *parent = nullptr);
    virtual ~FrontCamDlg();

protected:
    // UI
    void CreateLayout();

    // Inherited from PvDeviceEventSink.
    void OnLinkDisconnected( PvDevice *aDevice );

    void EnableInterface();

    void ShowGenWindow( PvGenBrowserWnd *aWnd, PvGenParameterArray *aArray, const QString &aTitle );
    void CloseGenWindow( PvGenBrowserWnd *aWnd );

private:
    FrontCamWidget *mFrontCamWidget;
    FrontCamSource *mFrontCamSource;

    //PvDeviceGEV mDevice;

    PvGenBrowserWnd *mCommunicationBrowser;
    PvGenBrowserWnd *mDeviceBrowser;
    PvGenBrowserWnd *StreamBrowser;

};

#endif // FRONTCAMDLG_H
