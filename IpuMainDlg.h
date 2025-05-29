#ifndef CIPUMAINDLG_H
#define CIPUMAINDLG_H

#pragma once

#include <QMainWindow>
#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QMutex>
#include <QEvent>
#include <QTime>
#include <QTimer>


//class CIpuMainDlg : public QDialog
class CIpuMainDlg : public QMainWindow
{
    Q_OBJECT
public:
    //CIpuMainDlg( QWidget *parent = 0);
    CIpuMainDlg( QWidget *parent = 0, Qt::WindowFlags flags = Qt::WindowFlags() );
    virtual ~CIpuMainDlg();

protected:
    void closeEvent(QCloseEvent *event) override;
    void CreateLayout();


signals:
    void currentChanged(int tabIdx);

protected slots:
    void SetCurrentTab(int nidx);

public:
    int GetCurrentTabIndex();

private:
    QTabWidget * m_pTabWidget;
    QVBoxLayout * m_lTabVLayout;
    QHBoxLayout * m_lTabHLayout;



    int m_nCurrentTabIndex;
};

#endif // CIPUMAINDLG_H
