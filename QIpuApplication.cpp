#include "QIpuApplication.h"
#include "ui_QIpuApplication.h"

QIPUApplication::QIPUApplication(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QIPUApplication)
{
    ui->setupUi(this);
}

QIPUApplication::~QIPUApplication()
{
    delete ui;
}

