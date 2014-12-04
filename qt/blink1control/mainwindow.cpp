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

// FIXME: this must be set to 5000 because of hard-coded values in QML by marcin/milo
#define updateInputsMillis 5000

// see: http://qt-project.org/doc/qt-5/exportedfunctions.html
//#ifdef Q_...
void qt_set_sequence_auto_mnemonic(bool);
//#endif 

// see: http://stackoverflow.com/questions/9334339/qt-add-qaction-menu-items-to-dock-icon-mac
#ifdef Q_OS_MAC
extern void qt_mac_set_dock_menu(QMenu *);
#endif

static QMutex blink1mutex;
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


#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif
void MainWindow::osFixes()
{
#ifdef Q_OS_MAC
    qDebug() << "running osFixes";
    // FIXME: the below is lame, but it appears putting this in Info.plist
    // or in mainBundle dict doesn't work.
    // appears to require app restart to take effect
    QProcess cmd;
    cmd.start("defaults write com.thingm.Blink1Control NSAppSleepDisabled -bool YES");
    cmd.waitForFinished();
    /* this doesn't seem to work
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if( mainBundle ){
        // get the application's Info Dictionary. For app bundles this would live in the bundle's Info.plist,
        // for regular executables it is obtained in another way.
        CFMutableDictionaryRef infoDict = (CFMutableDictionaryRef) CFBundleGetInfoDictionary(mainBundle);
        if( infoDict ){
            CFDictionarySetValue(infoDict, CFSTR("NSAppSleepDisabled"), CFSTR("1"));
            // Add or set the "LSUIElement" key with/to value "1". This can simply be a CFString.
            //CFDictionarySetValue(infoDict, CFSTR("LSUIElement"), CFSTR("1"));
            // That's it. We're now considered as an "agent" by the window server, and thus will have
            // neither menubar nor presence in the Dock or App Switcher.
        }
    }
    */

#endif
}

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
    startmin=false;
    enableServer=false;
    enableGamma=false;
    firstRun=true;
    logging=false;
    srand(0);
    logFile=NULL;
    logstream=NULL;
    mode = NONE;
    duplicateCounter=0; // FIXME: bad name for this var

    osFixes();
    
    httpserver = new HttpServer();
    httpserver->setController(this);
    connect( httpserver, SIGNAL(blink1SetColorById(QColor,int,QString,int)), 
             this,         SLOT(blink1SetColorById(QColor,int,QString,int)) );

    blink1_enumerate();

    settingsLoad();

    addToLog("LOGGING: "+QString::number(logging));
    startOrStopLogging( logging );

    QIcon ico = QIcon(":/images/blink1-icon0.png");
    QIcon icobw = QIcon(":/images/blink1-icon0-bw.png");
    // This causes Mac menubar to change but not app icon.
    // (Mac app icon is specified in qmake .pro with ICON var)

    setWindowIcon( ico );
    createActions();
    createTrayIcon();
    trayIcon->setIcon( mac() ? icobw : ico );
    trayIcon->show();
   
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
    viewer.setFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    viewer.setMinimumHeight(717); // for bg-new.jpg
    viewer.setMaximumHeight(717);
    viewer.setMinimumWidth(1185);
    viewer.setMaximumWidth(1185);
    viewer.setTitle("Blink(1) Control");

    inputTimerCounter = 0;
    inputsTimer = new QTimer(this);
    inputsTimer->singleShot( updateInputsMillis, this, SLOT(updateInputs()));
    isIftttChecked = false;

    if(startmin){
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

    qApp->setQuitOnLastWindowClosed(false);  // this makes close button not quit qpp

    // some keyboard shortcut experiments that seem to ALL not work
    //alertsAction->setShortcut(Qt::Key_R | Qt::CTRL);
    //alertsAction->setShortcutContext( Qt::ApplicationShortcut ); 
    //resetAlertsShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R),this); //, this, SLOT(resetAlertsOption()));
    //resetAlertsShortcut = new QShortcut( QKeySequence(Qt::CTRL+Qt::Key_R), viewer.rootObject() );
    //resetAlertsShortcut->setContext(Qt::ApplicationShortcut);
    //connect( resetAlertsShortcut, SIGNAL(activated()), this, SLOT(resetAlertsOption()));

}

