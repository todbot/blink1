#include "mainwindow.h"
#include "httpserver.h"

#include <QSettings>
#include <QTimer>
#include <QPropertyAnimation>
#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QSslConfiguration>
#include <QSsl>
#include <QSslError>

#include <QClipboard>

#include <QMessageBox>

#include "osxFixes-c.h"

// FIXME: this must be set to 5000 because of hard-coded values in QML by marcin/milo
#define updateInputsMillis 5000

// set to true to honor menu item mnemonics on OS X
// see: http://qt-project.org/doc/qt-5/exportedfunctions.html
//#ifdef Q_...
void qt_set_sequence_auto_mnemonic(bool);
//#endif 

// allows us to set a OS X Dock menu
// see: http://stackoverflow.com/questions/9334339/qt-add-qaction-menu-items-to-dock-icon-mac
#ifdef Q_OS_MAC
extern void qt_mac_set_dock_menu(QMenu *);
#endif

// globals to hold devices in use
static blink1_device* blink1devs[16];
static int blink1devcount;

enum {
    NONE = 0,
    RGBSET,
    ON,
    OFF,
    RANDOM,
    RGBCYCLE,
    MOODLIGHT,
    STROBE
};


void noisyFailureMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

//
void MainWindow::osFixes()
{
    qDebug() << "running osFixes";
#ifdef Q_OS_WIN
    // to capture Windows power change (sleep/wake) on Windows
    qApp->installNativeEventFilter(this);
#endif

#ifdef Q_OS_MAC
    // 
    installOSXSleepWakeNotifiers(this); //, this);
    
    // FIXME: the below is lame, but it appears putting this in Info.plist
    // or in mainBundle dict doesn't work.
    // and appears to require app restart to take effect
    QProcess cmd;
    cmd.start("defaults write com.thingm.Blink1Control NSAppSleepDisabled -bool YES");
    cmd.waitForFinished();

#endif
}

//
//
//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    qt_set_sequence_auto_mnemonic( true );
    fromPattern=false;
    mk2=true;
    blinkStatus="";
    iftttKey="";
    blink1Id="none";
    autorun=false;
    dockIcon=true;
    startMinimized = false;
    enableServer=false;
    enableGamma=false;
    firstRun=true;
    logging=false;
    srand(0);
    logFile=NULL;
    logstream=NULL;
    mode = NONE;
    duplicateCounter=0; // FIXME: bad name for this var
    refreshCounter = 0;

    osFixes();
    sleepytime = false;

    // hush errors "QSslSocket: cannot resolve SSL_set_psk_client_callback"
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    //QErrorMessage::qtHandler(); // install handler that turns qWarning() into dialogs
    //qInstallMessageHandler(noisyFailureMessageHandler);
        
    httpserver = new HttpServer();
    httpserver->setController(this);
    connect( httpserver, SIGNAL(blink1SetColorById(QColor,int,QString,int)), 
             this,         SLOT(blink1SetColorById(QColor,int,QString,int)) );

    blink1_enumerate();

    settingsLoad();

    QIcon ico = QIcon(":/images/blink1-icon0.png");
    QIcon icobw = QIcon(":/images/blink1-icon0-bw.png");
    // This causes Mac menubar to change but not app icon.
    // (Mac app icon is specified in qmake .pro with ICON var)

    setWindowIcon( ico );
    createActions();
    createTrayIcon();
    trayIcon->setIcon( mac() ? icobw : ico );
    trayIcon->show();

    /*
    // test for shift key held-down at startup to enable logging
    if( QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) == true ) {
        qDebug() << "SHIFT key on start, turning on logging";
        logging = true;
        startOrStopLogging( logging );
    }
    */    
    activePatternName="";

    cc = QColor(0,0,0);

    if( firstRun ) {
        trayIcon->showMessage("Blink1Control running",
                              (mac() ? "Click menubar icon for settings & options" :
                               "Click tray icon for settings, right-click for options") );
        cc = QColor(100,100,100);  // initial color
        firstRun = false;
    }

    blink1timer= new QTimer(this);
    blink1timer->stop();  // errant timer causing extra updateBlink1() calls?
    connect(blink1timer, SIGNAL(timeout()), this, SLOT(updateBlink1()));

    refreshBlink1s = true;
    refreshBlink1State();
    updateBlink1();

    // load help file index.html to text property of QML
    //QString currPath = QDir::currentPath();
    QString appDirPath = QCoreApplication::applicationDirPath();
    //fprintf( stderr, "currPath: %s\nappDirPath: %s\n", qPrintable(currPath),qPrintable(appDirPath));
    QString helpfilepath = appDirPath;
    if( mac() ) helpfilepath += "/../Resources/help/help/index.html";  // FIXME: better way?
    else        helpfilepath += "/help/help/index.html";
    QFile f(helpfilepath);
    QString helpTextStr = "<b> Help file not file </b>";
    if( f.open(QFile::ReadOnly | QFile::Text) ) { 
        QTextStream in(&f);
        helpTextStr = in.readAll();
        f.close();
    }
    viewer.rootContext()->setContextProperty("helpTextString", helpTextStr);

    // set up QML viewer
    qmlRegisterType<QsltCursorShapeArea>("CursorTools", 1, 0, "CursorShapeArea");
    viewer.rootContext()->setContextProperty("mw", this);
    viewer.setMainQmlFile(QStringLiteral("qml/qml/main.qml"));
    //viewer.setFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    viewer.setFlags( Qt::Window ); //| Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    viewer.setMinimumHeight(717); // for bg-new.jpg
    viewer.setMaximumHeight(717);
    viewer.setMinimumWidth(1185);
    viewer.setMaximumWidth(1185);
    viewer.setTitle("Blink(1) Control");
 
    inputTimerCounter = 0;
    inputsTimer = new QTimer(this);
    inputsTimer->singleShot( updateInputsMillis, this, SLOT(updateInputs()));
    isIftttChecked = false;

    if(startMinimized){
        viewer.showMinimized();
        viewer.hide();
    }else{
        viewer.showExpanded();
    }
    startStopServer();
    led=0;
    emit ledsUpdate();
    emit deviceUpdate();

    setColorToBlink(cc,400);  // give a default non-black color to let people know it works

    connect( &viewer, SIGNAL(activeChanged()),this,SLOT(viewerActiveChanged()));
	connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            this, SLOT(onApplicationStateChange(Qt::ApplicationState)));
    qApp->setQuitOnLastWindowClosed(false);  // this makes close button not quit qpp

    // some keyboard shortcut experiments that seem to ALL not work
    //alertsAction->setShortcut(Qt::Key_R | Qt::CTRL);
    //alertsAction->setShortcutContext( Qt::ApplicationShortcut ); 
    //resetAlertsShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R),this); //, this, SLOT(resetAlertsOption()));
    //resetAlertsShortcut = new QShortcut( QKeySequence(Qt::CTRL+Qt::Key_R), viewer.rootObject() );
    //resetAlertsShortcut->setContext(Qt::ApplicationShortcut);
    //connect( resetAlertsShortcut, SIGNAL(activated()), this, SLOT(resetAlertsOption()));
}

// call when we know we're about to suspend
void MainWindow::goingToSleep()
{
    addToLog("goingToSleep...");
    sleepytime = true;
    on_buttonOff_clicked();
}
// call when we know we've been woken up
void MainWindow::wakingUp()
{
    addToLog("wakingUp...");
    sleepytime = false;
    refreshBlink1s = true;
}

// to capture windows power management (sleep/wake) events
// see: https://msdn.microsoft.com/en-us/library/windows/desktop/aa373247(v=vs.85).aspx
// see: http://vb.mvps.org/articles/vsm20100105.pdf
//bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
#ifdef Q_OS_WIN
bool MainWindow::nativeEventFilter(const QByteArray &, void *message, long* )
{
    //qDebug() << "nativeEvent: "<< message;
    MSG* msg = (MSG*)(message);
    if(msg->message == WM_POWERBROADCAST ) {
        qDebug() << "WM_POWERBROADCAST! wParam:"<< msg->wParam;  // 4, 18, 7, 10,10, 10,10
        // (sleep) -> PBT_APMSUSPEND -> PBT_APMRESUMEAUTOMATIC -> PBT_APMRESUMESUSPEND -> PBT_APMPOWERSTATUSCHANGE
        if( msg->wParam == PBT_APMSUSPEND ) {
            goingToSleep();
        }
        else if( msg->wParam == PBT_APMRESUMESUSPEND ) {
            wakingUp();
        }
        //SYSTEM_POWER_STATUS pwr;
        // GetSystemPowerStatus(&pwr);
        //qDebug() << pwr.BatteryFlag << endl;
    }
    return false;
}
#else
bool MainWindow::nativeEventFilter(const QByteArray &, void *, long* )
{
    return false;
}
#endif

// called when window has focus
void MainWindow::viewerActiveChanged() {
    //qDebug() << "viewerActiveChanged: " << viewer.isActive();
    //if( viewer.isActive() ) settingsSave();
}

// called when appllication state has changed
// http://doc.qt.io/qt-5/qt.html#ApplicationState-enum
// (tho only Active & Inactive are issued)
void MainWindow::onApplicationStateChange(Qt::ApplicationState state) {
    qDebug() << "applicationStateChanged: " << state;
}

// close all blink1s
void MainWindow::blink1CloseAll()
{
    for( int i=0; i<blink1devcount; i++) {
        blink1_close( blink1devs[i] );
    }
    memset( blink1devs, 0, sizeof(blink1devs));  // clear local device cache
    blink1devcount = 0;
    blink1dev = NULL;
}

// periodically poll for blink(1) device connect or disconnect
// update internal knowledge about blink(1)s when change is detected
void MainWindow::refreshBlink1State()
{
    if( sleepytime ) {
        addToLog( "refreshBlink1State: sleeping...");
        blink1CloseAll();
        blinkStatus = "blink(1) disconnected";
        emit deviceUpdate();
        return;
    }
   
    if( !refreshBlink1s ) {
        return;
    }
    if( enableGamma ) {
        blink1_enableDegamma();  // FIXME: bad names for these funcs
    } else {
        blink1_disableDegamma();
    }
    refreshCounter++;
    qint64 nowMillis = QDateTime::currentDateTime().toMSecsSinceEpoch();
    addToLog( "refreshBlink1State: --- starting. count:" + QString::number(refreshCounter));
    
    blink1CloseAll();
    
    // open up all blink1s
    blink1devcount = blink1_enumerate();
    for( int i=0; i<blink1devcount; i++ ) { 
        blink1devs[i] = blink1_openById( i );
        addToLog("refreshBlink1State: open & cached blink1 #" +QString::number(i));
    }
    
    addToLog("refreshBlink1State: opening for use blink1Index:" + QString::number(blink1Index,16) );
    int blid = blink1_getCacheIndexById( blink1Index );
    blink1dev = blink1devs[ blid ];

    if( blink1dev ) {
        blinkStatus="blink(1) connected";
        QString serialstr = QString(blink1_getCachedSerial( blink1_getCacheIndexByDev(blink1dev)));
        addToLog("refreshBlink1State: connected, blink1serial='"+serialstr+"'");
        blink1Id = serialstr;
        iftttKey = iftttKey.left(8) + blink1Id;
        mk2 = blink1_isMk2(blink1dev);
        //emit deviceUpdate();
        refreshBlink1s = false;  // only refresh on no blink1s
    } else {
        blinkStatus = "no blink(1) found";
        iftttKey = iftttKey.left(8) + "00000000";
        blink1Id = "none";
        refreshBlink1s = true;
        addToLog("refreshBlink1State: no blink(1) found");
    }

    qint64 elapsedMillis = QDateTime::currentDateTime().toMSecsSinceEpoch() - nowMillis;
    addToLog("refreshBlink1State: --- done. elapsedMillis:"+ QString::number(elapsedMillis) );

    blinkStatusAction->setText(blinkStatus);
    blinkIdAction->setText("blink(1) id: " + blink1Id);
    iftttKeyAction->setText("IFTTT Key: " + iftttKey);

    emit deviceUpdate();
    emit iftttUpdate();
    emit updateBlink1Serials();
}

