#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QColor>

#include "color_dialog.hpp"

#include "blink1pattern.h"
#include "blink1input.h"

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
    void closeEvent(QCloseEvent *);

private slots:
    void quit();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void updateBlink1();

    void colorChanged(QColor);

    void onColorDialogChange(QColor);

    void on_buttonRGBcycle_clicked();
    void on_buttonMoodlight_clicked();
    void on_buttonOff_clicked();
    void on_buttonWhite_clicked();
    void on_buttonColorwheel_clicked();
    void on_buttonBusyColorSpot_clicked();

    void on_colorwheel_colorSelected(const QColor &);

private:
    void loadSettings();
    void saveSettings();

    void createActions();
    void createTrayIcon();

    Ui::MainWindow *ui;

    QString m_sSettingsFile;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QTimer* blink1timer;
    hid_device* blink1dev;

    uint8_t mode;
    QColor cc;  // current color
    Color_Dialog colorDialog;

    QMap<QString,Blink1Pattern*> patterns;
    QMap<QString,Blink1Input*> inputs;
};

#endif // MAINWINDOW_H
