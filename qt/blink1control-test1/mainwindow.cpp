#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fadeMillis = 300;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_scanButton_clicked()
{
    int blink1count = blink1_enumerate();
    blink1dev = blink1_open();
    blink1_fadeToRGB(blink1dev, 1000, 40,40,40 );

    qDebug() << "num blink(1)s found: " << blink1count;

    if( blink1count ) {
        ui->statusLabel->setText("blink(1) connected");
        ui->serialLabel->setText(blink1_getCachedSerial(0));
    }
    else {
        ui->statusLabel->setText("no blink(1) found");
    }
}

void MainWindow::on_redButton_clicked()
{
    blink1_fadeToRGB( blink1dev, fadeMillis, 255,0,0);
}
void MainWindow::on_greenButton_clicked()
{
    blink1_fadeToRGB( blink1dev, fadeMillis, 0,255,0);
}
void MainWindow::on_blueButton_clicked()
{
    blink1_fadeToRGB( blink1dev, fadeMillis, 0,0,255);
}
void MainWindow::on_whiteButton_clicked()
{
    blink1_fadeToRGB( blink1dev, fadeMillis, 255,255,255);
}
void MainWindow::on_offButton_clicked()
{
    blink1_fadeToRGB( blink1dev, fadeMillis, 0,0,0);
}