// set which blink(1) is to be used (defaults to 0, the first blink1)
void MainWindow::setBlink1Index( QString blink1IndexStr )
{
    qDebug() << "blink1IndexStr: "<< blink1IndexStr;
    bool ok;
    blink1Index  = blink1IndexStr.toLong(&ok,16);    // check if blink1indexstr a hex serial number?
    if( !ok ) {
        blink1Index = blink1IndexStr.toLong(&ok,10); // check if its a 0-n index?
        if( !ok ) blink1Index = 0; // if not, default back to 0
    }
    qDebug() << "blink1Index: " << QString::number(blink1Index,16);
}

// synchronously blink the specified blink(1), used in Preferences dialog
// if no blink1 (via no blink1serialstr), return immediately
void MainWindow::blink1Blink( QString blink1serialstr, QString colorstr, int millis )
{
    if( blink1serialstr=="" ) return;
    qDebug() << "blink1Blink1: "<< blink1serialstr;
    bool ok;
    int blink1ser  = blink1serialstr.toLong(&ok,16);
    blink1_device* bdev = (blink1serialstr != blink1Id) ? blink1_openById( blink1ser ) : blink1dev;
    if( bdev ) {
        QColor c = QColor(colorstr);
        blink1_fadeToRGBN(bdev, millis/2, c.red(),c.green(),c.blue(),0);
        blink1_sleep(millis/2);
        blink1_fadeToRGBN(bdev, millis/2, 0,0,0,0);
        blink1_sleep(millis/2);
    }
    if( blink1serialstr != blink1Id ) {
        blink1_close(bdev);
    }
}

//
void MainWindow::blink1SetColorById( QColor color, int millis, QString blink1serialstr, int ledn )
{
    //if( blink1serialstr=="" ) return;
    qDebug() << "*** blink1SetColorById:"<< blink1serialstr<< "color:"<<color << " ms:"<<millis << "blink1Id:"<<blink1Id;
    bool ok;
    int blid  = blink1serialstr.toLong(&ok,16);
    
    blid = blink1_getCacheIndexById( blid );

    bool ismaindev = ( blink1serialstr == blink1Id || blid==0 ) ;
    qDebug() << "blink1SetColorById: blid:"<<blid<< " ismaindev:"<<ismaindev;

    if( ismaindev ) {
        setColorToBlinkN( color,millis, ledn);
        return;
    }
    blink1_device* bdev =  blink1devs[ blid ];

    if( bdev ) {
        qDebug() << "blink1SetColorById: fading";
        blink1_fadeToRGBN(bdev, millis, color.red(),color.green(),color.blue(), ledn);
    }
    else { 
        qDebug() << "blink1SetColorById: null bdev";
    }

    qDebug() << "*** blink1SetColorById: done";
}


// FIXME: wth is this function for?
// and why is it called periodically?
void MainWindow::deleteDataInput(DataInput *dI)
{
    //qDebug()<<"deleteDataInput";
    dI->input->isChecking=false;
    if(dI->input->toDelete)
        delete dI->input;
    delete dI;
}

void MainWindow::runPattern(QString name, bool fromQml)
{
    if(!patterns.value(name))
        return;

    if(!fromQml){  // FIXME: why do we need this boolean?
        if(patterns.contains(name)) patterns.value(name)->play(cc);
    }
}

void MainWindow::setColorFromDataInput(QColor color)
{
    //qDebug() << "setColorFromDataInput";
    cc = color;
    fadeSpeed = 300;
    changeColorOnVirtualBlink(cc, fadeSpeed);
    mode=RGBSET;
    updateBlink1();
    //changeColorFromQml(color);  // what marcin originally had
}


void MainWindow::updateInputs()
{
    //qDebug() << "updateInputs()";
    refreshBlink1State(); // not sure why this is here, but whatever

    QString type;
    QString key;
    delete inputsIterator;
    inputsIterator = new QMapIterator<QString, Blink1Input*>(inputs);
    inputsIterator->toFront();
    while(inputsIterator->hasNext())
    {
        inputsIterator->next();
        key = inputsIterator->key();
        type = inputs[key]->type();

        // FIXME: why are these objects created and deleted for each iteration?
        if( type == "url" ) {
            if(inputs[key]->freqCounter()==0) { //inputTimerCounter == 0)
                qDebug() << "type: URL, freqcounter==0";

                DataInput *dI = new DataInput(this);
                connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
                connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
                connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColorFromDataInput(QColor)));
                connect(dI, SIGNAL(addReceiveEvent(int,QString,QString)), this, SLOT(addRecentEvent(int,QString,QString)));
                dI->setType(type);
                dI->setInput(inputs[key]);
                dI->setIftttKey(iftttKey);
                dI->start();
            }
            inputs[key]->changeFreqCounter();
        }
        else if( type == "ifttt" ) {
            if(inputTimerCounter == 0){
                inputs[key]->updateTime();
                if(!isIftttChecked) {
                    isIftttChecked = true;
                    DataInput *dI = new DataInput(this);
                    connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
                    connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
                    connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColorFromDataInput(QColor)));
                    connect(dI, SIGNAL(iftttToCheck(QString)), this, SLOT(checkIfttt(QString)));
                    connect(dI, SIGNAL(addReceiveEvent(int,QString,QString)), this, SLOT(addRecentEvent(int,QString,QString)));
                    dI->setType(type);
                    dI->setInput(inputs[key]);
                    dI->setIftttKey(iftttKey);
                    dI->start();
                }
            }
        }
        else {
            if(inputs[key]->freqCounter()==0){
                DataInput *dI = new DataInput(this);
                connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
                connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
                connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColorFromDataInput(QColor)));
                connect(dI, SIGNAL(addReceiveEvent(int,QString,QString)), this, SLOT(addRecentEvent(int,QString,QString)));
                dI->setType(type);
                dI->setInput(inputs[key]);
                dI->setIftttKey(iftttKey);
                dI->start();
            }
            inputs[key]->changeFreqCounter();
        }
    }

    delete emailsIterator;
    emailsIterator = new QMapIterator<QString, Email*>(emails);
    emailsIterator->toFront();
    while(emailsIterator->hasNext())
    {
        emailsIterator->next();
        QString mailname=emailsIterator->key();
        //qDebug() << "updateInputs: "<< mailname;
        addToLog("email: "+mailname);
        if(emails.value(mailname)->getFreqCounter()==0){
            emails.value(mailname)->checkMail();
        }
        emails.value(mailname)->changeFreqCounter();
        emails.value(mailname)->updateValues();
    }

    delete hardwaresIterator;
    hardwaresIterator = new QMapIterator<QString, HardwareMonitor*>(hardwareMonitors);
    hardwaresIterator->toFront();
    while(hardwaresIterator->hasNext())
    {
        hardwaresIterator->next();
        QString name=hardwaresIterator->key();
        //qDebug() << "updateInputs: " << name;
        addToLog("hardware:"+name);
        if(hardwareMonitors.value(name)->getFreqCounter()==0){
            hardwareMonitors.value(name)->checkMonitor();
        }
        hardwareMonitors.value(name)->changeFreqCounter();
    }

    isIftttChecked = false;
    inputTimerCounter = (inputTimerCounter + 1) % 3;
    inputsTimer->singleShot(updateInputsMillis, this, SLOT(updateInputs()));

}

/**
 * Parses the /eventsall JSON response from api.thingm.com.
 * 1. Scans through received IFTTT events
 * 2. Look for events newer than lastIftttDate
 * 2. Find events that match configured input rule names.
 * 3. If match and if date is newer than last saved date for that event,
 *      execute the pattern bound to that rule and update event list.
 * 4. Update last
 *
 * @note FIXME: not sure why this isn't in DataInput or Blink1Input
 *
 * @param txt Full JSON response from ifttt api server
 */
void MainWindow::checkIfttt(QString txt)
{
    qDebug() << "checkIfttt(txt) " << txt;
    QJsonDocument respdoc = QJsonDocument::fromJson( txt.toUtf8() );
    QJsonObject respobj = respdoc.object();
    QJsonValue statval = respobj.value( QString("status") );  // should really check status :)
    //qDebug() << "checkIfttt: status: " << statval.toString();

    // uh-oh, this is bad if this happens
    if( !respobj.contains(QString("events")) ) {
        // we need some way to notify user (maybe set arg2 on all iftt items? yuk)
    }

    qint64 recentIftttDate = -1;
    // march through each item of the events array, comparing to each input
    QJsonArray events = respobj.value( QString("events") ).toArray();
    foreach( const QJsonValue& val, events) {
        QJsonObject ev = val.toObject();
        QString evid      = ev["blink1_id"].toString();
        QString evdatestr = ev["date"].toString();
        QString evname    = ev["name"].toString();
        QString evsource  = ev["source"].toString();
        qint64 evdate = evdatestr.toLongLong();
        qDebug() << "evname:"<<evname<<"evdate:"<< evdate << "recent:"<<recentIftttDate << "last:"<<lastIftttDate;

        // is this event newer since last time we checked?
        if( evdate > lastIftttDate && evdate != recentIftttDate ) {
            recentIftttDate = evdate;
            addRecentEvent(evdate, evname+" - "+evsource, "IFTTT");
        }

        foreach ( Blink1Input* input, inputs ) {
            //qDebug() << "blink1input: "<<input->type() <<":"<<input->arg1() <<":"<< input->date();
            // check if this is an IFTTT input and does the event name match?
            // FIXME: name should be same as rule name (aka arg1)
            if( input->type() == "ifttt" ) {
                // is the event newer than our last event, then trigger!
                if( evdate > input->date() ) {
                    qDebug() << "new ifttt event for "<< input->arg1();
                    if( evname == input->arg1() ) {
                        qDebug() << "saving new ifttt event for "<< input->arg1();
                        input->setDate(evdate); // save for next go around
                        input->setArg2(evsource);
                        if( patterns.contains(input->patternName()) ) {
                            patterns.value( input->patternName() )->play(cc);  // FIXME: why is cc being passed?
                        }
                    }
                }
            }
        } // foreach input
    } // foreach event

    if( recentIftttDate > 0 ) {
        lastIftttDate = recentIftttDate;
    }
}

