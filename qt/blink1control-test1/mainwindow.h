#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "blink1-lib.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:

    void on_scanButton_clicked();
    void on_redButton_clicked();
    void on_blueButton_clicked();
    void on_greenButton_clicked();
    void on_whiteButton_clicked();
    void on_offButton_clicked();

private:
    Ui::MainWindow *ui;

    hid_device* blink1dev;
    int fadeMillis;

};

#endif // MAINWINDOW_H
