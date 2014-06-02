#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "Cursorshapearea.h"
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QColor>

#include "blink1pattern.h"
#include "blink1input.h"

#include "blink1-lib.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QTextStream>
#include "qtquick2applicationviewer.h"
#include <QQmlContext>
#include <QtQml>

#include "datainput.h"

#include "bigbuttons.h"
#include "email.h"
#include "hardwaremonitor.h"

#include <time.h>
#include <stdlib.h>

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>

#include <QFileDialog>

#include <QCloseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QAction>

#include <QMenu>
#include <QUrlQuery>
#include <QPair>
#include <QDesktopWidget>
#include <QFont>
#include <QFontMetrics>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int led READ getLed WRITE setLed NOTIFY ledsUpdate)
    Q_PROPERTY(int getSize READ getSize NOTIFY patternsUpdate)
    Q_PROPERTY(QString activePattern READ getActivePatternName NOTIFY updatePatternName)
    Q_PROPERTY(QString blink1 READ getBlinkStatus NOTIFY iftttUpdate)
    Q_PROPERTY(bool isMk2 READ getIsMk2 NOTIFY deviceUpdate)
    Q_PROPERTY(QString iftttKey READ getIftttKey NOTIFY iftttUpdate)
    Q_PROPERTY(QString blinkKey READ getBlinkKey NOTIFY iftttUpdate)
    Q_PROPERTY(QList<QObject*> getList READ getList NOTIFY patternsUpdate)
    Q_PROPERTY(QList<QObject*> getInputsList READ getInputsList NOTIFY inputsUpdate)
    Q_PROPERTY(QList<QObject*> getMailsList READ getMailsList NOTIFY emailsUpdate)
    Q_PROPERTY(QList<QObject*> getHardwareList READ getHardwareList NOTIFY hardwareUpdate)
    Q_PROPERTY(QList<QObject*> getIFTTTList READ getIFTTTList NOTIFY inputsUpdate)
    Q_PROPERTY(QVariantList getRecentEvents READ getRecentEvents NOTIFY recentEventsUpdate)
    Q_PROPERTY(QList<QObject*> getBigButtons READ getBigButtons NOTIFY bigButtonsUpdate)
    Q_PROPERTY(QVariantList getPatternsNames READ getPatternsNames NOTIFY patternsUpdate2)

    Q_PROPERTY(QString valueRet READ valueRet WRITE setvalueRet NOTIFY changeValRef)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);
    QString valueRet() const;
    void setvalueRet(const QString& name);
    bool closing;
public slots:
    void quit();
    void minimalize();
    void normal();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void updateBlink1();

    void colorChanged(QColor);

    void onColorDialogChange(QColor);

    void on_buttonRGBcycle_clicked();
    void on_buttonMoodlight_clicked();
    void on_buttonOff_clicked();
    void on_buttonWhite_clicked();
    void on_buttonStrobe_clicked();
    void on_buttonColorwheel_clicked();
    void on_buttonBusyColorSpot_clicked();

    void slotCheckTimeout();
    void replyFinished(QNetworkReply*);
    int getSize();

    static bool comparePatternsFunction(Blink1Pattern *bi1,Blink1Pattern *bi2){
        return bi1->date()>bi2->date();
    }
    static bool compareInputsFunction(Blink1Input *bi1,Blink1Input *bi2){
        return bi1->date()<bi2->date();
    }

    QList<QObject*> getList();
    QList<QObject*> getMailsList();
    QList<QObject*> getHardwareList();
    QList<QObject*> getInputsList();
    QList<QObject*> getIFTTTList();
    QVariantList getRecentEvents();
    QVariantList getPatternsNames();
    QList<QObject*> getBigButtons();

    void setColorToBlink(QColor,QString,int f=100);
    void setColorToBlink2(QColor,int f);
    void removeRecentEvent(int idx);
    void removeAllRecentEvents();

    void removePattern(QString key);
    void removeInputAndPattern(QString key);
    void removeInput(QString key,bool update=false);
    void new_input_and_pattern(QString name,QString type,QString rule,QString pname,int repeats);
    void edit_input_and_pattern(QString name,QString type,QString rule,QString pname,int repeats,QString old_name);
    void addColorAndTimeToPattern(QString pname,QString color,double time);
    void addNewPattern(QColor col, double time);

    void update();
    void setPatternNameToInput(QString name, QString pn);
    void setFreqToInput(QString name, int freq);

    QVariantList getPattern(QString key);
    QVariantList getPatternTimes(QString key);
    int getPatternColorSize(QString key);
    int getPatternRepeats(QString key);
    QString getActivePatternName();
    QString getBlinkStatus();
    QString getIftttKey();
    QString getBlinkKey();
    void playPattern(QString name);
    void playPattern2(QString name);

    void stopPattern(QString name);

    void removeColorAndTimeFromPattern(QString name,int idx);
    void changeRepeats(QString name);
    void changeRepeatsTo(QString name, int to);

    void addNewBigButton(QString name, QColor col);
    void updateBigButtonColor(int idx, QColor col);
    void updateBigButtonName(int idx, QString name);
    void updateBigButtonPatternName(int idx, QString name);
    void updateBigButtonLed(int idx, int l);
    void playBigButton(int idx);
    void updateInputsArg1(QString name, QString arg1);
    void updateInputsType(QString name, QString type);
    void updateInputsPatternName(QString name, QString pn);
    void createNewIFTTTInput();
    void createNewInput();
    void changePatternName(QString oldName,QString newName);
    void update2();

    void changeInputName(QString oldName,QString newName);
    void removeBigButton2(int idx);
    void updateBigButtons();
    void setAutorun();
    void showhideDockIcon();
    void minButton();
    void checkInput(QString key);
    // unused? void checkInput2(Blink1Input *in,QTcpSocket *client);
    void startStopServer();
    void acceptConnection();
    void startRead();
    void discardClient();
    void changeLed(int l);
    bool getIsMk2();
    QString selectFile(QString name);
    void setLed(int l);
    int getLed();
    int getLedFromPattern(QString name, int idx);
    void setLedToPattern(QString name,int idx, int led);
    bool mac();

    void fromQmlRequest(QString type, QString rule);
    void addRecentEvent(int date, QString name, QString from);

    void editColorAndTimeInPattern(QString pname,QString color,double time, int index);
    void qu(QQuickCloseEvent*);
    void qu2(bool);
    void mark();
    void add_new_mail(QString name,int type, QString server, QString login, QString passwd, int port, bool ssl, int result, QString parser);
    void edit_mail(QString oldname, QString name,int type, QString server, QString login, QString passwd, int port, bool ssl, int result, QString parser);
    void remove_email(QString name,bool update=false);
    void updateMail();
    void setFreqToEmail(QString name,int freq);
    void setPatternNameToEmail(QString name, QString pn);
    void checkMail(QString name);
    QString getHostId();
    void setHostId(QString hostId);
    bool checkHex( QString newText);
    void markEditing(QString s,bool e);
    void changePatternReadOnly(QString s, bool ro);
    void copyPattern(QString name);

    void remove_hardwareMonitor(QString name,bool update=false);
    void updateHardware();
    void setFreqToHardwareMonitor(QString name,int freq);
    void setPatternNameToHardwareMonitor(QString name, QString pn);

    void checkHardwareMonitor(QString name);

    void add_new_hardwaremonitor(QString name,int type,int lvl, int action, int role);
    void edit_hardwaremonitor(QString oldname,QString name,int type,int lvl, int action, int role);
    void markHardwareEditing(QString s,bool e);
    void addToLog(QString txt);
    void resetAlertsOption();
    bool checkIfCorrectPositionX(int x);
    bool checkIfCorrectPositionY(int y,int bar);
    int checkWordWidth(QString s,int size);
    void changeColorOnVirtualBlink(QColor);
    bool checkIfColorIsTooBright(QString);
    bool checkIfColorIsTooDark(QString);