void MainWindow::addRecentEvent(int date, QString name, QString from)
{
    addToLog(name+" via "+from);
    //QString text = getTimeFromInt(QDateTime::currentDateTime().toTime_t()/*date*/) + "-" + name + " via " + from;
    if( date < 1 ) date = QDateTime::currentDateTime().toTime_t();
    QString text = getTimeFromInt(date) + "-" + name + " via " + from;

    recentEvents.prepend(text);
    if( recentEvents.count() > RECENT_EVENTS_MAX ) {
        recentEvents.removeLast();
    }
    addToLog(QString::number(date));

    // uncomment this to add message popups for alerts
    //trayIcon->showMessage("Blink1Control",  text );

    emit recentEventsUpdate();
}

QString MainWindow::getTimeFromInt(int t)
{
    QDateTime dT = QDateTime::fromTime_t(t);
    if(dT.date() == QDate::currentDate())
        return  dT.time().toString("HH:mm:ss");
    else
        return dT.date().toString("dd/MM/yy");
}

MainWindow::~MainWindow()
{
    //qDebug() << "destructor";
    quit();

    delete minimizeAction;
    delete restoreAction;
    delete quitAction;
    delete blinkIdAction;
    delete iftttKeyAction;

    delete trayIcon;
    delete trayIconMenu;

    delete blink1timer;

    if(logging){
        delete logFile;
        delete logstream;
    }
}

void MainWindow::quit()
{

    // secret isQuit bool so quit() knows not to run itself twice
    // (app quit semantics are ill-defined in this weird universe
    // of not running the main window in a MainWindow: damn QML)
    static bool isQuit = false;
    //qDebug() << "quit isQuit:" << isQuit;
    if( isQuit ) return;
    isQuit = true;

    settingsSave();

    if(httpserver->status()){
        httpserver->stop();
        delete httpserver;
    }
    if(logging)
        logFile->close();

    trayIcon->hide(); // can cause Mac crash, see notes: http://qt-project.org/doc/qt-4.8/qsystemtrayicon.html
    foreach (QString name, patterns.keys()) {
       stopPattern(name);
       removePattern(name);
    }
    foreach (QString name, inputs.keys()) {
       removeInput(name);
    }
    foreach (QString name, emails.keys()) {
       remove_email(name);
    }
    foreach (QString name, hardwareMonitors.keys()) {
       remove_hardwareMonitor(name);
    }

    if(blink1dev!=NULL){
        led=0;
        blink1_fadeToRGBN(blink1dev, 0, 0,0,0 ,led);
        blink1_close(blink1dev);
    }

    qApp->quit();
}

// file must not already exist, otherwise it gets overwritten
void MainWindow::settingsExport( QString filepath )
{
    if( filepath.startsWith("file:") ) {
        QUrl furl = QUrl( filepath );
        filepath = furl.toLocalFile();
    }
    addToLog("settingsExport: " + filepath);
    QSettings settings( filepath, QSettings::IniFormat );
    settingsSave( settings );
}

void MainWindow::settingsSave()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ThingM", "Blink1Control");
    int st = settings.status();
    if( st != QSettings::NoError ) { // error case
        addToLog("settingsSave: error code=" + QString::number(st) );
        //errorMessageDialog->showMessage("Cannot save settings");
        qWarning("Settings cannot be saved.");
        QMessageBox::critical(this, "Blink1Control",
                              "Cannot save settings.\nSettings file: "+settings.fileName(), QMessageBox::Ok);
        //qDebug() << "message box ret ="<< ret;
    }
    settingsSave( settings );
}

void MainWindow::settingsSave( QSettings & settings )
{
    qDebug() << "settingsSave: " << settings.fileName();
    settings.setValue("iftttKey", iftttKey);//sText);
    settings.setValue("autorun", autorunAction->isChecked());
    settings.setValue("dockIcon", dockIconAction->isChecked());
    settings.setValue("startMinimized",startMinimized);
    settings.setValue("server", enableServer);
    settings.setValue("logging",logging);
    settings.setValue("blink1Index", QString::number(blink1Index,16) );
    settings.setValue("enableGamma", enableGamma);
    settings.setValue("firstRun", firstRun);

    settings.setValue("serverHost", serverHost);
    settings.setValue("serverPort", serverPort);

    settings.setValue("proxyType", proxyType);
    settings.setValue("proxyHost", proxyHost);
    settings.setValue("proxyPort", proxyPort);
    settings.setValue("proxyUser", proxyUser);
    settings.setValue("proxyPass", proxyPass);

    // save patterns
    QJsonArray qarrp;
    foreach (QString nm, patterns.keys() ) {
        if(!patterns.value(nm)) continue; // if no pattern by that name, skip (should never happen)
        Blink1Pattern* p = patterns.value(nm);
        if(p->isSystem()) continue;  // don't save system patterns
        if(p->getColors().count() == 0 ) continue; // disregard zero-length patterns
        QJsonObject obj = patterns.value(nm)->toJson();
        qarrp.append(obj);
    }
    QString patternsstr = QJsonDocument(qarrp).toJson(QJsonDocument::Compact);
    settings.setValue("patterns", patternsstr);

    // save inputs
    QJsonArray qarri;
    foreach (QString nm, inputs.keys() ) {
        QJsonObject obj = inputs.value(nm)->toJson(); //FullJsonReadyToSave();
        qarri.append(obj);
    }
    QString inputsstr = QJsonDocument(qarri).toJson(QJsonDocument::Compact);
    settings.setValue("inputs", inputsstr);

    // save buttons
    QJsonArray qbutt;
    for(int i=0;i<bigButtons2.count();i++){
        QJsonObject but=bigButtons2.at(i)->toJson();
        qbutt.append(but);
    }
    QString buttsstr = QJsonDocument(qbutt).toJson(QJsonDocument::Compact);
    settings.setValue("bigbuttons2",buttsstr);

    // save emails  (why aren't these inputs?)
    QJsonArray qarrpm;
    foreach (QString nm, emails.keys() ) {
        if(!emails.value(nm)) continue;
        QJsonObject obj = emails.value(nm)->toJson();
        qarrpm.append(obj);
    }
    QString emsstr = QJsonDocument(qarrpm).toJson(QJsonDocument::Compact);
    settings.setValue("emailstoo", emsstr);

    // save hardware monitors (why aren't these inputs?)
    QJsonArray qarrpm2;
    foreach (QString nm, hardwareMonitors.keys() ) {
        if(!hardwareMonitors.value(nm)) continue;
        QJsonObject obj = hardwareMonitors.value(nm)->toJson();
        qarrpm2.append(obj);
    }
    QString harstr = QJsonDocument(qarrpm2).toJson(QJsonDocument::Compact);
    settings.setValue("hardwareMonitors", harstr);

    settings.sync();  // just in case
    addToLog("settingsSave: done");
}

//
void MainWindow::settingsImport( QString filepath )
{
    if( filepath.startsWith("file:") ) {
        QUrl furl = QUrl( filepath );
        filepath = furl.toLocalFile();
    }
    QSettings settings( filepath, QSettings::IniFormat );
    settingsLoad( settings );
    //emit bigButtonsUpdate();
    viewer.update();
}

//
void MainWindow::settingsLoad()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ThingM", "Blink1Control");
    int st = settings.status();
    if( st != QSettings::NoError ) { // error case
        addToLog("settingsLoad: error code=" + QString::number(st) );
        QMessageBox::critical(this, "Blink1Control",
                              "Cannot load settings.\nSettings file: "+settings.fileName(), QMessageBox::Ok);
    }
    settingsLoad( settings );
}

