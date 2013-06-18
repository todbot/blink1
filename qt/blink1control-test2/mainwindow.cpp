#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QTimer>
#include <QPropertyAnimation>
#include <QDebug>

//#include <QVariant>
//#include <QObjectHelper>
#include <QJsonObject>
#include <QJsonDocument>

enum {
    NONE = 0,
    RGBSET,
    ON,
    OFF,
    RANDOM,
    RGBCYCLE,
    MOODLIGHT,
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "appDirPath: " << QApplication::applicationDirPath();

    loadSettings();

    cc = QColor(255,0,0);
    ui->colorwheel->setColor( cc );

    QIcon ico = QIcon(":/images/blink1-icon0.png");
    setWindowIcon(ico);

    createActions();
    createTrayIcon();

    trayIcon->setIcon( ico);
    trayIcon->setToolTip("Test");
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();

    blink1timer = new QTimer(this);
    connect(blink1timer, SIGNAL(timeout()), this, SLOT(updateBlink1()));
    blink1timer->start(1000);

    blink1_disableDegamma();  // for mk2 only

    int n = blink1_enumerate();
    blink1dev =  blink1_open();
    blink1_fadeToRGB(blink1dev, 1000, 40,40,40 );

    qDebug() << "num blink(1)s found: " << n;

    if( n ) {
        ui->blink1Status->setText("blink(1) connected");
        char ser[10];
        char iftttkey[20];
        sprintf(ser,"%s",blink1_getCachedSerial(0));
        sprintf(iftttkey, "%s%s",ser,ser);
        ui->blink1SerialNumber->setText(ser);
        ui->blink1IftttKey->setText(iftttkey);
        saveSettings();
    } else {
        ui->blink1Status->setText("no blink(1) found");
    }

    mode=RGBSET;
    updateBlink1();
}

MainWindow::~MainWindow()
{
    qDebug() << "destructor";
    //blink1_fadeToRGB(blink1dev, 0, 0,0,0 );
    //blink1_close(blink1dev);

    delete ui;
}

void MainWindow::quit()
{
    blink1_fadeToRGB(blink1dev, 0, 0,0,0 );
    blink1_close(blink1dev);

    QList<QString> pattnames = patterns.keys();
    foreach (QString nm, pattnames ) {
        Blink1Pattern*  patt = patterns.value(nm);
        qDebug() << "patt name: " << patt->name();
        QJsonObject obj = patt->toJson();
        QJsonDocument doc;
        doc.setObject(obj);
        qDebug() << "toJson: " << doc.toJson();
    }

    qApp->quit();
}

void MainWindow::loadSettings()
{
    QSettings settings("ThingM", "Blink1ControlQt");
    QString sText = settings.value("text", "").toString();
    ui->blink1IftttKey->setText(sText);

    for( int i=0; i<5; i++) {
        QString name = QString("pattern-%1-%2").arg(i).arg("abc");
        Blink1Pattern* bp = new Blink1Pattern(); //name );
        bp->setName( name );
        bp->setPlaycount(i*3);
        patterns.insert( name, bp );
    }

    /* test badly formatted QJsonObject
    Blink1Pattern* bp = new Blink1Pattern();
    QJsonObject obj = bp->toJson();
    obj.remove("playcount");
    obj.remove("name");
    Blink1Pattern* bp2 = new Blink1Pattern();
    bp2->fromJson(obj);
    QJsonObject obj2 = bp2->toJson();

    QJsonDocument doc;
    doc.setObject(obj2);
    qDebug() << "toJson2: " << doc.toJson();
*/
    /*
    patterns = settings.value("patterns",0);
    if( patterns = NULL ) {
    }
    */
}

void MainWindow::saveSettings()
{
    QSettings settings("ThingM", "Blink1ControlQt");
    QString sText = ui->blink1IftttKey->text();
    settings.setValue("text", sText);
    //settings.setValue("patterns", patterns);
}

