#ifndef FRONTCAMWIDGET_H
#define FRONTCAMWIDGET_H

#pragma once

#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>

#include <PvDeviceFinderWnd.h>
#include <PvDeviceInfoGEV.h>
#include <PvDeviceGEV.h>
#include <PvStream.h>

#include <opencv2/opencv.hpp>


class PvDisplayWnd;
class FrontCamSource;

#define WM_UPDATEACQCONTROLS ( 0x4000 )
#define WM_UPDATEACQMODE ( 0x4001 )
#define WM_UPDATEACQMODES ( 0x4002 )

class FrontCamWidget : public QWidget, protected PvDeviceEventSink
{
    Q_OBJECT

public:
    FrontCamWidget();
    FrontCamWidget( const QString &aTitle, FrontCamSource *aSource, QWidget *parent = 0, Qt::WindowFlags flags = Qt::WindowFlags() );
    virtual ~FrontCamWidget();

    QComboBox *GetModeComboBox() { return mModeComboBox; }
    PvDisplayWnd *GetDisplay() { return mDisplay; }

    void UpdateCaptureImage(cv::Mat image, std::string filepath);

    void EnableInterface();

    void Log_FrontCam(QString log);

protected slots:
    void OnConnectButton();
    void OnDisconnectButton();
    void OnStart();
    void OnStop();
    void OnTimer();
    void OnCbnSelchangeMode( int aIndex );

    void OnSaveImagecheck();


protected:

    bool event( QEvent *aEvent );

    void UpdateAcquisitionMode();
    void UpdateAcquisitionModes();

    // UI
    void CreateLayout();
    QLayout *CreateDisplayLayout();
    QLayout *CreateCaptureLayout();
    QLayout *CreateControlsLayout();
    QLayout *CreateLogLayout();

public:
    void Connect( const PvDeviceInfoGEV *aDeviceInfo );
    void Disconnect();

    void SetSaveImageChecked(bool bsave) { mbSaveImage = bsave; }
    bool GetSaveImageChecked() { return mbSaveImage; };

    QCheckBox * GetSaveImageCheckBox() { return mSaveImgCheckBox; }

private:
    PvDeviceGEV mDevice;
    PvDisplayWnd *mDisplay;
    PvStream *mStream;

    QTextEdit *mStatusLine;
    QTimer *mTimer;

    QLabel *mCaptureLabel;
    QLabel *mPlateLabel;

    QComboBox *mModeComboBox;
    QPushButton *mConnectButton;
    QPushButton *mDisconnectButton;
    QPushButton *mPlayButton;
    QPushButton *mStopButton;
    QTextEdit *mLogText;

    QString mTitle;

    FrontCamSource *mFrontCamSource;

    QCheckBox *mSaveImgCheckBox; //VD Trigger test
    bool mbSaveImage;
};

#endif // FRONTCAMWIDGET_H