//
void MainWindow::settingsLoad( QSettings & settings )
{
    qDebug() << "settingsLoad: " << settings.fileName();
    logging        = settings.value("logging",false).toBool();
    autorun        = settings.value("autorun",false).toBool();
    dockIcon       = settings.value("dockIcon",true).toBool();
    startMinimized = settings.value("startMinimized",false).toBool();
    enableServer   = settings.value("server",false).toBool();
    enableGamma    = settings.value("enableGamma",true).toBool();
    firstRun       = settings.value("firstRun",true).toBool();

    startOrStopLogging( logging );
    addToLog("settingsLoad: settings loaded");
    
    QString sIftttKey = settings.value("iftttKey", "").toString();
    QRegExp re("^[a-f0-9]{16}"); // 16-digit hex 
    addToLog("settingsLoad: iftttKey='" + sIftttKey  + "'"); //re.exactMatch(sIftttKey.toLower()));

    // check to see if saved iftttKey is valid
    if(sIftttKey=="" || sIftttKey=="none" || !re.exactMatch(sIftttKey.toLower()) ) {
        addToLog("settingsLoad: empty or bad IftttKey, generating new IftttKey");
        sIftttKey="";
        srand(time(NULL));
            for(int i=0;i<8;i++){
                int tmp=rand()%55+48;
                while((tmp>=58 && tmp<=96))
                    tmp=rand()%55+48;  // 48? 55? 96?  these are ascii I think?
                sIftttKey.append(QChar(tmp).toLatin1());
            }
    }
    iftttKey = sIftttKey;

    // allow selection of "host" and port for API server
    // serverHost can be "localhost" or "any"
    serverHost = settings.value("serverHost","localhost").toString(); // FIXME: hardcoded default
    serverPort = settings.value("serverPort", 8934).toInt();     // FIXME: hardcoded default
    httpserver->setHost( serverHost );
    httpserver->setPort( serverPort );

    // to-do: finish implementing & documenting proxy support, issue #138
    // test SOCKS proxy with ssh -ND 9999 you@example.com
    // proxy settings
    // proxyType can be "none", "socks5" or "http"
    proxyType = settings.value("proxyType","").toString().toLower(); // "none" or "socks5" or "http"
    proxyHost = settings.value("proxyHost","").toString();
    proxyPort = settings.value("proxyPort",0).toInt();
    proxyUser = settings.value("proxyUser","").toString();
    proxyPass = settings.value("proxyPass","").toString();

    if( (proxyType == "socks5" || proxyType == "socks") && proxyHost !="" && proxyPort != 0 ) {
        addToLog("Setting SOCKS5 proxy");
        QNetworkProxy proxy;
        proxy.setType( QNetworkProxy::Socks5Proxy );
        proxy.setHostName( proxyHost );
        proxy.setPort( proxyPort );
        if( proxyUser!="" ) proxy.setUser( proxyUser );
        if( proxyPass!="" ) proxy.setPassword( proxyPass );
        QNetworkProxy::setApplicationProxy(proxy);
    }
    else if( proxyType == "http" && proxyHost !="" && proxyPort != 0 ) {
        addToLog("Setting HTTP proxy");
        QNetworkProxy proxy;
        proxy.setType( QNetworkProxy::HttpProxy );
        proxy.setHostName( proxyHost );
        proxy.setPort( proxyPort );
        if( proxyUser!="" ) proxy.setUser( proxyUser );
        if( proxyPass!="" ) proxy.setPassword( proxyPass );
        QNetworkProxy::setApplicationProxy(proxy);
    }

    // select blink(1) device to use
    QString blink1IndexStr = settings.value("blink1Index","").toString();
    setBlink1Index( blink1IndexStr );

    // load up list of built-in light patterns
    QString patternspath = QCoreApplication::applicationDirPath();
    if( mac() ) patternspath += "/../Resources/help/help/patternsReadOnly.json";  // FIXME: better way?
    else {
#if QT_NO_DEBUG        
        patternspath += "/help/help/patternsReadOnly.json";
#else
        patternspath += "/../help/help/patternsReadOnly.json";
#endif
// how much do I dislike Qt? see above
    }
    QFile patternsfile(patternspath);
    QString patternsReadOnly = "{}";
    if( patternsfile.open(QFile::ReadOnly | QFile::Text) ) { 
        QTextStream in(&patternsfile);
        patternsReadOnly = in.readAll();
        patternsfile.close();
    } else { 
        addToLog( "couldn't open patternsfile: " + patternspath);
    }

    // read only patterns
    QJsonDocument doc = QJsonDocument::fromJson( patternsReadOnly.toUtf8() );
    if( doc.isNull() ) {
        addToLog("ERROR!: patternsReadOnly syntax error!");
    }
    //qDebug() << doc.toJson( QJsonDocument::Indented );
    QJsonArray qarr = doc.array();

    qint64 nowSecs = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000;
    lastIftttDate = nowSecs;

    for( int i=0; i< qarr.size(); i++ ) {
        Blink1Pattern* bp = new Blink1Pattern();
        bp->fromJson( qarr.at(i).toObject() );
        patterns.insert( bp->name(), bp );
        connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlinkAndChangeActivePatternName(QColor,QString,int)));
        connect(bp,SIGNAL(changeColorOnVirtualBlink(QColor,double)),this,SLOT(changeColorOnVirtualBlink(QColor,double)));
    }

    QString sPatternStr = settings.value("patterns","").toString();
    if( sPatternStr.length() ) {
        QJsonDocument doc = QJsonDocument::fromJson( sPatternStr.toUtf8() );
        QJsonArray qarr = doc.array();
        for( int i=0; i< qarr.size(); i++ ) {
            Blink1Pattern* bp = new Blink1Pattern();
            bp->fromJson( qarr.at(i).toObject() );
            patterns.insert( bp->name(), bp );
            connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlinkAndChangeActivePatternName(QColor,QString,int)));
            connect(bp,SIGNAL(changeColorOnVirtualBlink(QColor,double)),this,SLOT(changeColorOnVirtualBlink(QColor,double)));
        }
    }
    QString sInputStr = settings.value("inputs","").toString();
    if( sInputStr.length() ) {
        QJsonDocument doc = QJsonDocument::fromJson( sInputStr.toUtf8() );
        QJsonArray qarr = doc.array();
        for( int i=0; i< qarr.size(); i++ ) {
            Blink1Input* bi = new Blink1Input();
            bi->fromJson( qarr.at(i).toObject() );
            inputs.insert( bi->name(), bi );
            // find most recent ifttt time
            if( bi->type() == "ifttt" ) {
                addToLog("input name:" + bi->name() +" iftttTime: " + bi->date() +
                         " nowSecs: " + QString::number(nowSecs));
                if( bi->date() > nowSecs || bi->date() < 1 ) {  // if bad stored date, fix it
                    bi->setDate( nowSecs );
                }
                //if( bi->date() > lastIftttDate )
                //    lastIftttDate = bi->date();
            }
        }
    }
    //if( lastIftttDate > nowSecs ) { 
    //    qDebug() << "lastIftttDate: bad date in input: "<< lastIftttDate;
    //    lastIftttDate = nowSecs;
    //}

    QString sButtStr = settings.value("bigbuttons2","").toString();
    if( sButtStr.length() ) {
        QJsonDocument doc = QJsonDocument::fromJson( sButtStr.toUtf8() );
        QJsonArray qarr = doc.array();
        for( int i=0; i< qarr.size(); i++ ) {
            BigButtons *bb = new BigButtons("",QColor());
            bb->fromJson( qarr.at(i).toObject() );
            bigButtons2.append(bb);
        }
    }
    if(bigButtons2.count()==0){
        bigButtons2.prepend(new BigButtons("Away",QColor("#FFFF00")));
        bigButtons2.prepend(new BigButtons("Busy",QColor("#FF0000")));
        bigButtons2.prepend(new BigButtons("Available",QColor("#00FF00")));
    }
    emit bigButtonsUpdate();

    QString sEmStr = settings.value("emailstoo","").toString();
    if( sEmStr.length() ) {
        QJsonDocument docm = QJsonDocument::fromJson( sEmStr.toUtf8() ); //sEmStr.toLatin1() );
        QJsonArray qarrm = docm.array();
        for( int i=0; i< qarrm.size(); i++ ) {
            Email* bp = new Email("");
            connect(bp,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
            connect(bp,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
            connect(bp,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
            bp->fromJson( qarrm.at(i).toObject() );
            bp->setProxySettings( proxyType, proxyHost, proxyPort, proxyUser, proxyPass );
            emails.insert( bp->getName(), bp );
        }
    }

    QString sEmStr2 = settings.value("hardwareMonitors","").toString();
    if( sEmStr2.length() ) {
        QJsonDocument docm = QJsonDocument::fromJson( sEmStr2.toUtf8() );
        QJsonArray qarrm = docm.array();
        for( int i=0; i< qarrm.size(); i++ ) {
            HardwareMonitor* bp = new HardwareMonitor("");
            connect(bp,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
            connect(bp,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
            connect(bp,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
            bp->fromJson( qarrm.at(i).toObject() );
            hardwareMonitors.insert( bp->getName(), bp );
        }
    }
    
    inputsIterator=new QMapIterator<QString,Blink1Input*>(inputs);
    emailsIterator = new QMapIterator<QString, Email*>(emails);
    hardwaresIterator = new QMapIterator<QString, HardwareMonitor*>(hardwareMonitors);

    // do these even do anything?
    emit inputsUpdate();  
    emit emailsUpdate();
    emit hardwareUpdate();
}

// single point of entry for updating the blink1 device
// also confusingly has state-machine for special BigButton functinonality
void MainWindow::updateBlink1()
{
    bool setBlink1 = false;

    if( mode == RANDOM ) {
        cc = QColor( rand() % 255, rand() % 255, rand() % 255 );
        setBlink1 = true;
        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,fadeSpeed/1000.0));
    }
    else if( mode == RGBCYCLE ) {
        if(rgbCycle==0){
            cc=QColor(255-rgbCounter,rgbCounter,0);
        }else if(rgbCycle==1){
            cc=QColor(0,255-rgbCounter,rgbCounter);
        }else{
            cc=QColor(rgbCounter,0,255-rgbCounter);
        }
        if(rgbCounter==255){
            rgbCycle=(rgbCycle+1)%3;
            rgbCounter=0;
        }
        rgbCounter += 15;
        fadeSpeed = 300;
        setBlink1 = true;
        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,fadeSpeed/1000.0));
    }
    else if( mode == STROBE ) {
        cc = (cc==QColor("#000000")) ? QColor("#ffffff") : QColor("#000000");
        fadeSpeed = 10;
        setBlink1 = true;
        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,fadeSpeed/1000.0));
    }
    else if( mode == ON ) {
        mode = NONE;
        cc = QColor("#cccccc");
        fadeSpeed = 10;
        setBlink1 = true;
        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,fadeSpeed/1000.0));
    }
    else if( mode == OFF ) {
        mode = NONE;
        cc = QColor(0,0,0);
        fadeSpeed = 10;
        setBlink1 = true;
        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,fadeSpeed/1000.0));
    }
    else if( mode == RGBSET ) {
        mode = NONE;
        // uses existing global fadespeed
        setBlink1 = true;
    }

    if( setBlink1 ) {
        addToLog("updateBlink1: fadeSpeed=" + QString::number(fadeSpeed) +", "+ cc.name());
        //+" - threadid:"+ QString::number((qint64)QThread::currentThreadId()) );
        if(blink1dev!=NULL) {
            int rc = blink1_fadeToRGBN( blink1dev, fadeSpeed , cc.red(), cc.green(), cc.blue(), led);
            if( rc < 0 ) {
                addToLog("updateBlink1: no blink1, setting refreshBlink1");
                refreshBlink1s = true;
            }
        }
        
        if(!fromPattern)
            QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor", Q_ARG(QVariant, cc.name()));
    }

    if( mode == NONE ) {
        blink1timer->stop();
    }
    
}

// Given a string of text, put it on the system clipboard
// Uses by the QML GUI to copy IFTTT key to clipboard on right-click
void MainWindow::copyToClipboard(QString txt)
{
     QClipboard *clipboard = QApplication::clipboard();
     clipboard->setText(txt);
}

// called by QML?
// called by colorwheel on colorwheel change,
// (which gets changed by pattern, so got race condition)
void MainWindow::changeColorFromQml(QColor c)
{
    cc = c;
    fadeSpeed = 0;
    mode=RGBSET;
    updateBlink1();
}

// 
void MainWindow::createActions()
{
    aboutAction = new QAction(tr("About Blink1Control"),this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

    blinkStatusAction=new QAction(blinkStatus,this);
    blinkIdAction =new QAction("blink(1) id: "+blink1Id,this);
    iftttKeyAction=new QAction("IFTTT key: "+iftttKey,this);
    #ifdef Q_OS_MAC
    blinkStatusAction->setDisabled(true);
    blinkIdAction->setDisabled(true);
    iftttKeyAction->setDisabled(true);
    #endif
    minimizeAction = new QAction(tr("Start minimized"), this);
    connect(minimizeAction,SIGNAL(triggered()),this,SLOT(changeMinimizeOption()));
    minimizeAction->setCheckable(true);
    minimizeAction->setChecked(startMinimized);
    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction,SIGNAL(triggered()),this,SLOT(showNormal()));
    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

    autorunAction=new QAction("Start at login",this);
    autorunAction->setCheckable(true);
    autorunAction->setChecked(autorun);
    connect(autorunAction,SIGNAL(triggered()),this,SLOT(changeAutorunOption()));
    dockIconAction=new QAction("Show Dock Icon",this);
    dockIconAction->setCheckable(true);
    dockIconAction->setChecked(dockIcon);
    connect(dockIconAction,SIGNAL(triggered()),this,SLOT(showhideDockIcon()));
    settingAction=new QAction("Open Settings",this);
    connect(settingAction,SIGNAL(triggered()),this,SLOT(showNormal()));

    serverAction=new QAction("Enable API server",this);
    serverAction->setCheckable(true);
    serverAction->setChecked(enableServer);
    connect(serverAction,SIGNAL(triggered()),this,SLOT(startStopServer()));
    
    offAction=new QAction("Off / Reset Alerts",this);
    connect(offAction,SIGNAL(triggered()),this,SLOT(resetAlertsOption()));
    offAction->setShortcut(Qt::Key_R | Qt::CTRL);
    // shortcuts dont' work, instead must put in QML, they're here for visual indication in menu
    //resetAlertsShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R),this); //, this, SLOT(resetAlertsOption()));
    // shortcuts don't work apparently in traymenus
    // offAction->setShortcut(Qt::Key_R | Qt::CTRL);
    // this doesn't appear to work either
    // resetAlertsShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SLOT(resetAlertsOption()));
    // neither does this work
}

