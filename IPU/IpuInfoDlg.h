#ifndef CIPUINFODLG_H
#define CIPUINFODLG_H

#include <QWidget>
#include <QMainWindow>
#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QGroupBox>

class CIpuInfoDlg : public QWidget
{
    Q_OBJECT
public:
    explicit CIpuInfoDlg(QWidget *parent = nullptr);
    ~CIpuInfoDlg();

signals:

protected:
    void CreateLayOut();

    QGroupBox *CreateLogLayout();
    QGroupBox *CreateInfoLayout();

private:
    QTextEdit *mLogText;
    QTextEdit *mInfoText;

    // Menu
    QMenu *mFileMenu;
    QAction *mFileOpenAction;
    QMenu *mFileOpenRecentMenu;
    QAction *mFileSaveAction;
    QAction *mFileSaveAsAction;
    QAction *mFileExitAction;



    QLineEdit *mDeviceKeyEdit;
    QLineEdit *mGroupKeyEdit;
    QLineEdit *mGroupMaskEdit;
    QCheckBox *mScheduledCheck;
    QLabel *mTimestampLabel;
    QLineEdit *mTimestampEdit;
    QCheckBox *mForceACKFlagCheck;
    QPushButton *mSendButton;
    QTextEdit *mAcknowledgementsEdit;

    QString mAppName;

    bool mForceACKFlag;
    // <========================

};

#endif // CIPUINFODLG_H
