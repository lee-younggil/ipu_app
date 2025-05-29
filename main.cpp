#include "QIpuApplication.h"
#include <QApplication>
#include <QDebug>

#include "IpuFactory.h"
#include "IpuMainDlg.h"

#include <signal.h>

void SignalHandler( int aSig )
{
    qDebug() << "[SignalHandler] - aSig : " << aSig;
    qApp->quit();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CIpuFactory f;
    CIpuMainDlg * w = f.CreateDlg();

    signal( SIGTERM, SignalHandler );
    signal( SIGINT, SignalHandler );

    w->show();

    return a.exec();
}