private:
    QNetworkAccessManager *nam;
    QtQuick2ApplicationViewer viewer;

    void loadSettings();
    void saveSettings();

    void createActions();
    void createTrayIcon();

    QString m_sSettingsFile;

    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QAction *blinkIdAction;
    QAction *iftttKeyAction;
    QAction *blinkStatusAction;
    QAction *autorunAction;
    QAction *dockIconAction;
    QAction *settingAction;
    QAction *alertsAction;
    QAction *serverAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QTimer* blink1timer, *checkTimer;
    blink1_device* blink1dev;

    uint8_t mode;
    QColor cc;  // current color

    QMap<QString,Blink1Pattern*> patterns;
    QMap<QString,Blink1Input*> inputs;
    QMap<QString,Email*> emails;
    QMap<QString,HardwareMonitor*> hardwareMonitors;
    QMapIterator<QString, Blink1Input*> *it;
    QMapIterator<QString, Email*> *it2;
    QMapIterator<QString, HardwareMonitor*> *it3;
    int counter;
    QString activePatternName;
    int inputsAmount;
    QString blinkStatus;
    QString blinkKey;
    QString iftttKey;
    int rgbCycle;
    int rgbCounter;
    int fadeSpeed;
    int led;
    QList<QString> recentEvents;
    QList<BigButtons*> bigButtons2;
    QTcpServer server;
    QList<QTcpSocket *> clientConnections;
    QString mvalueRet;

    QTimer *inputsTimer;
    int inputTimerCounter;
    int dataInputCount;
    bool isIftttChecked;
    bool autorun;
    bool dockIcon;
    bool startmin;
    bool enableServer;
    QString getTimeFromInt(int t);
    bool mk2;
    bool logging;
    QFile *logFile;
    QTextStream *out;
    bool fromPattern;
signals:
    void patternsUpdate();
    void inputsUpdate();
    void updatePatternName();
    void recentEventsUpdate();
    void bigButtonsUpdate();
    void emailsUpdate();
    void hardwareUpdate();
    void changeValRef();
    void patternsUpdate2();
    void iftttUpdate();
    void ledsUpdate();
    void deviceUpdate();
private slots:
    void updateInputs();
    void deleteDataInput(DataInput *dI);
    void runPattern(QString, bool);
    void setColor(QColor);
    void setVRet(QString);
    void checkIfttt(QString);
    bool checkIfColor(QString);
    bool checkIfNumber(QString,int);
    bool checkIfPatternsStr(QString);
};

#endif // MAINWINDOW_H
