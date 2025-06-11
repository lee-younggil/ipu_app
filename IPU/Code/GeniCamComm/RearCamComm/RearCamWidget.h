#ifndef REARCAMWIDGET_H
#define REARCAMWIDGET_H

#pragma once

#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QTimer>

#include <PvDeviceFinderWnd.h>
#include <PvDeviceInfoGEV.h>
#include <PvDeviceGEV.h>


class PvDisplayWnd;
class RearCamSource;

#define WM_UPDATEACQCONTROLS ( 0x4000 )
#define WM_UPDATEACQMODE ( 0x4001 )
#define WM_UPDATEACQMODES ( 0x4002 )

class RearCamWidget : public QWidget, protected PvDeviceEventSink
{
    Q_OBJECT
public:
    RearCamWidget();
    RearCamWidget( const QString &aTitle, RearCamSource *aSource, QWidget *parent = 0, Qt::WindowFlags flags = Qt::WindowFlags() );
    virtual ~RearCamWidget();

    QComboBox *GetModeComboBox() { return mModeComboBox; }
    PvDisplayWnd *GetDisplay() { return mDisplay; }

    void EnableInterface();

protected slots:
    void OnConnectButton();
    void OnDisconnectButton();
    void OnStart();
    void OnStop();
    void OnTimer();
    void OnCbnSelchangeMode( int aIndex );

protected:

    bool event( QEvent *aEvent );

    void UpdateAcquisitionMode();
    void UpdateAcquisitionModes();

    // UI
    void CreateLayout();
    QLayout *CreateDisplayLayout();
    QLayout *CreateControlsLayout();

    void Connect( const PvDeviceInfoGEV *aDeviceInfo );
    void Disconnect();

private:
    PvDeviceGEV mDevice;
    PvDisplayWnd *mDisplay;

    QTextEdit *mStatusLine;
    QTimer *mTimer;

    QComboBox *mModeComboBox;
    QPushButton *mConnectButton;
    QPushButton *mDisconnectButton;
    QPushButton *mPlayButton;
    QPushButton *mStopButton;

    QString mTitle;

    RearCamSource *mRearCamSource;
};

#endif // REARCAMWIDGET_H
