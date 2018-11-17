#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Utils/sem.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
private:
    Ui::MainWindow *ui;
    double getData(double time);
    double getData_1(double time);

};

#endif // MAINWINDOW_H