// called when window has focus
void MainWindow::viewerActiveChanged() {
    qDebug() << "viewerActiveChanged: " << viewer.isActive();
    settingsSave();
}

//
void MainWindow::refreshBlink1State()
{
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
    addToLog( "refreshBlink1State: --- refreshing:" + QString::number(refreshCounter));
    //+" - threadid:"+ QString::number((qint64)QThread::currentThreadId()) );
    
    //bool gotlock = blink1mutex.tryLock(500); // wait
    //if( !gotlock ) addToLog("refreshBlink1State: *** COULD NOT MUTEX LOCK ***");
    
    // close all blink1s
    for( int i=0; i<blink1devcount; i++) {
        blink1_close( blink1devs[i] );
    }
    memset( blink1devs, 0, sizeof(blink1devs));  // clear local device cache

    // open up all blink1s
    blink1devcount = blink1_enumerate();
    for( int i=0; i<blink1devcount; i++ ) { 
        blink1devs[i] = blink1_openById( i );
        addToLog("refreshBlink1State: opened blink1 #" +QString::number(i)); // << blink1devs[i] );
    }
    
    addToLog("refreshBlink1State: opening blink1Index:" + QString::number(blink1Index,16) );
    int blid = blink1_getCacheIndexById( blink1Index );
    blink1dev = blink1devs[ blid ];

    //if( gotlock ) blink1mutex.unlock();
    
    if( blink1dev ) {
        blinkStatus="blink(1) connected";
        QString serialstr = blink1_getCachedSerial( blink1_getCacheIndexByDev(blink1dev));
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
//
void MainWindow::setBlink1Index( QString blink1IndexStr )
{
    qDebug() << "blink1IndexStr: "<< blink1IndexStr;
    bool ok;
    blink1Index  = blink1IndexStr.toLong(&ok,16);  // is blink1indexstr a hex serial number?
    if( !ok ) {
        blink1Index = blink1IndexStr.toLong(&ok,10); // is it an 0-n index?
        if( !ok ) blink1Index = 0;
    }
    qDebug() << "blink1Index: " << QString::number(blink1Index,16);
}

//
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
    
    //blink1mutex.lock();
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
    //blink1mutex.unlock();
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

    if(!fromQml){
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
    //qDebug() << "todtest: checkIfttt(txt) " << txt;
    QJsonDocument respdoc = QJsonDocument::fromJson( txt.toLatin1() );
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
                    //qDebug() << "new ifttt event for "<< input->arg1();
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
    addToLog(name+" "+from);
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
    qDebug() << "destructor";
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
    qDebug() << "quit isQuit:" << isQuit;
    if( isQuit ) return;
    isQuit = true;

    if(httpserver->status()){
        httpserver->stop();
        delete httpserver;
    }
    if(logging)
        logFile->close();
    settingsSave();
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
    qDebug() << "settingsExport: "<<filepath;
    QSettings settings( filepath, QSettings::IniFormat );
    settingsSave( settings );
}


void MainWindow::settingsSave()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ThingM", "Blink1Control");
    settingsSave( settings );
}

void MainWindow::settingsSave( QSettings & settings )
{
    //qDebug() << "settingsSave: " << settings.fileName();
    
    settings.setValue("iftttKey", iftttKey);//sText);
    settings.setValue("autorun", autorunAction->isChecked());
    settings.setValue("dockIcon", dockIconAction->isChecked());
    settings.setValue("startmin",startmin);
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
    settings.setValue("emails", emsstr);

    // save hardware monitors (why aren't these inputs?)
    QJsonArray qarrpm2;
    foreach (QString nm, hardwareMonitors.keys() ) {
        if(!hardwareMonitors.value(nm)) continue;
        QJsonObject obj = hardwareMonitors.value(nm)->toJson();
        qarrpm2.append(obj);
    }
    QString harstr = QJsonDocument(qarrpm2).toJson(QJsonDocument::Compact);
    settings.setValue("hardwareMonitors", harstr);

    //settings.sync();  // just in case
    //qDebug() << "settingsSave() done";
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
    settingsLoad( settings );
}

//
void MainWindow::settingsLoad( QSettings & settings )
{

    QString sIftttKey = settings.value("iftttKey", "").toString();
    QRegExp re("^[a-f0-9]+$");
    qDebug() << "settingsLoad: " << sIftttKey << re.exactMatch(sIftttKey.toLower());
    addToLog("iftttKey:"+sIftttKey);

    if(sIftttKey=="" || sIftttKey=="none" || !re.exactMatch(sIftttKey.toLower())){
        sIftttKey="";
        srand(time(NULL));
            for(int i=0;i<8;i++){
                int tmp=rand()%55+48;
                while((tmp>=58 && tmp<=96))
                    tmp=rand()%55+48;
                sIftttKey.append(QChar(tmp).toLatin1());
            }
    }
    iftttKey=sIftttKey;

    autorun      = settings.value("autorun",false).toBool();
    dockIcon     = settings.value("dockIcon",true).toBool();
    startmin     = settings.value("startmin",false).toBool();
    enableServer = settings.value("server",false).toBool();
    logging      = settings.value("logging",false).toBool();
    enableGamma  = settings.value("enableGamma",true).toBool();
    firstRun     = settings.value("firstRun",true).toBool();

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
        qDebug() << "Setting SOCKS5 proxy";
        QNetworkProxy proxy;
        proxy.setType( QNetworkProxy::Socks5Proxy );
        proxy.setHostName( proxyHost );
        proxy.setPort( proxyPort );
        if( proxyUser!="" ) proxy.setUser( proxyUser );
        if( proxyPass!="" ) proxy.setPassword( proxyPass );
        QNetworkProxy::setApplicationProxy(proxy);
    }
    else if( proxyType == "http" && proxyHost !="" && proxyPort != 0 ) {
        qDebug() << "Setting HTTP proxy";
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


    QString patternspath = QCoreApplication::applicationDirPath();
    if( mac() ) patternspath += "/../Resources/help/help/patternsReadOnly.json";  // FIXME: better way?
    else        patternspath += "/help/help/patternsReadOnly.json";
    QFile patternsfile(patternspath);
    QString patternsReadOnly = "{}";
    if( patternsfile.open(QFile::ReadOnly | QFile::Text) ) { 
        QTextStream in(&patternsfile);
        patternsReadOnly = in.readAll();
        patternsfile.close();
    } else { 
        qDebug() << "couldn't open patternsfile: " << patternspath;
    }

    // read only patterns
    QJsonDocument doc = QJsonDocument::fromJson( patternsReadOnly.toLatin1() );
    if( doc.isNull() ) {
        qDebug() << "ERROR!: patternsReadOnly syntax error!";
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
        QJsonDocument doc = QJsonDocument::fromJson( sPatternStr.toLatin1() );
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
        QJsonDocument doc = QJsonDocument::fromJson( sInputStr.toLatin1() );
        QJsonArray qarr = doc.array();
        for( int i=0; i< qarr.size(); i++ ) {
            Blink1Input* bi = new Blink1Input();
            bi->fromJson( qarr.at(i).toObject() );
            inputs.insert( bi->name(), bi );
            // find most recent ifttt time
            if( bi->type() == "ifttt" ) {
                qDebug() << "input name:"<<bi->name() <<" iftttTime: "<<bi->date() << " nowSecs: "<<nowSecs;
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
        QJsonDocument doc = QJsonDocument::fromJson( sButtStr.toLatin1() );
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

    QString sEmStr = settings.value("emails","").toString();
    if( sEmStr.length() ) {
        QJsonDocument docm = QJsonDocument::fromJson( sEmStr.toLatin1() );
        QJsonArray qarrm = docm.array();
        for( int i=0; i< qarrm.size(); i++ ) {
            Email* bp = new Email("");
            connect(bp,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
            connect(bp,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
            connect(bp,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
            bp->fromJson( qarrm.at(i).toObject() );
            emails.insert( bp->getName(), bp );
        }
    }

    QString sEmStr2 = settings.value("hardwareMonitors","").toString();
    if( sEmStr2.length() ) {
        QJsonDocument docm = QJsonDocument::fromJson( sEmStr2.toLatin1() );
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
    minimizeAction->setChecked(startmin);
    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction,SIGNAL(triggered()),this,SLOT(showNormal()));
    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

    autorunAction=new QAction("Start at login",this);
    autorunAction->setCheckable(true);
    autorunAction->setChecked(autorun);
    connect(autorunAction,SIGNAL(triggered()),this,SLOT(setAutorun()));
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
    
    alertsAction=new QAction("Off / Reset Alerts",this);
    connect(alertsAction,SIGNAL(triggered()),this,SLOT(resetAlertsOption()));
    
    resetAlertsShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R),this); //, this, SLOT(resetAlertsOption()));
    alertsAction->setShortcut(Qt::Key_R | Qt::CTRL);

    // shortcuts don't work apparently in traymenus
    //alertsAction->setShortcut(Qt::Key_R | Qt::CTRL);
    // this doesn't appear to work either
    //resetAlertsShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SLOT(resetAlertsOption()));
    // neither does this work
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
    trayIconMenu->addAction(settingAction);
    trayIconMenu->addAction(alertsAction);
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
    message += "2013-2014 ThingM Corp.\n";
    message += "Visit blink1.thingm.com for more info\n";
    QMessageBox::about(this, QString("About Blink1Control"), message);
}

void MainWindow::changeMinimizeOption() {
    startmin=!startmin;
    //emit prefsUpdate();
}
void MainWindow::showMinimize(){
    viewer.showMinimized();
    viewer.hide();
}
void MainWindow::showNormal(){
    qDebug() << "showNormal";
    viewer.showNormal();
    viewer.raise();  // for MacOS
    //from: http://stackoverflow.com/questions/6087887/bring-window-to-front-raise-show-activatewindow-don-t-work
    // but doesn't work
    //viewer.setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    viewer.requestActivate();
    //viewer.activateWindow(); // for Windows
}
void MainWindow::playBigButton(int idx){
    blink1timer->stop();
    QString tmp=bigButtons2.at(idx)->getPatternName();
    if(tmp==""){
        this->led=bigButtons2.at(idx)->getLed();
        emit ledsUpdate();
        cc=bigButtons2.at(idx)->getCol();
        mode=RGBSET;
        updateBlink1();
        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,0.0));
    }else{
        if(patterns.contains(tmp))
            patterns.value(tmp)->play(cc);
    }
}
void MainWindow::setAutorun(){
    autorun = autorunAction->isChecked();
    //emit prefsUpdate();
    if( autorun ){
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
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
        settings.remove("blink");
        #endif
    }else{
        #ifdef Q_OS_MAC
            QFileInfo dir(QDir::homePath()+"/Library/LaunchAgents");

            if(!dir.exists()){
                QDir d(QDir::homePath()+"/Library");
                d.mkdir(QDir::homePath()+"/Library/LaunchAgents");
            }
            QFile file(QDir::homePath()+"/Library/LaunchAgents/Blink1Control.plist");
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file);
            out <<"<plist version=\"1.0\"><dict><key>Label</key><string>Blink1Control</string><key>RunAtLoad</key><true/><key>Program</key><string>/Applications/Blink1Control.app/Contents/MacOS/Blink1Control</string></dict></plist>";
            file.close();

            QProcess *myProcess = new QProcess();
            QStringList arg;
            arg.append("load");
            arg.append(QDir::homePath()+"/Library/LaunchAgents/Blink1Control.plist");
            myProcess->start("launchctl", arg);
        #endif
        #ifdef Q_OS_WIN
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
        settings.setValue("blink", QCoreApplication::applicationFilePath().replace('/','\\'));
        #endif
    }
}
void MainWindow::showhideDockIcon(){
    dockIcon = dockIconAction->isChecked();
    //emit prefsUpdate();
    #ifdef Q_OS_MAC
    QSettings settings(QCoreApplication::applicationDirPath()+"/../Info.plist",QSettings::NativeFormat);
    settings.setValue("LSUIElement",dockIcon?0:1);
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
    qDebug()<<"SERVER IS "<<httpserver->status() << " enableServer: "<<enableServer;
    addToLog("SERVER IS "+QString::number(httpserver->status()));
}

void MainWindow::updatePreferences() { 
    qDebug() << "updatePreferences";

    qDebug() << "proxyUser: "<< proxyUser << "proxyPasss:"<<proxyPass;
    
    serverAction->setChecked( enableServer );
    dockIconAction->setChecked( dockIcon );

    showhideDockIcon();
    startStopServer();

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
        if(!mac())
            emit bigButtonsUpdate();
    }
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
    qDebug()<<"LED "<<l;
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
void MainWindow::add_new_mail(QString name,int type, QString server, QString login, QString passwd, int port, bool ssl, int result, QString parser){
    qDebug()<<"NEW EMAIL "+name;
    addToLog("NEW EMAIL "+name);
    if(name=="") name="name";
    int ile=0;
    while(emails.contains(name)){
        ile++;
        name=name+QString::number(ile);
    }
    qDebug()<<"NEW EMAIL "+name;
    Email *e=new Email(name);
    connect(e,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
    connect(e,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
    connect(e,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
    e->setServer(server);
    e->setType(type);
    e->setLogin(login);
    e->setPasswd(passwd);
    e->setPort(port);
    e->setSsl(ssl);
    e->setResult(result);
    e->setParser(parser);
    QString email=login;
    // FIXME: wtf marcin?
    //if(login.indexOf("@")==-1){
    //    if(server.indexOf(".")!=-1)
    //        email+="@"+server.mid(server.indexOf(".")+1);
    //}
    e->setEmail(email);
    emails.insert(name,e);
    emit emailsUpdate();
    e->checkMail();
}
// FIXME: This method is madness with so many parameters
// "result" is which "Blink when" radio button is checked
// "parser" is the thing to match (Subject or From)
void MainWindow::edit_mail(QString oldname, QString name,int type, QString server, QString login, QString passwd, int port, bool ssl, int result, QString parser){
    qDebug()<<"EDIT EMAIL "+name;
    addToLog("EDIT EMAIL "+name);
    QString value;
    if(emails.contains(oldname)){
        Email *e=emails.value(oldname);
        value=e->getValue();
        if(name=="") name="name";
        if(oldname!=name){
            int tmpfreq=e->getFreq();
            int tmplastid=e->getLastid();
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
            e->setLastid(tmplastid);
            connect(e,SIGNAL(runPattern(QString,bool)),this,SLOT(runPattern(QString,bool)));
            connect(e,SIGNAL(addReceiveEvent(int,QString,QString)),this,SLOT(addRecentEvent(int,QString,QString)));
            connect(e,SIGNAL(addToLog(QString)),this,SLOT(addToLog(QString)));
        }
        qDebug()<<"EDIT EMAIL "+name;
        e->setServer(server);
        e->setType(type);
        e->setLogin(login);
        e->setPasswd(passwd);
        e->setPort(port);
        e->setSsl(ssl);
        e->setResult(result);
        e->setParser(parser);
        e->setValue(value);
        QString email=login;
        if(login.indexOf("@")==-1){
            if(server.indexOf(".")!=-1)
                email+="@"+server.mid(server.indexOf(".")+1);
        }
        e->setEmail(email);
        emails.insert(name,e);
        e->setEdit(true);
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

void MainWindow::setFreqToEmail(QString name,int freq){
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

    if(emails.contains(name)){
        emails.value(name)->setFreq(f);
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
    qDebug()<<"checking email "<<name;
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
    qDebug()<<iftttKey;
    addToLog(iftttKey);
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
    qDebug()<<"checking hardware monitor "<<name;
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
    qDebug()<<"NEW HARDWARE MONITOR "+name;
    addToLog("NEW HARDWARE MONITOR "+name);
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
    qDebug()<<"EDIT HARDWAREMONITOR "+name;
    addToLog("EDIT HARDWAREMONITOR "+name);
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
    txt = QString::number(nowMillis) +":"+ txt;
    qDebug()<<("log:"+txt);
    if(logging){
        if(logstream) {
            (*logstream)<< txt <<"\n";
            logstream->flush();
        }
    }
}
void MainWindow::resetAlertsOption(){
    qDebug() << "resetAlertsOption";
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

void MainWindow::startOrStopLogging(bool log){
    if(logging) {
        if(logFile) {
            logFile->close();
            delete logFile;
            delete logstream;
            logFile=NULL;
            logstream=NULL;
        }
    }
    logging = log;
    if(logging){
        //logFile = new QFile("blink1control-log.txt");
        //logFile = new QTemporaryFile("blink1control-log.txt");
        logFile = new QFile( QDir::tempPath() + "/blink1control-log.txt");
        if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
            qDebug()<<"File open error";
            delete logFile;
            logFile=NULL;
        }else{
            logstream=new QTextStream(logFile);
        }
        qDebug() << "logging to "<< getLogFileName();
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
    qDebug() << "setStartupPattern: "<<patternName;
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


// the below was in MainWindow::MainWindow

    //
    // testing what visiblitity we have into window handling
    //
    //connect(&viewer,SIGNAL(closing(QQuickCloseEvent*)),this,SLOT(viewerClosingSlot(QQuickCloseEvent*)));
    //if(mac()) connect(&viewer,SIGNAL(visibleChanged(bool)),this,SLOT(viewerVisibleChangedSlot(bool)));
    // instead of above, just watch for when app is quitting,
    // and use static bool to make sure we don't quit twice
    //connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()) );

    //connect( &viewer, SIGNAL(changeEvent(QEvent *)), this, SLOT(viewerChangeEvent(QEvent*)) );
    //connect( &viewer, SIGNAL(statusChanged(QQuickView::Status)), this, SLOT(viewerStatusChanged(QQuickView::Status)) );
    //connect( &viewer, SIGNAL(closing(QQuickCloseEvent*)),this,SLOT(viewerClosing(QQuickCloseEvent*)));
    //connect( &viewer, SIGNAL(windowStateChanged(Qt::WindowState)),this,SLOT(viewerWindowStateChanged(Qt::WindowState)));
    //connect( &viewer, SIGNAL(visibilityChanged(QWindow::Visibility)), this, SLOT(viewerVisibilityChanged(QWindow::Visibility)) );

// for testing the above connect()s

/*
void MainWindow::changeEvent(QEvent* e)
{
    qDebug() << "changeEvent " << e;
    QMainWindow::changeEvent(e);
}
// called only on minimize?
void MainWindow::viewerVisibilityChanged(QWindow::Visibility visibility) {
    qDebug() << "viewerVisibilityChanged: " << visibility;

}
// called when minimize is finished
void MainWindow::viewerWindowStateChanged(Qt::WindowState state) {
    qDebug() << "viewerWindowStateChanged: " << state;
    if( state == Qt::WindowMinimized ) {
        qDebug() << "minimized!";
        viewer.hide();
    }
}
void MainWindow::viewerStatusChanged(QQuickView::Status status) {
    qDebug() << "viewerStatusChanged: " << status;
}
void MainWindow::viewerChangeEvent(QEvent* event) {
    qDebug() << "viewerChangeEvent: " << event;
}
void MainWindow::viewerClosing(QQuickCloseEvent*event){
    qDebug() << "viewerClosing: "<< event ;
}

// these three not needed now we're just watching QApp::aboutToQuit() and using quit() for everything
void MainWindow::viewerClosingSlot(QQuickCloseEvent* event){
    qDebug() << "viewerClosingSlot: "<< event;
}
void MainWindow::viewerVisibleChangedSlot(bool v){
    qDebug() << "viewerVisibleChangedSlot: "<< v;
}
void MainWindow::markViewerAsClosing(){
    qDebug() << "markViewerAsClosing: ";
}
*/