void MainWindow::trayBigButtonTriggered(QAction* act)
{
    //QString pname = act->text();
    //pname.replace("Set to ",""); 
    //playBigButton( pname );
    //QKeySequence k = act->shortcut();
    qDebug() << "triggered:"<< act->text()<< ":" << act->toolTip();
    playBigButton( act->toolTip().toInt()-1 );
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addAction(blinkStatusAction);
    trayIconMenu->addAction(blinkIdAction);
    trayIconMenu->addAction(iftttKeyAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(autorunAction);
    trayIconMenu->addAction(serverAction);
    #ifdef Q_OS_MAC
    trayIconMenu->addAction(dockIconAction);
    #endif
    trayIconMenu->addSeparator();
    bigButtonActions = new QActionGroup(this);
    connect( bigButtonActions, SIGNAL(triggered(QAction*)), this, SLOT(trayBigButtonTriggered(QAction*)) );
    for( int i=0; i< bigButtons2.count(); i++ ) {
        BigButtons* b = bigButtons2.at(i);
        QAction* a = new QAction( "Set to "+b->getName(), this);
        a->setShortcut(QKeySequence("Ctrl+"+QString::number(i+1)));
        a->setToolTip( QString::number(i+1) );
        bigButtonActions->addAction(a);
        trayIconMenu->addAction( a );
    }
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(offAction);
    trayIconMenu->addAction(settingAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    trayIcon->setToolTip("Blink1Control");

    connect( trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)) );

    #ifdef Q_OS_MAC
    qt_mac_set_dock_menu( trayIconMenu );
    #endif
}

// see: http://stackoverflow.com/questions/16431270/qt-context-menu-on-trigger
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << "tray icon clicked! " << reason;

    //if( reason == QSystemTrayIcon::DoubleClick ) {
    if( reason == QSystemTrayIcon::Trigger && !mac() ) {
        showNormal();
        settingsSave();
    }
}

void MainWindow::on_buttonRGBcycle_clicked()
{
    if(mode==RGBCYCLE) return;
    blink1timer->stop();
    mode = RGBCYCLE;
    rgbCycle=0;
    rgbCounter=0;
    blink1timer->start(200);  // FIXME: timer timeotu and fadeSpeed are related
    updateBlink1();
}

void MainWindow::on_buttonMoodlight_clicked()
{
    if(mode==RANDOM) return;
    blink1timer->stop();
    mode = RANDOM;
    fadeSpeed = 2000;
    blink1timer->start( fadeSpeed );    // FIXME: timer timeotu and fadeSpeed are related
    updateBlink1();
}

void MainWindow::on_buttonStrobe_clicked()
{
    led=0;
    emit ledsUpdate();
    blink1timer->stop();
    mode = STROBE;
    blink1timer->start(200);
    updateBlink1();
}

void MainWindow::on_buttonWhite_clicked()
{
    led=0;
    emit ledsUpdate();
    blink1timer->stop();
    mode = RGBSET;
    cc = QColor("#cccccc");
    updateBlink1();
    // what is this for?
    //QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,0.0));
}

void MainWindow::on_buttonOff_clicked()
{
    foreach (QString name, patterns.keys()) {
       stopPattern(name);
    }
    mode = OFF;
    led=0;
    emit ledsUpdate();
    updateBlink1();
}

void MainWindow::on_buttonColorwheel_clicked()
{
    mode = RGBSET;
    updateBlink1();
}


void MainWindow::setColorToBlinkAndChangeActivePatternName(QColor c,QString s,int fademillis){
    fromPattern=true;
    if(s!="")
        cc=c;
    mode=RGBSET;
    activePatternName=s;
    fadeSpeed=fademillis;
    emit updateActivePatternName();
    if(patterns.contains(s)){
        led=patterns.value(s)->getCurrentLed();
        emit ledsUpdate();
    }
    //qDebug()<<"    setColorToBlink: fadespeed:"<<fadeSpeed << " color: " <<c.name() << " s: "<<s;
    updateBlink1();
    //if(s!="")// && !fromPattern) // FIXME: what was this for?
        //QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc));
    fromPattern=false;
}

// FIXME: fix this naming
// called by httpserver, patterns and many other things
void MainWindow::setColorToBlink(QColor c,int fademillis){
    cc=c;
    mode=RGBSET;
    activePatternName="";
    fadeSpeed=fademillis;
    emit updateActivePatternName();
    //qDebug()<<"todtest: setColorToBlink2: fadespeed:"<<fadeSpeed << " color: " << c.name();
    updateBlink1();
    QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,fadeSpeed/1000.0));
}

void MainWindow::setColorToBlinkN(QColor c, int fademillis, int ledn) {
    led=ledn;
    emit ledsUpdate();
    setColorToBlink(c,fademillis);
}

void MainWindow::showAboutDialog(){
    QString message = "Blink1Control \n";
    message += "   for blink(1) and blink(1) mk2.\n";
    message += "Version: " + QString(BLINK1CONTROL_VERSION)+ "\n";
    message += "2013-2015 ThingM Corp.\n";
    message += "Visit blink1.thingm.com for more info\n";
    QMessageBox::about(this, QString("About Blink1Control"), message);
}

void MainWindow::changeMinimizeOption() {
    startMinimized = !startMinimized;
    qDebug() << "changeMinimizeOption";
    //emit prefsUpdate();
    settingsSave();
}
void MainWindow::showMinimize(){
    viewer.showMinimized();
    viewer.hide();
}
void MainWindow::showNormal(){
    //qDebug() << "showNormal";
    viewer.showNormal();
    viewer.raise();  // for MacOS
    //from: http://stackoverflow.com/questions/6087887/bring-window-to-front-raise-show-activatewindow-don-t-work
    // but doesn't work
    //viewer.setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    viewer.requestActivate();
    //viewer.activateWindow(); // for Windows
}
// play the first named bigbutton
void MainWindow::playBigButton(QString name)
{
    int idx = -1;
    for( int i=0; i< bigButtons2.count(); i++) {
        if( bigButtons2.at(i)->getName() == name ) idx = i;
    }
    playBigButton(idx);
}
void MainWindow::playBigButton(int idx)
{
    if( idx >= bigButtons2.count() ) return;
    blink1timer->stop();
    QString tmp = bigButtons2.at(idx)->getPatternName();
    if( tmp=="" ) {
        this->led=bigButtons2.at(idx)->getLed();
        emit ledsUpdate();
        cc = bigButtons2.at(idx)->getCol();
        mode=RGBSET;
        updateBlink1();
        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,0.0));
    }else{
        if(patterns.contains(tmp))
            patterns.value(tmp)->play(cc);
    }
}

// called by tray menu
void MainWindow::changeAutorunOption() {
    autorun = autorunAction->isChecked();
    qDebug() << "changeAutoRun:" << autorun;
    if( autorun ){
#ifdef Q_OS_MAC
            QFileInfo dir(QDir::homePath()+"/Library/LaunchAgents");
            if(!dir.exists()) {
                QDir d(QDir::homePath()+"/Library");
                d.mkdir(QDir::homePath()+"/Library/LaunchAgents");
            }
            QFile file(QDir::homePath()+"/Library/LaunchAgents/Blink1Control.plist");
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file);
            out <<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"><plist version=\"1.0\"><dict>"
                << "<key>Label</key><string>Blink1Control</string>"
                << "<key>RunAtLoad</key><true/>"
                << "<key>Program</key><string>"
                << "/Applications/Blink1Control.app/Contents/MacOS/Blink1Control"
                << "</string></dict></plist>";
            file.close();

            QProcess *myProcess = new QProcess();
            QStringList arg;
            arg.append("load");
            arg.append("-w");
            arg.append(QDir::homePath()+"/Library/LaunchAgents/Blink1Control.plist");
            myProcess->start("launchctl", arg);
#endif
#ifdef Q_OS_WIN
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                           QSettings::NativeFormat);
        settings.setValue("blink", QCoreApplication::applicationFilePath().replace('/','\\'));
#endif

    }
    else {  // turn off autorun / start at login

#ifdef Q_OS_MAC
        QStringList arg;
        arg.append("unload");
        arg.append(QDir::homePath()+"/Library/LaunchAgents/Blink1Control.plist");
        QProcess *myProcess = new QProcess();
        myProcess->start("launchctl", arg);
        QFile file(QDir::homePath()+"/Library/LaunchAgents/Blink1Control.plist");
        if( file.exists())
            file.remove();
#endif
#ifdef Q_OS_WIN
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                           QSettings::NativeFormat);
        settings.remove("Blink1Control");
        settings.remove("blink"); // remove old cruft
#endif

    }
    //emit prefsUpdate();
    settingsSave();
}

//
void MainWindow::showhideDockIcon(){
    dockIcon = dockIconAction->isChecked();
    //emit prefsUpdate();
#ifdef Q_OS_MAC
    QSettings settings(QCoreApplication::applicationDirPath()+"/../Info.plist",QSettings::NativeFormat);
    settings.setValue("LSUIElement",dockIcon ? 0 : 1 );
#endif
    // for window do something like:
    // setWindowFlags(windowFlags() | Qt::Tool);
}

void MainWindow::startStopServer(){
    enableServer = serverAction->isChecked();
    //emit prefsUpdate();
    if(!enableServer){
        httpserver->stop();
    }else{
        httpserver->start();
    }
    //qDebug()<<"SERVER "<<httpserver->status() << " enableServer: "<<enableServer;
    //addToLog("server: "+QString::number(httpserver->status())  );
}

void MainWindow::updatePreferences() { 
    addToLog("updatePreferences: proxyUser:"+ proxyUser+", proxyPass:"+proxyPass+".");
    
    serverAction->setChecked( enableServer );
    dockIconAction->setChecked( dockIcon );

    showhideDockIcon();
    startStopServer();

    settingsSave();
}

QList<QObject*> MainWindow::getPatternsList(){
    QList<QObject*> patternsList;

    QList<Blink1Pattern*> in=patterns.values();
    qSort(in.begin(),in.end(),MainWindow::comparePatternsFunction);
    for(int i=0;i<in.count();i++)
        patternsList.append(in.at(i));
    return patternsList;
}