void MainWindow::updateBlink1()
{
    bool setBlink1 = false;
    uint16_t fadeSpeed = 200;

    //qDebug() << "updateBlink1!";
    if( mode == RANDOM ) {
        cc = QColor( rand() % 255, rand() % 255, rand() % 255 );
        fadeSpeed = 200;
        setBlink1 = true;
    }
    else if( mode == RGBCYCLE ) {
        if( cc == QColor(255,0,0) ) {
            cc = QColor(0,255,0);
        } else if( cc == QColor(0,255,0) ) {
            cc = QColor(0,0,255);
        } else {
            cc = QColor(255,0,0);
        }
        setBlink1 = true;
    }
    else if( mode == ON ) {
        cc = QColor(255,255,255); //cr = cg = cb = 255;
        setBlink1 = true;
        mode = NONE;
        blink1timer->setInterval(1000);
    }
    else if( mode == OFF ) {
        cc = QColor(0,0,0); //cr = cg = cb = 0;
        setBlink1 = true;
        mode = NONE;
        blink1timer->setInterval(1000);
    }
    else if( mode == RGBSET ) {
        mode = NONE;
        setBlink1 = true;
    }

    if( setBlink1 ) {
        uint8_t cr = cc.red();  uint8_t cg = cc.green();  uint8_t cb = cc.blue();
        blink1_fadeToRGB( blink1dev, fadeSpeed, cc.red(), cc.green(), cc.blue() );

        QString stylestr = QString("background: rgb(%1,%2,%3)").arg(cr).arg(cg).arg(cb);
        ui->virtualBlink1->setStyleSheet(stylestr);

        QString cname = QString("rgb(%1,%2,%3)").arg(cr).arg(cg).arg(cb);
        ui->colorpreview->setColor(cc);
        ui->colorHexText->setText(cname);

        ui->colorwheel->setColor(cc);
        /*
        QPropertyAnimation *animation = new QPropertyAnimation(ui->colorwheel, "color");
        animation->setDuration( blink1timer->interval() );
        animation->setStartValue(ui->colorwheel->color());
        animation->setEndValue(cc);
        //animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->setEasingCurve(QEasingCurve::Linear);
        animation->start();
        */
    }
}

void MainWindow::colorChanged(QColor c)
{
    ui->buttonColorwheel->setChecked(true);

    cc = c; // cr = c.red(); cg = c.green(); cb = c.blue();
    mode=RGBSET;
    updateBlink1();
}

void MainWindow::on_colorwheel_colorSelected(const QColor &c)
{
    colorChanged(c);
}

//systray / statusbar stuff

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        //iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        //showMessage();
        break;
    default:
        ;
    }
}

void MainWindow::messageClicked()
{
    QMessageBox::information(0, tr("Systray"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}


void MainWindow::on_buttonRGBcycle_clicked()
{
    mode = RGBCYCLE;
    //blink1timer->setInterval(300);
    updateBlink1();
}

void MainWindow::on_buttonMoodlight_clicked()
{
    mode = MOODLIGHT;
    updateBlink1();
}

void MainWindow::on_buttonOff_clicked()
{
    mode = OFF;
    updateBlink1();
}

void MainWindow::on_buttonWhite_clicked()
{
    mode = ON;
    updateBlink1();
}

void MainWindow::on_buttonColorwheel_clicked()
{
    mode = RGBSET;
    updateBlink1();
}

void MainWindow::onColorDialogChange(QColor qc)
{
    //qDebug() << "color: " << qc;
    QString cname = QString("background: rgb(%1,%2,%3)").arg(qc.red()).arg(qc.green()).arg(qc.blue());
    ui->buttonBusyColorSpot->setStyleSheet(cname);
    ui->buttonBusyColorSpot->setText(qc.name());
    colorChanged(qc);

}

void MainWindow::on_buttonBusyColorSpot_clicked()
{
    qDebug() << "click!";
    colorDialog.setColor( cc );
    connect(&colorDialog, SIGNAL(colorChanged(QColor)), this, SLOT(onColorDialogChange(QColor)));
    colorDialog.show();
   // colorDialog.

}

