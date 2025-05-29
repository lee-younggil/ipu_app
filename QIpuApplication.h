#ifndef QIPUAPPLICATION_H
#define QIPUAPPLICATION_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class QIPUApplication; }
QT_END_NAMESPACE

class QIPUApplication : public QMainWindow
{
    Q_OBJECT

public:
    QIPUApplication(QWidget *parent = nullptr);
    ~QIPUApplication();

private:
    Ui::QIPUApplication *ui;
};
#endif // QIPUAPPLICATION_H