QList<QObject*> MainWindow::getMailsList(){
    QList<QObject*> mailList;
    QList<Email*> in=emails.values();
    for(int i=0;i<in.count();i++)
        mailList.append(in.at(i));
    return mailList;
}
QList<QObject*> MainWindow::getHardwareList(){
    QList<QObject*> hardList;
    QList<HardwareMonitor*> in=hardwareMonitors.values();
    for(int i=0;i<in.count();i++)
        hardList.append(in.at(i));
    return hardList;
}
QList<QObject*> MainWindow::getInputsList(){
    QList<QObject*> inputsList;

    QList<Blink1Input*> in=inputs.values();
    qSort(in.begin(),in.end(),MainWindow::compareInputsFunction);
    for(int i=0;i<in.count();i++) {
        if(in.at(i)->type()!="ifttt") // FIXME: wat?
            inputsList.append(in.at(i));
    }
    return inputsList;
}

QList<QObject*> MainWindow::getIFTTTList(){
    QList<QObject*> inputsList;

    QList<Blink1Input*> in=inputs.values();
    qSort(in.begin(),in.end(),MainWindow::compareInputsFunction);
    for(int i=0;i<in.count();i++)
        if(in.at(i)->type()=="ifttt")
            inputsList.append(in.at(i));
    return inputsList;
}
QVariantList MainWindow::getRecentEvents(){
    QVariantList recentList;
    for(int i=0;i<recentEvents.count();i++){
        //if(i==5) break;
        recentList.append(QVariant(recentEvents.at(i)));
    }
    return recentList;
}
/*
QVariantList MainWindow::getMailFreqs(){
    QVariantList mailFreqsList;

    mailFreqsList.append( "1 min", 12 );
}
*/
/*
QVariantList MainWindow::getMailFreqsNames(){
    QVariantList mailFreqsNamesList;
    mailFreqsNamesList.append( QString("1 min") );
    mailFreqsNamesList.append( QString("5 min") );
    mailFreqsNamesList.append( QString("15 min") );
    mailFreqsNamesList.append( QString("30 min") );
    mailFreqsNamesList.append( QString("1 hour") );
    return mailFreqsNamesList;
}
*/
QVariantList MainWindow::getPatternsNames(){
    QVariantList patternsNamesList;

    QList<Blink1Pattern*> in=patterns.values();
    qSort(in.begin(),in.end(),MainWindow::comparePatternsFunction);
    patternsNamesList.append(QVariant("no pattern chosen"));
    for(int i=0;i<in.count();i++)
        patternsNamesList.append(in.at(i)->name());
    return patternsNamesList;
}
// similar to getCatchedBlinkId (omg marcin), FIXME: refactor?
QVariantList MainWindow::getBlink1Serials() {
    QVariantList blink1SerialsList;

    int n = blink1_getCachedCount();
    if( n==0 ) {
        blink1SerialsList.append(QVariant("no blink(1)s found"));
    } else {
        for(int i=0;i<n;i++) {
            blink1SerialsList.append( QVariant(QString(blink1_getCachedSerial(i))) );
        }
    }
    return blink1SerialsList;
}

QList<QObject*> MainWindow::getBigButtons(){
    QList<QObject*> bigButtonsList;
    for(int i=0;i<bigButtons2.count();i++)
        bigButtonsList.append(bigButtons2.at(i));
    return bigButtonsList;
}
void MainWindow::removeRecentEvent(int idx){
    recentEvents.removeAt(idx);
    emit recentEventsUpdate();
}
void MainWindow::removeAllRecentEvents(){
    recentEvents.clear();
    emit recentEventsUpdate();
    on_buttonOff_clicked();
}

void MainWindow::removePattern(QString key){
    if(patterns.contains(key)){
        Blink1Pattern *tmp=patterns.value(key);
        //patterns.value(key)->stop();
        patterns.remove(key);
        disconnect(tmp);
        delete tmp;
        changePatternNameInAllMonitors(key,"");
        emit updatePatternsNamesOnUi();
    }
}
void MainWindow::removeInput(QString key,bool update){
    if(inputs.contains(key)){
        Blink1Input *tmp=inputs.value(key);
        inputs.remove(key);
        disconnect(tmp);
        //delete tmp;
        if(tmp->isChecking)
            tmp->toDelete=true;
        else
            delete tmp;
        if(!mac() || update)
            emit inputsUpdate();
    }
}

// used by color pattern editor when clicking "+" to add color spot
void MainWindow::addColorAndTimeToPattern(QString pname,QString color,double time){
    if(patterns.value(pname)->getColors().count()>=8) return;
    patterns.value(pname)->addColorAndTime(color,time);
    patterns.value(pname)->editLed(patterns.value(pname)->getColorList().count()-1,led);
}

void MainWindow::addNewPattern(QColor col, double time){
    Blink1Pattern *bp=new Blink1Pattern();
    int tmp=patterns.count();
    while(patterns.contains("pattern"+QString::number(tmp)))
        tmp++;
    bp->setName("pattern"+QString::number(tmp));
    bp->addColorAndTime(col.name(),time);
    bp->setRepeats(3);
    patterns.insert(bp->name(),bp);
    connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlinkAndChangeActivePatternName(QColor,QString,int)));
    connect(bp,SIGNAL(changeColorOnVirtualBlink(QColor,double)),this,SLOT(changeColorOnVirtualBlink(QColor,double)));
    emit patternsUpdate();
    emit updatePatternsNamesOnUi();
}

void MainWindow::updateInputsList(){
    emit inputsUpdate();
}
void MainWindow::setPatternNameToInput(QString name, QString pn){
    inputs.value(name)->setPatternName(pn);
    emit inputsUpdate();
}
void MainWindow::setFreqToInput(QString name, int freq){
    int f=freq+1;
    if(freq==1){
        f=3;
    }else if(freq==2){
        f=6;
    }else if(freq==3){
        f=12;
    }else if(freq==4){
        f=60;
    }
    inputs.value(name)->setFreq(f);
    checkInput(name);
}

QString MainWindow::getActivePatternName(){
    return activePatternName;
}
QString MainWindow::getBlinkStatus(){
    return blinkStatus;
}
QString MainWindow::getIftttKey(){
    return iftttKey;
}
QString MainWindow::getBlinkKey(){  // FIXME: rename this
    return blink1Id;
}

void MainWindow::playOrStopPattern(QString name){
    if(!patterns.contains(name)) return;
    if(!patterns.value(name)->playing()) patterns.value(name)->play(cc);
    else patterns.value(name)->stop();
}
void MainWindow::playPattern(QString name){
    if(!patterns.contains(name)) return;
    if(!patterns.value(name)->playing()) patterns.value(name)->play(cc);
}

void MainWindow::stopPattern(QString name){
    if(!patterns.contains(name)) return;
    patterns.value(name)->stop();
}

void MainWindow::removeColorAndTimeFromPattern(QString name,int idx){
    patterns.value(name)->removeColorAndTime(idx);
}
void MainWindow::changePatternRepeats(QString name){
    patterns.value(name)->changeRepeats();
}
void MainWindow::changePatternRepeatsTo(QString name, int to){
    patterns.value(name)->setRepeats(to);
}

void MainWindow::addNewBigButton(QString name, QColor col){
    bigButtons2.append(new BigButtons(name,col));
    emit bigButtonsUpdate();
}
void MainWindow::updateBigButtonColor(int idx, QColor col){
    bigButtons2.at(idx)->setColor(col);
    bigButtons2.at(idx)->setPatternName("");
    emit bigButtonsUpdate();
}
void MainWindow::updateBigButtonName(int idx, QString name){
    bigButtons2.at(idx)->setName(name);
    emit bigButtonsUpdate();
}
void MainWindow::updateBigButtonPatternName(int idx, QString name){
    bigButtons2.at(idx)->setPatternName(name);
    bigButtons2.at(idx)->setColor(patterns.value(name)->activeColor());
    emit bigButtonsUpdate();
}
void MainWindow::updateBigButtonLed(int idx, int l){
    bigButtons2.at(idx)->setLed(l);
}

void MainWindow::updateInputsArg1(QString name, QString arg1){
    inputs.value(name)->setArg1(arg1);
    checkInput(name);
}
void MainWindow::updateInputsType(QString name, QString type){
    inputs.value(name)->setType(type.toLower());
    emit inputsUpdate();
    checkInput(name);
}
void MainWindow::updateInputsPatternName(QString name, QString pn){
    if(pn=="no pattern chosen") pn="";
    inputs.value(name)->setPatternName(pn);
}
void MainWindow::createNewIFTTTInput(){
    Blink1Input *bp=new Blink1Input();
    while(inputs.contains("Name"+QString::number(duplicateCounter)))
        duplicateCounter++;
    bp->setName("Name"+QString::number(duplicateCounter));
    bp->setType("ifttt");
    bp->setArg1("My Rule Name "+QString::number(duplicateCounter));  // FIXME: arg1 & name convolved
    bp->setArg2("no value");
    bp->setPatternName("");
    inputs.insert(bp->name(),bp);
    emit inputsUpdate();
    duplicateCounter++;
}
void MainWindow::createNewInput(){
    Blink1Input *bp=new Blink1Input();
    while(inputs.contains("Name"+QString::number(duplicateCounter)))
        duplicateCounter++;
    bp->setName("Name"+QString::number(duplicateCounter));
    bp->setType("file");
    bp->setArg1("Double click to change path");
    bp->setArg2("no value");
    bp->setFreqCounter(1);
    bp->setPatternName("");
    inputs.insert(bp->name(),bp);
    emit inputsUpdate();
    duplicateCounter++;
    checkInput(bp->name());
}
void MainWindow::changePatternName(QString oldName,QString newName){
    newName=newName.left(20);
    Blink1Pattern *bp=patterns.take(oldName);
    int ile=0;
    while(patterns.contains(newName)){
        ile++;
        newName=newName+QString::number(ile);
    }
    patterns.insert(newName,bp);
    bp->setName(newName);
    changePatternNameInAllMonitors(oldName,newName);
    emit updatePatternsNamesOnUi();
}
void MainWindow::updatePatternsList(){
    emit patternsUpdate();
}

void MainWindow::changeInputName(QString oldName,QString newName){
    Blink1Input *bp=inputs.take(oldName);
    int ile=0;
    while(inputs.contains(newName)){
        ile++;
        newName=newName+QString::number(ile);
    }
    inputs.insert(newName,bp);
    bp->setName(newName);
    checkInput(newName);
}
void MainWindow::removeBigButton2(int idx){
    if(idx<bigButtons2.size() && idx>=0){
        BigButtons *tmp=bigButtons2.at(idx);
        bigButtons2.removeAt(idx);
        delete tmp;
        //if(!mac())
        emit bigButtonsUpdate();
    }
}
void MainWindow::moveBigButton2(int oldidx, int newidx)
{
    if(oldidx<bigButtons2.size() && oldidx>=0 &&
       newidx<bigButtons2.size() && newidx>=0 ) {
        //bigButtons2.swap( oldidx, newidx );
        BigButtons* tmp = bigButtons2.takeAt( oldidx );
        bigButtons2.insert( newidx, tmp );
    }
    emit bigButtonsUpdate();
}

