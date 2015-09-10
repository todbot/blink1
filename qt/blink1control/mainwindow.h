#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "cursorshapearea.h"
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
#include "emailcurl.h"
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
#include <QShortcut>

#include <QMenu>
#include <QUrlQuery>
#include <QPair>
#include <QDesktopWidget>
#include <QFont>
#include <QFontMetrics>

#include <QErrorMessage> // for error dialog

// maximum number events in the Recent Events list
#define RECENT_EVENTS_MAX 200

class HttpServer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_PROPERTY(int led READ getLed WRITE setLed NOTIFY ledsUpdate)
    Q_PROPERTY(QString activePattern READ getActivePatternName NOTIFY updateActivePatternName)
    Q_PROPERTY(QString blink1 READ getBlinkStatus NOTIFY iftttUpdate)
    Q_PROPERTY(bool isMk2 READ isMk2 NOTIFY deviceUpdate)
    Q_PROPERTY(QString iftttKey READ getIftttKey NOTIFY iftttUpdate)
    Q_PROPERTY(QString blinkKey READ getBlinkKey NOTIFY iftttUpdate)
    Q_PROPERTY(QList<QObject*> getPatternsList READ getPatternsList NOTIFY patternsUpdate)
    Q_PROPERTY(QList<QObject*> getInputsList READ getInputsList NOTIFY inputsUpdate)
    Q_PROPERTY(QList<QObject*> getMailsList READ getMailsList NOTIFY emailsUpdate)
    Q_PROPERTY(QList<QObject*> getHardwareList READ getHardwareList NOTIFY hardwareUpdate)
    Q_PROPERTY(QList<QObject*> getIFTTTList READ getIFTTTList NOTIFY inputsUpdate)
    Q_PROPERTY(QVariantList getRecentEvents READ getRecentEvents NOTIFY recentEventsUpdate)
    Q_PROPERTY(QList<QObject*> getBigButtons READ getBigButtons NOTIFY bigButtonsUpdate)
    Q_PROPERTY(QVariantList getPatternsNames READ getPatternsNames NOTIFY updatePatternsNamesOnUi)
    Q_PROPERTY(QVariantList getBlink1Serials READ getBlink1Serials NOTIFY updateBlink1Serials)
    Q_PROPERTY(int blink1Index MEMBER blink1Index)

    // preference properties
    Q_PROPERTY(bool enableServer MEMBER enableServer NOTIFY prefsUpdate)
    Q_PROPERTY(bool autorun MEMBER autorun NOTIFY prefsUpdate) 
    Q_PROPERTY(bool startMinimized MEMBER startMinimized NOTIFY prefsUpdate) 
    Q_PROPERTY(bool dockIcon MEMBER dockIcon NOTIFY prefsUpdate)
    Q_PROPERTY(bool enableGamma MEMBER enableGamma NOTIFY prefsUpdate)
    Q_PROPERTY(bool logging MEMBER logging NOTIFY prefsUpdate)

    Q_PROPERTY(QString serverHost MEMBER serverHost NOTIFY prefsUpdate)
    Q_PROPERTY(int     serverPort MEMBER serverPort NOTIFY prefsUpdate)
    Q_PROPERTY(QString  proxyType MEMBER proxyType NOTIFY prefsUpdate)
    Q_PROPERTY(QString  proxyHost MEMBER proxyHost NOTIFY prefsUpdate)
    Q_PROPERTY(int      proxyPort MEMBER proxyPort NOTIFY prefsUpdate)
    Q_PROPERTY(QString  proxyUser MEMBER proxyUser NOTIFY prefsUpdate)
    Q_PROPERTY(QString  proxyPass MEMBER proxyPass NOTIFY prefsUpdate)


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    
    void quit();
    void changeMinimizeOption();
    void showNormal();
    void showMinimize();
    void updateBlink1();
    void blink1Blink( QString blink1serial, QString color, int millis );
    void blink1SetColorById( QColor color, int millis, QString blink1serial, int ledn );
    void showAboutDialog();
    void copyToClipboard( QString txt );

    void changeColorFromQml(QColor);

    void on_buttonRGBcycle_clicked();
    void on_buttonMoodlight_clicked();
    void on_buttonOff_clicked();
    void on_buttonWhite_clicked();
    void on_buttonStrobe_clicked();
    void on_buttonColorwheel_clicked();

    void goingToSleep();
    void wakingUp();
    
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
    
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayBigButtonTriggered(QAction* action);
    
    static bool comparePatternsFunction(Blink1Pattern *bi1,Blink1Pattern *bi2){
        return bi1->date()>bi2->date();
    }
    static bool compareInputsFunction(Blink1Input *bi1,Blink1Input *bi2){
        return bi1->name()<bi2->name();
        //return bi1->date()<bi2->date();
    }

    QList<QObject*> getPatternsList();
    QList<QObject*> getMailsList();
    QList<QObject*> getHardwareList();
    QList<QObject*> getInputsList();
    QList<QObject*> getIFTTTList();
    QVariantList getRecentEvents();  // qml
    QVariantList getPatternsNames(); // qml
    QVariantList getBlink1Serials(); // qml
    QList<QObject*> getBigButtons();

    void blink1CloseAll();
    void refreshBlink1State();

    void setColorToBlinkAndChangeActivePatternName(QColor,QString,int f=100);
    void setColorToBlink(QColor,int f);
    void setColorToBlinkN(QColor c, int fademillis, int ledn);
    void removeRecentEvent(int idx);
    void removeAllRecentEvents();

    void removePattern(QString key);
    void removeInput(QString key,bool update=false);
    void addColorAndTimeToPattern(QString pname,QString color,double time);
    void addNewPattern(QColor col, double time);

    void updateInputsList();
    void setPatternNameToInput(QString name, QString pn);
    void setFreqToInput(QString name, int freq);

    QString getActivePatternName();
    QString getBlinkStatus();
    QString getIftttKey();
    QString getBlinkKey();
    void setBlink1Index( QString blink1indexstr );

    void playOrStopPattern(QString name);
    void playPattern(QString name);
    void stopPattern(QString name);

    void removeColorAndTimeFromPattern(QString name,int idx);
    void changePatternRepeats(QString name);
    void changePatternRepeatsTo(QString name, int to);
    void changePatternName(QString oldName,QString newName);
    int getLedFromPattern(QString name, int idx);
    void setLedToPattern(QString name,int idx, int led);
    void changePatternReadOnly(QString s, bool ro);
    void copyPattern(QString name);

    void addNewBigButton(QString name, QColor col);
    void updateBigButtonColor(int idx, QColor col);
    void updateBigButtonName(int idx, QString name);
    void updateBigButtonPatternName(int idx, QString name);
    void updateBigButtonLed(int idx, int l);
    void playBigButton(QString name);
    void playBigButton(int idx);
    void removeBigButton2(int idx);
    void moveBigButton2(int oldidx, int idx);
    void updateBigButtons();
    void editColorAndTimeInPattern(QString pname,QString color,double time, int index);

    void updateInputsArg1(QString name, QString arg1);
    void updateInputsType(QString name, QString type);
    void updateInputsPatternName(QString name, QString pn);
    void createNewIFTTTInput();
    void createNewInput();    
    void updatePatternsList();
    void changeInputName(QString oldName,QString newName);

    void updatePreferences();
    void changeAutorunOption();
    void showhideDockIcon();    
    void checkInput(QString key);
    void startStopServer();
    void changeLed(int l);
    bool isMk2();
    QString selectFile(QString name);

    void setLed(int l);
    int getLed();

    bool mac();

    void addRecentEvent(int date, QString name, QString from);


    void viewerActiveChanged();
    void onApplicationStateChange(Qt::ApplicationState state);
    /*
    void changeEvent(QEvent * event);
    void viewerVisibilityChanged(QWindow::Visibility visibility);
    void viewerWindowStateChanged(Qt::WindowState state);
    void viewerClosing(QQuickCloseEvent*event);
    void viewerChangeEvent(QEvent* event);
    void viewerStatusChanged(QQuickView::Status status);

    void viewerClosingSlot(QQuickCloseEvent*);
    void viewerVisibleChangedSlot(bool);
    void markViewerAsClosing();
    */

    void add_new_mail(QString name,int type, QString server, QString login, QString passwd, int port, bool ssl, int result, QString parser);
    void edit_mail(QString oldname, QString name,int type, QString server, QString login, QString passwd, int port, bool ssl, int result, QString parser);
    void remove_email(QString name,bool update=false);
    void updateMail();
    void setFreqToEmail(QString name,int freq);
    void setPatternNameToEmail(QString name, QString pn);
    void checkMail(QString name);
    void markEmailEditing(QString s,bool e);

    QString getHostId();
    void setHostId(QString hostId);
    bool checkHex( QString newText);

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

    //bool checkIfCorrectPositionX(int x);
    //bool checkIfCorrectPositionY(int y,int bar);
    int checkWordWidth(QString s,int size);
    void changeColorOnVirtualBlink(QColor,double);
    bool checkIfColorIsTooBright(QString);
    bool checkIfColorIsTooDark(QString);

    void changePatternNameInAllMonitors(QString from,QString to);
    QVariantList getFullColorsFromPattern(QString patternName);

    // FUNCTIONS FOR HTTP SERVER
    void regenerateBlink1Id();
    QJsonArray getCachedBlinkId();
    QColor getCurrentColor();
    QMap<QString,Blink1Pattern*> getFullPatternList();
    QMap<QString,Blink1Input*> getFullInputList();
    bool addNewPatternFromPatternStr(QString name, QString patternStr);
    void startOrStopLogging(bool);
    bool getLogging();
    QString getLogFileName();
    QString getSettingsFileName();
    void updateColorsOnBigButtons2List();

    void setStartupPattern( QString patternName );
    
    void settingsExport( QString filepath );
    void settingsImport( QString filepath );
    void osFixes();

 private:
    QtQuick2ApplicationViewer viewer;

    void settingsLoad();
    void settingsLoad( QSettings & settings );
    void settingsSave(); 
    void settingsSave( QSettings & settings );

    void createActions();
    void createTrayIcon();

    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QAction *blinkIdAction;
    QAction *iftttKeyAction;
    QAction *blinkStatusAction;
    QAction *autorunAction;
    QAction *dockIconAction;
    QAction *settingAction;
    QAction *offAction;
    QAction *serverAction;
    QAction *aboutAction;
    QActionGroup* bigButtonActions;
    
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QShortcut* resetAlertsShortcut;

    QTimer* blink1timer;
    blink1_device* blink1dev;
    //bool blink1Refreshing;
    int refreshCounter;

    uint8_t mode;
    QColor cc;

    QList<QString> recentEvents;
    QList<BigButtons*> bigButtons2;

    QMap<QString,Blink1Pattern*> patterns;
    QMap<QString,Blink1Input*> inputs;
    QMap<QString,Email*> emails;
    QMap<QString,HardwareMonitor*> hardwareMonitors;
    QMapIterator<QString, Blink1Input*> *inputsIterator;
    QMapIterator<QString, Email*> *emailsIterator;
    QMapIterator<QString, HardwareMonitor*> *hardwaresIterator;

    int duplicateCounter;
    QString activePatternName;

    QString blinkStatus;
    QString blink1Id;
    QString iftttKey;
    int rgbCycle;
    int rgbCounter;
    int fadeSpeed;

    int led;

    QTimer *inputsTimer;
    int inputTimerCounter;
    qint64 lastIftttDate;    // timestamp of the last IFTTT event we received
    bool isIftttChecked;  // FIXME: only check IFTTT once, even tho multiple DataInputs for it

    HttpServer *httpserver;

    bool sleepytime;
    bool refreshBlink1s;
    int blink1Index;
    QString getTimeFromInt(int t);
    bool mk2;
    bool logging;
    QFile *logFile;
    QTextStream *logstream;
    bool fromPattern;

    bool autorun;
    bool dockIcon;
    bool startMinimized;
    bool enableServer;
    bool enableGamma;
    bool firstRun;
    
    QString serverHost;
    int     serverPort; 

    QString proxyType;
    QString proxyHost;
    int     proxyPort;
    QString proxyUser;
    QString proxyPass;



signals:
    void patternsUpdate();
    void updatePatternsNamesOnUi();
    void updateBlink1Serials();

    void inputsUpdate();
    void updateActivePatternName();
    void recentEventsUpdate();
    void bigButtonsUpdate();
    void emailsUpdate();
    void hardwareUpdate();    
    void iftttUpdate();
    void ledsUpdate();
    void deviceUpdate();
    void prefsUpdate();
    
private slots:
    void updateInputs();
    void deleteDataInput(DataInput *dI);
    void runPattern(QString pattname, bool fromQml);
    void setColorFromDataInput(QColor);
    void checkIfttt(QString);
};

#endif // MAINWINDOW_H
