#include "IpuInfoDlg.h"

#include <QPainter>
#include <QMessageBox>
#include <QDebug>

CIpuInfoDlg::CIpuInfoDlg(QWidget *parent) : QWidget(parent)
{
    CreateLayOut();
}

CIpuInfoDlg::~CIpuInfoDlg()
{

}

void CIpuInfoDlg::CreateLayOut()
{
    QGroupBox *lLogBox = CreateLogLayout();
    QGroupBox *lInfoBox = CreateInfoLayout();

    QVBoxLayout *lVLayout = new QVBoxLayout();
    lVLayout->addWidget(lLogBox);

    QHBoxLayout *lHLayout = new QHBoxLayout();
    lHLayout->addLayout(lVLayout);
    lHLayout->addWidget(lInfoBox);

    setLayout(lHLayout);
}



//QLayout *CIpuInfoDlg::CreateLogLayout()
QGroupBox *CIpuInfoDlg::CreateLogLayout()
{
    qDebug() << "[CIpuInfoDlg::CreateLogLayout] - ";

    mLogText = new QTextEdit;

    mLogText->setEnabled( true );
    mLogText->setReadOnly( true );
    mLogText->setBackgroundRole( QPalette::Base );
    mLogText->setWordWrapMode( QTextOption::NoWrap );
    mLogText->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    mLogText->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

    QFontMetrics lFM( mLogText->font() );
    //mLogText->setMaximumHeight( lFM.height() * 3 + lFM.lineSpacing() );
    //mLogText->setMinimumWidth(400);
    //mLogText->setMinimumHeight(600);

    QVBoxLayout *lLogLayout = new QVBoxLayout;
    lLogLayout->addWidget( new QLabel( "Info Log" ) );
    lLogLayout->addWidget( mLogText);

    QVBoxLayout *lBoxLayout = new QVBoxLayout;
    lBoxLayout->addLayout( lBoxLayout );

    QGroupBox *lLogBox = new QGroupBox;
    lLogBox->setLayout(lLogLayout);
    lLogBox->setMinimumWidth(400);
    lLogBox->setMinimumHeight(600);

    return lLogBox;
}

//QLayout *CIpuInfoDlg::CreateInfoLayout()
QGroupBox *CIpuInfoDlg::CreateInfoLayout()
{
    qDebug() << "[CIpuInfoDlg::CreateInfoLayout] - ";

    mInfoText = new QTextEdit;

    //mLogDisplay->setReadOnly( true );
    mInfoText->setEnabled( true );
    mInfoText->setReadOnly( true );
    mInfoText->setBackgroundRole( QPalette::Base );
    mInfoText->setWordWrapMode( QTextOption::NoWrap );
    mInfoText->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    mInfoText->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

    QFontMetrics lFM( mInfoText->font() );
    //mInfoText->setMaximumHeight( lFM.height() * 3 + lFM.lineSpacing() );
    //mInfoText->setMinimumWidth(400);
    //mInfoText->setMinimumHeight(600);

    QVBoxLayout *lInfoLayout = new QVBoxLayout;
    lInfoLayout->addWidget( new QLabel( "시스템 정보" ) );
    lInfoLayout->addWidget( mInfoText );

    QVBoxLayout *lBoxLayout = new QVBoxLayout;
    lBoxLayout->addLayout( lInfoLayout );

    QGroupBox *lLogBox = new QGroupBox;
    lLogBox->setLayout(lBoxLayout);
    lLogBox->setMinimumWidth(400);
    lLogBox->setMinimumHeight(600);

    return lLogBox;
}