void MainWindow::updateBigButtons(){
    emit bigButtonsUpdate();
}

void MainWindow::checkInput(QString key){
    DataInput *dI = new DataInput(this);
    connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
    connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
    connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColorFromDataInput(QColor)));
    connect(dI, SIGNAL(addReceiveEvent(int,QString,QString)), this, SLOT(addRecentEvent(int,QString,QString)));
    dI->setType(inputs[key]->type());
    dI->setInput(inputs[key]);
    dI->setIftttKey(iftttKey);
    dI->start();
    inputs[key]->isChecking=true;
}
// FIXME: why changeLed() and setLed()
void MainWindow::changeLed(int l){
    this->led=l;
}
bool MainWindow::isMk2(){
    return mk2;
}
QString MainWindow::selectFile(QString name){
    if(inputs.contains(name)){
        if(inputs.value(name)->type()=="file"){
            QString tmp=QFileDialog::getOpenFileName(this,tr("Select File"),"", tr(""));
            return tmp;
        }else{
            QString tmp=QFileDialog::getOpenFileName(this,tr("Select Script"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr(""));
            //QFileInfo fi(tmp);
            //return fi.fileName();
            return tmp;
        }
    }
    return "";
}
// FIXME: why changeLed() and setLed()
void MainWindow::setLed(int l){
    //qDebug()<<"LED "<<l;
    led=l;
    emit ledsUpdate();
    //mode = 1;
    updateBlink1();
}
int MainWindow::getLed(){
    return led;
}
int MainWindow::getLedFromPattern(QString name, int idx){
    if(patterns.contains(name)){
        return patterns.value(name)->getLed(idx);
    }
    return 0;
}
void MainWindow::setLedToPattern(QString name,int idx, int led){
    if(patterns.contains(name)){
        patterns.value(name)->editLed(idx,led);
    }
}
bool MainWindow::mac(){
    #ifdef Q_OS_MAC
            return 1;
    #endif
            return 0;
}
void MainWindow::editColorAndTimeInPattern(QString pname,QString color,double time, int index){
    patterns.value(pname)->editColorAndTime(color, time, index);
}
// FIXME: much code duplication with add_new_mail and edit_mail
void MainWindow::add_new_mail(QString name,int type, QString server, QString username, QString passwd, int port, bool ssl, int searchtype, QString searchstr ){
    addToLog("add_new_mail: "+name);
    if(name=="") name="name";
    int ile=0;
    while(emails.contains(name)){
        ile++;
        name=name+QString::number(ile);
    }
    Email *e=new Email(name);
    connect(e,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
    connect(e,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
    connect(e,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
    e->setServer(server);
    e->setServerType( (Email::ServerType)type ); 
    e->setUsername(username);
    e->setPasswd(passwd);
    e->setPort(port);
    e->setSsl(ssl);
    e->setSearchType( (Email::SearchType)searchtype );
    e->setSearchString( searchstr );
    e->setProxySettings( proxyType, proxyHost, proxyPort, proxyUser, proxyPass );
    emails.insert(name,e);
    emit emailsUpdate();
    e->checkMail();
}
// FIXME: This method is madness with so many parameters
// "result" is which "Blink when" radio button is checked
// "parser" is the thing to match (Subject or From)
// "result" is the kind of search to do (result=0 is "unread count", result=1 is "subject", result=2 is "Sender"
// "result" is now "searchtype", "parser" is now "searchstr"
void MainWindow::edit_mail(QString oldname, QString name,int type, QString server, QString username, QString passwd, int port, bool ssl, int searchtype, QString searchstr){
    addToLog("edit_mail: "+name);
    QString value;
    if(emails.contains(oldname)){
        Email *e=emails.value(oldname);
        value = e->getValue();
        if(name=="") name="name";
        if(oldname!=name){
            int tmpfreq=e->getFreq();
            int tmplastmsgid = e->getLastMsgId();
            QString tmpPatt=e->getPatternName();
            emails.remove(oldname);
            disconnect(e);
            delete e;
            emit emailsUpdate();
            int ile=0;
            while(emails.contains(name)){
                ile++;
                name=name+QString::number(ile);
            }
            e=new Email(name);
            e->setFreq(tmpfreq);
            e->setPatternName(tmpPatt);
            e->setLastMsgId(tmplastmsgid);
            connect(e,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
            connect(e,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
            connect(e,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
        }
        addToLog("edit_mail: "+name);
        e->setServer(server);
        e->setServerType( (Email::ServerType)type );
        e->setUsername(username);
        e->setPasswd(passwd);
        e->setPort(port);
        e->setSsl(ssl);
        //e->setSearchType( (searchtype==0) ? Email::UNREAD : (searchtype==1) ? Email::SUBJECT : Email::SENDER );
        e->setSearchType( (Email::SearchType)searchtype );
        e->setSearchString( searchstr );
        e->setValue(value);
        e->setProxySettings( proxyType, proxyHost, proxyPort, proxyUser, proxyPass );
        
        emails.insert(name,e);
        
        //e->settingsUpdated();  // FIXME: 
        e->checkMail();
    }
}
void MainWindow::remove_email(QString name,bool update){
    if(emails.contains(name)){
        Email *e=emails.value(name);
        emails.remove(name);
        disconnect(e);
        delete e;
        if(!mac() || update)
            emit emailsUpdate();
    }
}
void MainWindow::updateMail(){
    emit emailsUpdate();
}
/*
void MainWindow::setFreqToEmailOld(QString name,int freq){
    int f;
    if(freq==0){        f=12;
    }else if(freq==1){  f=60;
    }else if(freq==2){  f=180;
    }else if(freq==3){  f=360;
    }else{              f=720;
    }

    if(emails.contains(name)){
        emails.value(name)->setFreq(f);
        emit emailsUpdate();
    }
}
*/
void MainWindow::setFreqToEmail(QString name,int freq){
    if(emails.contains(name)){
        emails.value(name)->setFreq( freq );
        emit emailsUpdate();
    }
}
void MainWindow::setPatternNameToEmail(QString name, QString pn){
    if(pn=="no pattern chosen") pn="";
    if(emails.contains(name)){
        emails.value(name)->setPatternName(pn);
        emit emailsUpdate();
    }
}
void MainWindow::checkMail(QString name){
    addToLog("checking email "+name);
    if(emails.contains(name)){
        emails.value(name)->checkMail();
        emit emailsUpdate();
    }
}
QString MainWindow::getHostId(){
    return iftttKey.left(8);
}
void MainWindow::setHostId(QString hostId){
    //for(int i=hostId.length();i<8;i++)
    //hostId+="0";
    hostId.replace("_","0");
    iftttKey=hostId+iftttKey.right(8);
    addToLog("setHostId: "+iftttKey);
    emit iftttUpdate();
}
bool MainWindow::checkHex( QString newText){
    QRegExp re("^[a-f0-9_]+$");
    return re.exactMatch(newText.toLower());
    //return oldText;
    //return newText;
}
void MainWindow::markEmailEditing(QString s,bool e){
    if(emails.contains(s)){
        emails.value(s)->setEditing(e);
    }
}
void MainWindow::changePatternReadOnly(QString s, bool ro){
    if(patterns.contains(s)){
        patterns.value(s)->setReadOnly(ro);
        //emit patternsUpdate();
    }
}

void MainWindow::remove_hardwareMonitor(QString name, bool update){
    if(hardwareMonitors.contains(name)){
        HardwareMonitor *e=hardwareMonitors.value(name);
        hardwareMonitors.remove(name);
        disconnect(e);
        delete e;
        if(!mac() || update)
            emit hardwareUpdate();
    }
}
void MainWindow::updateHardware(){
    emit hardwareUpdate();
}

void MainWindow::setFreqToHardwareMonitor(QString name,int freq){
    int f;
    if(freq==0){
        f=12;
    }else if(freq==1){
        f=60;
    }else if(freq==2){
        f=180;
    }else if(freq==3){
        f=360;
    }else{
        f=720;
    }

    if(hardwareMonitors.contains(name)){
        hardwareMonitors.value(name)->setFreq(f);
        emit hardwareUpdate();
    }
}
void MainWindow::setPatternNameToHardwareMonitor(QString name, QString pn){
    if(pn=="no pattern chosen") pn="";
    if(hardwareMonitors.contains(name)){
        hardwareMonitors.value(name)->setPatternName(pn);
        emit hardwareUpdate();
    }
}
void MainWindow::checkHardwareMonitor(QString name){
    addToLog("checking hardware monitor "+name);
    if(hardwareMonitors.contains(name)){
        hardwareMonitors.value(name)->checkMonitor();
        emit hardwareUpdate();
    }
}
void MainWindow::copyPattern(QString name){
    Blink1Pattern *bp=new Blink1Pattern();
    int tmp=0;
    QString tmpname=name.left(9);
    while(patterns.contains(tmpname+"-copy"+QString::number(tmp)))
        tmp++;
    bp->setName(tmpname+"-copy"+QString::number(tmp));
    Blink1Pattern *oryg=patterns.value(name);
    int n=oryg->getColorList().size();
    for(int i=0;i<n;i++){
        bp->addColorAndTime(oryg->getColor(i).name(),oryg->getTime(i));
        bp->editLed(i,oryg->getLed(i));
    }
    bp->setRepeats(oryg->repeats());
    patterns.insert(bp->name(),bp);
    connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlinkAndChangeActivePatternName(QColor,QString,int)));
    connect(bp,SIGNAL(changeColorOnVirtualBlink(QColor,double)),this,SLOT(changeColorOnVirtualBlink(QColor,double)));
    emit patternsUpdate();
    emit updatePatternsNamesOnUi();
}
void MainWindow::add_new_hardwaremonitor(QString name,int type,int lvl, int action, int role){
    addToLog("add_new_hardwaremonitor: "+name);
    if(name=="") name="name";
    int ile=0;
    while(hardwareMonitors.contains(name)){
        ile++;
        name=name+QString::number(ile);
    }
    HardwareMonitor *e=new HardwareMonitor(name);
    connect(e,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
    connect(e,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
    connect(e,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
    e->setType(type);
    e->setLvl(lvl);
    e->setAction(action);
    e->setRole(role);
    hardwareMonitors.insert(name,e);
    emit hardwareUpdate();
    e->checkMonitor();
}
void MainWindow::edit_hardwaremonitor(QString oldname,QString name,int type,int lvl, int action, int role){
    addToLog("edit_hardwaremonitor: "+name);
    QString value;
    if(hardwareMonitors.contains(oldname)){
        HardwareMonitor *e=hardwareMonitors.value(oldname);
        value=e->getValue();
        if(name=="") name="name";
        if(oldname!=name){
            int tmpfreq=e->getFreq();
            QString tmpPatt=e->getPatternName();
            bool done=e->getDone();
            hardwareMonitors.remove(oldname);
            disconnect(e);
            delete e;
            emit hardwareUpdate();
            int ile=0;
            while(hardwareMonitors.contains(name)){
                ile++;
                name=name+QString::number(ile);
            }
            e=new HardwareMonitor(name);
            e->setFreq(tmpfreq);
            e->setPatternName(tmpPatt);
            e->setDone(done);
            connect(e,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
            connect(e,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
            connect(e,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
        }
        e->setAction(action);
        e->setType(type);
        e->setLvl(lvl);
        e->setRole(role);
        hardwareMonitors.insert(name,e);
        e->checkMonitor();
    }
}
void MainWindow::markHardwareEditing(QString s,bool e){
    if(hardwareMonitors.contains(s)){
        hardwareMonitors.value(s)->setEditing(e);
    }
}
void MainWindow::addToLog(QString txt){
    qint64 nowMillis = QDateTime::currentDateTime().toMSecsSinceEpoch();
    txt = QString::number(nowMillis) +": "+ txt;
    qDebug()<<("log:"+txt);
    if(logging){
        if(logstream) {
            (*logstream)<< txt <<"\n";
            logstream->flush();
        }
    }
}
void MainWindow::resetAlertsOption(){
    addToLog("resetAlertsOption");
    if(patterns.contains(activePatternName)){
        patterns.value(activePatternName)->stop();
    }
    on_buttonOff_clicked();
}
int MainWindow::checkWordWidth(QString s,int size){
    QFont f(QFont().defaultFamily(),size);
    return QFontMetrics(f).width(s);
}
void MainWindow::changeColorOnVirtualBlink(QColor c,double t){
    QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor", Q_ARG(QVariant, c.name()));
    QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, c),Q_ARG(QVariant,t));
}
bool MainWindow::checkIfColorIsTooBright(QString c){
    QColor col(c);
    if(col.red()>180 && col.green()>180 && col.blue()>180)
        return true;
    return false;
}
bool MainWindow::checkIfColorIsTooDark(QString c){
    QColor col(c);
    if(col.red()<90 && col.green()<90 && col.blue()<90)
        return true;
    return false;
}

void MainWindow::regenerateBlink1Id(){
    QString ifttt_tmp="";
    srand(time(NULL));
    for(int i=0;i<8;i++){
        int tmp=rand()%55+48;
        while((tmp>=58 && tmp<=96))
            tmp=rand()%55+48;
        ifttt_tmp.append(QChar(tmp).toLatin1());
    }
    iftttKey=ifttt_tmp+iftttKey.right(8);
    emit deviceUpdate();
    emit iftttUpdate();
}
//
QJsonArray MainWindow::getCachedBlinkId(){
    QJsonArray ja;
    int n=blink1_getCachedCount();
    if(n>0){
        for(int i=0;i<n;i++)
            ja.append(QJsonValue(QString(blink1_getCachedSerial(i))));
    }
    return ja;
}
QColor MainWindow::getCurrentColor(){
    return cc;
}
QMap<QString,Blink1Pattern*> MainWindow::getFullPatternList(){
    return patterns;
}

// called by httpserver
// name must be defined and not empty
// returns true on good parsing of patternStr and pattern was added
// or false if not
bool MainWindow::addNewPatternFromPatternStr(QString name, QString patternStr){
    Blink1Pattern *bp=new Blink1Pattern();
    bp->setName( name );
    if( ! bp->fromPatternStr( patternStr) ) {
        return false;
    }

    patterns.insert(bp->name(),bp);
    connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlinkAndChangeActivePatternName(QColor,QString,int)));
    connect(bp,SIGNAL(changeColorOnVirtualBlink(QColor,double)),this,SLOT(changeColorOnVirtualBlink(QColor,double)));
    emit patternsUpdate();
    emit updatePatternsNamesOnUi();
    emit updateActivePatternName();
    return true;
}

void MainWindow::startOrStopLogging(bool shouldLog){
    if(logging) {
        if(logFile) {
            logFile->close();
            delete logFile;
            delete logstream;
            logFile=NULL;
            logstream=NULL;
        }
    }
    logging = shouldLog;
    if(logging){
        logFile = new QFile( QDir::tempPath() + "/blink1control-log.txt");
        // to append to logfile, use this instead
        //if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
        if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text )){
            qWarning() << "File open error";
            delete logFile;
            logFile=NULL;
        }else{
            logstream = new QTextStream(logFile);
        }
        //qDebug() << "logging to "<< getLogFileName();
        //if( trayIcon )
        //    trayIcon->showMessage("Blink1Control logging", "logging to file "+getLogFileName());
    }
}
bool MainWindow::getLogging(){
    return logging;
}
QString MainWindow::getLogFileName() {
    //return (logFile!=NULL) ? logFile->fileName() : "";
    if( !logFile ) return "";
    QFileInfo fileInfo(logFile->fileName());
    return QString(fileInfo.absoluteFilePath());
}
QString MainWindow::getSettingsFileName() {
    //return (logFile!=NULL) ? logFile->fileName() : "";
    //QFileInfo fileInfo(logFile->fileName());
    // FIXME: make this match settingsLoad() & settingsSave()
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ThingM", "Blink1Control");
    return settings.fileName();
}

QMap<QString,Blink1Input*> MainWindow::getFullInputList(){
    return inputs;
}
void MainWindow::changePatternNameInAllMonitors(QString from, QString to){
    for(int i=0;i<bigButtons2.count();i++){
        if(bigButtons2.at(i)->getPatternName()==from)
            bigButtons2.at(i)->setPatternName(to);
    }
    foreach (QString name, inputs.keys()) {
       if(inputs.value(name)->patternName()==from)
           inputs.value(name)->setPatternName(to);
    }
    foreach (QString name, emails.keys()) {
        if(emails.value(name)->getPatternName()==from)
            emails.value(name)->setPatternName(to);
    }
    foreach (QString name, hardwareMonitors.keys()) {
        if(hardwareMonitors.value(name)->getPatternName()==from)
            hardwareMonitors.value(name)->setPatternName(to);
    }
}
QVariantList MainWindow::getFullColorsFromPattern(QString patternName){
    QVariantList colorsList;
    if(patterns.contains(patternName)){
        QList<QColor> tmp=patterns.value(patternName)->getColorList();
        for(int i=0;i<tmp.count();i++){
            colorsList.append(QVariant(tmp.at(i).name()));
        }
    }
    return colorsList;
}
void MainWindow::updateColorsOnBigButtons2List(){
    QList<BigButtons*> tmp=bigButtons2;
    bigButtons2.clear();
    emit updateBigButtons();
    bigButtons2=tmp;
    emit updateBigButtons();
}

void MainWindow::setStartupPattern( QString patternName )
{
    addToLog("setStartupPattern: " + patternName);
    if( patternName == "_OFF" ) {
        for( int i=0; i<16; i++ ) {
            blink1_writePatternLine( blink1dev, 1000, 0,0,0, i ); // off
        }
        blink1_savePattern( blink1dev );
    }
    else if( patternName == "_DEFAULT" ) {  // not quite the real default for mk2 or mk1
        blink1_writePatternLine( blink1dev, 500, 0xff,0x00,0x00, 0 ); // red
        blink1_writePatternLine( blink1dev, 500, 0xff,0x00,0x00, 1 ); // red
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0x00, 2 ); // off
        blink1_writePatternLine( blink1dev, 500, 0x00,0xff,0x00, 3 ); // green
        blink1_writePatternLine( blink1dev, 500, 0x00,0xff,0x00, 4 ); // green
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0x00, 5 ); // off
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0xff, 6 ); // blue
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0xff, 7 ); // blue
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0x00, 8 ); // off
        blink1_writePatternLine( blink1dev, 500, 0x80,0x80,0x80, 9 ); // half-bright
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0x00, 10 ); // off
        blink1_writePatternLine( blink1dev, 500, 0xF0,0xF0,0xF0, 11 ); // white
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0x00, 12 ); // off
        blink1_writePatternLine( blink1dev, 500, 0xF0,0xF0,0xF0, 13 ); // white
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0x00, 14 ); // off
        blink1_writePatternLine( blink1dev, 500, 0x00,0x00,0x00, 15 ); // off
        blink1_savePattern( blink1dev );
    }
    else {

    }

}



// from: http://blog.hostilefork.com/qt-essential-noisy-debug-hook/
// By default, fairly big problems like QObject::connect not working due to not being able
// to find a signal or slot goes to the debug output.  There can be a lot of spew which
// makes that easy to miss.  While perhaps the release build would want to try and
// keep going, it helps debugging to get told this ASAP.
//
// Would be nice to chain to the default Qt platform error handler
// However, this is not feasible as there is no "default error handler" function
// The default error handling is merely what runs in qt_message_output
//
//     http://qt.gitorious.org/qt/qt/blobs/4.5/src/corelib/global/qglobal.cpp#line2004
//
void noisyFailureMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "Blink1Control error: %s @ %s\n", localMsg.constData(), context.function );
    //QString logstr;
    //logstr.arg("noisyFailure: %s @ %s\n", localMsg.constData(), context.function); //nope
    //addToLog( logstr );
    
    // this is another one that doesn't make sense as just a debug message.
    // pretty serious sign of a problem
    // http://www.developer.nokia.com/Community/Wiki/QPainter::begin:Paint_device_returned_engine_%3D%3D_0_(Known_Issue)
    if ((type == QtDebugMsg)
            && msg.contains("QPainter::begin")
            && msg.contains("Paint device returned engine")) {
        type = QtWarningMsg;
    }

    // This qWarning about "Cowardly refusing to send clipboard message to hung application..."
    // is something that can easily happen if you are debugging and the application is paused.
    // As it is so common, not worth popping up a dialog.
    if ((type == QtWarningMsg)
            && QString(msg).contains("QClipboard::event")
            && QString(msg).contains("Cowardly refusing")) {
        type = QtDebugMsg;
    }

    // only the GUI thread should display message boxes.  If you are
    // writing a multithreaded application and the error happens on
    // a non-GUI thread, you'll have to queue the message to the GUI
    QCoreApplication * instance = QCoreApplication::instance();
    const bool isGuiThread = 
        instance && (QThread::currentThread() == instance->thread());

    if (isGuiThread) {
        QMessageBox messageBox;
        switch (type) {
        case QtDebugMsg:
            return;
        case QtWarningMsg:
        case QtInfoMsg:
            messageBox.setIcon(QMessageBox::Warning);
            messageBox.setInformativeText(msg);
            messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            break;
        case QtCriticalMsg:
            messageBox.setIcon(QMessageBox::Critical);
            messageBox.setInformativeText(msg);
            messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            break;
        case QtFatalMsg:
            messageBox.setIcon(QMessageBox::Critical);
            messageBox.setInformativeText(msg);
            messageBox.setStandardButtons(QMessageBox::Cancel);
            break;
        }

        int ret = messageBox.exec();
        if (ret == QMessageBox::Cancel)
            abort();
    } else {
        if (type != QtDebugMsg)
            abort(); // be NOISY unless overridden!        
    }
}

// eof
