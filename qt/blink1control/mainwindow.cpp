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

#include "patternsReadOnly.h"

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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    fromPattern=false;
    mk2=false;
    blinkStatus="";
    iftttKey="";
    blinkKey="none";
    autorun=false;
    dockIcon=true;
    startmin=false;
    enableServer=false;
    logging=false;
    srand(0);
    logFile=NULL;
    out=NULL;
    mode = NONE;
    duplicateCounter=0;

    httpserver=new HttpServer();
    httpserver->setController(this);

    loadSettings();
    qDebug()<<"LOGGING: "<<logging;    
    if(logging){
        logFile=new QFile("log.txt");
        if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
            qDebug()<<"File open error";
            delete logFile;
            logFile=NULL;
            out=NULL;
        }else{
            out=new QTextStream(logFile);
        }
    }
    addToLog("LOGGING: "+QString::number(logging));
    if(bigButtons2.count()==0){
        bigButtons2.prepend(new BigButtons("Away",QColor("#FFFF00")));
        bigButtons2.prepend(new BigButtons("Busy",QColor("#FF0000")));
        bigButtons2.prepend(new BigButtons("Available",QColor("#00FF00")));

        emit bigButtonsUpdate();
    }

    cc = QColor(100,100,100);

    QIcon ico = QIcon(":/images/blink1-icon0.png");
    setWindowIcon(ico);

    blink1timer = new QTimer(this);
    blink1timer->stop();  // errant timer causing extra updateBlink1() calls?
    connect(blink1timer, SIGNAL(timeout()), this, SLOT(updateBlink1()));

    blink1_disableDegamma();  // for mk2 only

    //int n = blink1_enumerate();
    //blink1dev =  blink1_open();
    blink1_enumerate();
    blink1dev = blink1_openById( blink1Index );

    if( blink1dev ) {
        char ser[10];
        char iftttkey2[20];
        sprintf(ser,"%s",blink1_getCachedSerial( blink1_getCacheIndexByDev(blink1dev)));
        //sprintf(ser,"%s",blink1_getCachedSerial(0));
        sprintf(iftttkey2, "%s",ser);
        blinkStatus="blink(1) connected";
        blinkKey=ser;
        iftttKey=iftttKey.left(8)+iftttkey2;
        mk2=blink1_isMk2(blink1dev);
        emit deviceUpdate();
    }
    else {
        blinkStatus="no blink(1) found";
        iftttKey=iftttKey.left(8)+"00000000";
        blinkKey="none";
    }

    createActions();
    createTrayIcon();
    trayIcon->setIcon( ico);
    trayIcon->show();
    trayIcon->showMessage("Blink1Control running", (mac() ? "Click menubar icon for settings & options" :
                                             "Click tray icon for settings, right-click for options"));

    activePatternName="";
    updateBlink1();

    // load help file index.html to text property of QML
    QString currPath = QDir::currentPath();
    QString appDirPath = QCoreApplication::applicationDirPath();
    //fprintf( stderr, "currPath: %s\nappDirPath: %s\n", qPrintable(currPath),qPrintable(appDirPath));
    QString helpfilepath = appDirPath;  
    if( mac() ) helpfilepath += "/../Resources/help/help/index.html";  // FIXME: better way?
    else        helpfilepath += "/help/help/index.html";
    QFile f(helpfilepath);
    //qDebug() << "opening help file: " << f.fileName();
    //fprintf( stderr, "opening help file: %s\n", qPrintable(f.fileName()) );
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&f);
    QString helpTextStr = in.readAll();
    f.close();
    viewer.rootContext()->setContextProperty("helpTextString", helpTextStr);

    qmlRegisterType<QsltCursorShapeArea>("CursorTools", 1, 0, "CursorShapeArea");
    viewer.rootContext()->setContextProperty("mw", this);
    viewer.setMainQmlFile(QStringLiteral("qml/qml/main.qml"));
    viewer.setFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    viewer.setMinimumHeight(717); // for bg-new.jpg
    viewer.setMaximumHeight(717);
    viewer.setMinimumWidth(1185);
    viewer.setMaximumWidth(1185);
    viewer.setTitle("Blink(1) Control");

    inputsIterator=new QMapIterator<QString,Blink1Input*>(inputs);

    inputTimerCounter = 0;
    inputsTimer = new QTimer(this);
    inputsTimer->singleShot(5000, this, SLOT(updateInputs()));
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

    emailsIterator = new QMapIterator<QString, Email*>(emails);
    hardwaresIterator = new QMapIterator<QString, HardwareMonitor*>(hardwareMonitors);

    setColorToBlink(cc,400);  // give a default non-black color to let people know it works

    //
    // testing what visiblitity we have into window handling
    //
    //connect(&viewer,SIGNAL(closing(QQuickCloseEvent*)),this,SLOT(viewerClosingSlot(QQuickCloseEvent*)));
    //if(mac()) connect(&viewer,SIGNAL(visibleChanged(bool)),this,SLOT(viewerVisibleChangedSlot(bool)));
    // instead of above, just watch for when app is quitting, 
    // and use static bool to make sure we don't quit twice
    //connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()) );
    
    connect( &viewer, SIGNAL(activeChanged()),this,SLOT(viewerActiveChanged()));
    //connect( &viewer, SIGNAL(changeEvent(QEvent *)), this, SLOT(viewerChangeEvent(QEvent*)) );
    //connect( &viewer, SIGNAL(statusChanged(QQuickView::Status)), this, SLOT(viewerStatusChanged(QQuickView::Status)) );
    //connect( &viewer, SIGNAL(closing(QQuickCloseEvent*)),this,SLOT(viewerClosing(QQuickCloseEvent*)));
    //connect( &viewer, SIGNAL(windowStateChanged(Qt::WindowState)),this,SLOT(viewerWindowStateChanged(Qt::WindowState)));
    //connect( &viewer, SIGNAL(visibilityChanged(QWindow::Visibility)), this, SLOT(viewerVisibilityChanged(QWindow::Visibility)) );
    
    qApp->setQuitOnLastWindowClosed(false);  // this makes close button not quit qpp
}

// for testing the above connect()s

// called when window has focus
void MainWindow::viewerActiveChanged() {
    qDebug() << "viewerActiveChanged: " << viewer.isActive();
    saveSettings();
}

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

void MainWindow::deleteDataInput(DataInput *dI)
{
    if(dI->responseTo){
        QJsonObject ob;
        ob.insert("input",dI->input->toJsonWithNameTypePNameArg1Arg2AndDate());
        ob.insert("status",QString("input "+dI->input->type().toLower()));
        QJsonDocument jd(ob);
        QByteArray ba=jd.toJson();
        dI->responseTo->write(ba);
        dI->responseTo=NULL;
    }
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
    changeColorFromQml(color);
}

void MainWindow::updateInputs()
{
    qDebug() << "updateInputs()";
    //if(blink1dev!=NULL){
    blink1_close(blink1dev);  // blink1_close checks for null
    blink1dev=NULL;
    //}
    blink1_disableDegamma();  // FIXME: why is this being done here in updateInputs()?
    // FIXME: the below is copy-n-pasted from what's in the constructor
    //int n=blink1_enumerate();
    //blink1dev =  blink1_open();
    blink1_enumerate();
    blink1dev =  blink1_openById( blink1Index );
    if( blink1dev ) {
        char ser[10];
        char iftttkey2[20];
        //sprintf(ser,"%s",blink1_getCachedSerial(0)); 
        sprintf(ser,"%s",blink1_getCachedSerial( blink1_getCacheIndexByDev(blink1dev)));
        sprintf(iftttkey2, "%s",ser);
        blinkStatus="blink(1) connected";
        blinkKey=ser;
        iftttKey=iftttKey.left(8)+iftttkey2;
        mk2=blink1_isMk2(blink1dev);
        emit deviceUpdate();
    }else {
        blinkStatus="no blink(1) found";
        iftttKey=iftttKey.left(8)+"00000000";
        blinkKey="none";
    }
    blinkStatusAction->setText(blinkStatus);
    blinkIdAction->setText("blink(1) id: "+blinkKey);
    iftttKeyAction->setText("IFTTT Key: "+iftttKey);

    emit iftttUpdate();
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
        
        if(type.toUpper() == "URL")
        {
            if(inputs[key]->freqCounter()==0)//inputTimerCounter == 0)
            {
                qDebug() << "type: URL, freqcounter==0";

                DataInput *dI = new DataInput(this);
                connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
                connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
                connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColorFromDataInput(QColor)));
                connect(dI, SIGNAL(addReceiveEvent(int,QString,QString)), this, SLOT(addRecentEvent(int,QString,QString)));
                dI->setType(type.toUpper());
                dI->setInput(inputs[key]);
                dI->setPattern(patterns.value(key));
                dI->setPatternList(patterns.keys());
                dI->setIftttKey(iftttKey);
                dI->start();
            }
            inputs[key]->changeFreqCounter();
        }
        else
        {
                if(type.toUpper() == "IFTTT.COM")
                {                    
                    if(inputTimerCounter == 0){
                        inputs[key]->updateTime();
                        if(!isIftttChecked)
                        {
                            isIftttChecked = true;
                            DataInput *dI = new DataInput(this);
                            connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
                            connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
                            connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColorFromDataInput(QColor)));
                            connect(dI, SIGNAL(iftttToCheck(QString)), this, SLOT(checkIfttt(QString)));
                            connect(dI, SIGNAL(addReceiveEvent(int,QString,QString)), this, SLOT(addRecentEvent(int,QString,QString)));
                            dI->setType(type.toUpper());
                            dI->setInput(inputs[key]);
                            dI->setPattern(patterns.value(key));
                            dI->setPatternList(patterns.keys());
                            dI->setIftttKey(iftttKey);
                            dI->start();
                        }
                    }
                }
                else
                {
                    if(inputs[key]->freqCounter()==0){
                        DataInput *dI = new DataInput(this);
                        connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
                        connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
                        connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColorFromDataInput(QColor)));
                        connect(dI, SIGNAL(addReceiveEvent(int,QString,QString)), this, SLOT(addRecentEvent(int,QString,QString)));
                        dI->setType(type.toUpper());
                        dI->setInput(inputs[key]);
                        dI->setPattern(patterns.value(key));
                        dI->setPatternList(patterns.keys());
                        dI->setIftttKey(iftttKey);
                        dI->start();
                    }
                    inputs[key]->changeFreqCounter();
                }
            }
    }

    delete emailsIterator;
    emailsIterator = new QMapIterator<QString, Email*>(emails);
    emailsIterator->toFront();
    while(emailsIterator->hasNext())
    {
        emailsIterator->next();
        QString mailname=emailsIterator->key();
        qDebug() << "updateInputs: "<< mailname;
        addToLog(mailname);
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
        qDebug() << "updateInputs: " << name;
        addToLog(name);
        if(hardwareMonitors.value(name)->getFreqCounter()==0){
            hardwareMonitors.value(name)->checkMonitor();
        }
        hardwareMonitors.value(name)->changeFreqCounter();
    }

    isIftttChecked = false;
    inputTimerCounter = (inputTimerCounter + 1) % 3;
    inputsTimer->singleShot(5000, this, SLOT(updateInputs()));

}

// checkIFttt() parses the /eventsall JSON response from api.thingm.com
// param txt is full json response from ifttt api server
// Scans through received IFTTT events, find matches to input's rule names.
// On match and if date is newer than last saved date, 
// execute the pattern bound to that rule and update event list
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

    // march through each item of the events array, comparing to each input
    QJsonArray events = respobj.value( QString("events") ).toArray();
    foreach( const QJsonValue& val, events) {
        QJsonObject ev = val.toObject();
        QString evid      = ev["blink1_id"].toString();
        QString evdatestr = ev["date"].toString();
        QString evname    = ev["name"].toString();
        QString evsource  = ev["source"].toString();
        int evdate = evdatestr.toInt();
        //qDebug() << "ev: name:"<<evname<<", date:"<< evdate;
        
        foreach ( Blink1Input* input, inputs ) {
            //qDebug() << "blink1input: "<< input->name() << "arg1: "<<input->arg1();
            // is this an IFTTT input and does the event name match?
            // FIXME: type should be just "ifttt" or enum 
            // FIXME: name should be same as rule name (aka arg1)
            if( input->type() == "IFTTT.COM" ) { 
                // is the event newer than our last event, then trigger!
                if( evdate > input->date() ) {
                    input->setDate(evdate); // save for next go around
                    input->setArg2(evsource); 
                    if( input->arg1() == evname && 
                        patterns.contains(input->patternName()))
                        patterns.value( input->patternName() )->play(cc);  // FIXME: why is cc being passed?
                    addRecentEvent(evdate, evname+" - "+evsource, "IFTTT");
                }
            }
        } // foreach input
    } // foreach event
}

void MainWindow::addRecentEvent(int date, QString name, QString from)
{
    addToLog(name+" "+from);
    QString text = getTimeFromInt(QDateTime::currentDateTime().toTime_t()/*date*/) + "-" + name + " via " + from;
    recentEvents.prepend(text);
    addToLog(QString::number(date));
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
        delete out;
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
    saveSettings();
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

void MainWindow::saveSettings()
{
    qDebug() << "saveSettings()";
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ThingM", "Blink1Control");

    settings.setValue("iftttKey", iftttKey);//sText);
    settings.setValue("autorun",autorunAction->isChecked());
    settings.setValue("dockIcon",dockIconAction->isChecked());
    settings.setValue("startmin",startmin);
    settings.setValue("server",serverAction->isChecked());
    settings.setValue("logging",logging);
    settings.setValue("blink1Index", QString::number(blink1Index,16) );

    // save patterns
    QJsonArray qarrp;
    foreach (QString nm, patterns.keys() ) {  // crashed here once?
        if(!patterns.value(nm)) continue;
        Blink1Pattern* p = patterns.value(nm);
        if(p->isSystem()) continue;  // don't save system patterns
        if(p->getColors().count() == 0 ) continue; // disregard zero-length patterns
        QJsonObject obj = patterns.value(nm)->toFullJsonReadyToSave();
        qarrp.append(obj);
    }
    QString patternsstr = QJsonDocument(qarrp).toJson();
    settings.setValue("patterns", patternsstr);
    // save inputs
    QJsonArray qarri;
    foreach (QString nm, inputs.keys() ) {
        QJsonObject obj = inputs.value(nm)->toFullJsonReadyToSave();
        qarri.append(obj);
    }
    QString inputsstr = QJsonDocument(qarri).toJson();
    settings.setValue("inputs", inputsstr);

    QJsonArray qbutt;
    for(int i=0;i<bigButtons2.count();i++){
        QJsonObject but=bigButtons2.at(i)->toJson();
        qbutt.append(but);
    }
    QString buttsstr = QJsonDocument(qbutt).toJson();
    settings.setValue("bigbuttons2",buttsstr);

    QJsonArray qarrpm;
    foreach (QString nm, emails.keys() ) {
        if(!emails.value(nm)) continue;
        QJsonObject obj = emails.value(nm)->toJson();
        qarrpm.append(obj);
    }
    QString emsstr = QJsonDocument(qarrpm).toJson();
    settings.setValue("emails", emsstr);

    QJsonArray qarrpm2;
    foreach (QString nm, hardwareMonitors.keys() ) {
        if(!hardwareMonitors.value(nm)) continue;
        QJsonObject obj = hardwareMonitors.value(nm)->toJson();
        qarrpm2.append(obj);
    }
    QString harstr = QJsonDocument(qarrpm2).toJson();
    settings.setValue("hardwareMonitors", harstr);
}

void MainWindow::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ThingM", "Blink1Control");

    QString sIftttKey = settings.value("iftttKey", "").toString();
    QRegExp re("^[a-f0-9]+$");
    qDebug() << "loadSettings: " << sIftttKey << re.exactMatch(sIftttKey.toLower());
    addToLog(sIftttKey);

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
    autorun      = settings.value("autorun","").toBool();
    dockIcon     = settings.value("dockIcon",true).toBool();
    startmin     = settings.value("startmin","").toBool();
    enableServer = settings.value("server","").toBool();
    logging      = settings.value("logging","").toBool();

    QString blink1IndexStr = settings.value("blink1Index","").toString();
    qDebug() << "blink1IndexStr: "<< blink1IndexStr;
    bool ok;
    blink1Index  = blink1IndexStr.toLong(&ok,16);
    if( !ok ) {
        blink1Index = blink1IndexStr.toLong(&ok,10);
        if( !ok ) blink1Index = 0;
    }
    qDebug() << "blink1Index: " << QString::number(blink1Index,16);

    // read only patterns
    QJsonDocument doc = QJsonDocument::fromJson( patternsReadOnly.toLatin1() );
    if( doc.isNull() ) {
        qDebug() << "ERROR!: patternsReadOnly syntax error!";
    }
    //qDebug() << doc.toJson( QJsonDocument::Indented );
    QJsonArray qarr = doc.array();

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
        }
    }

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
        cc = QColor(255,255,255); 
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
        // uses existing global fadespeed?
        setBlink1 = true;
    }

    if( setBlink1 ) {
        qDebug() << "todtest:         updateBlink1: fadeSpeed="<<fadeSpeed << ", "<< cc.name();
        if(blink1dev!=NULL) 
            blink1_fadeToRGBN( blink1dev, fadeSpeed , cc.red(), cc.green(), cc.blue(), led);
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
    blinkIdAction =new QAction("blink(1) id: "+blinkKey,this);
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
    alertsAction=new QAction("Reset Alerts",this);
    connect(alertsAction,SIGNAL(triggered()),this,SLOT(resetAlertsOption()));
    serverAction=new QAction("Enable API server",this);
    serverAction->setCheckable(true);
    serverAction->setChecked(enableServer);
    connect(serverAction,SIGNAL(triggered()),this,SLOT(startStopServer()));

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
    mode = RGBSET;
    led=0;
    emit ledsUpdate();
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
    qDebug()<<"todtest: setColorToBlink: fadespeed:"<<fadeSpeed << " color: " <<c.name() << " s: "<<s;
    updateBlink1();
    //if(s!="")// && !fromPattern)
        //QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc));
    fromPattern=false;
}

void MainWindow::setColorToBlink(QColor c,int fademillis){
    cc=c;
    mode=RGBSET;
    activePatternName="";
    fadeSpeed=fademillis;
    emit updateActivePatternName();
    qDebug()<<"todtest: setColorToBlink2: fadespeed:"<<fadeSpeed << " color: " << c.name();
    updateBlink1();
    QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc),Q_ARG(QVariant,fadeSpeed/1000.0));
}

void MainWindow::showAboutDialog(){
    QString message = "Blink1Control \n";
    message += "   for blink(1) and blink(1) mk2.\n";
    message += "Version: " + QString(BLINK1CONTROL_VERSION)+ "\n";
    message += "2013-2014 ThingM Corp.\n";
    message += "Visit blink1.thingm.com for more info\n";
    QMessageBox::about(this, QString("About Blink1Control"), message);
}

void MainWindow::changeMinimizeOption(){
    startmin=!startmin;
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
    if(autorunAction->isChecked()){
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
    #ifdef Q_OS_MAC
    QSettings settings(QCoreApplication::applicationDirPath()+"/../Info.plist",QSettings::NativeFormat);
    settings.setValue("LSUIElement",(dockIconAction->isChecked())?0:1);
    #endif
    // for window do something like:
    // setWindowFlags(windowFlags() | Qt::Tool);
}

//
// HTTP Server begin
//
void MainWindow::startStopServer(){
    if(!serverAction->isChecked()){
        httpserver->stop();
    }else{
        httpserver->start();
    }
    qDebug()<<"SERVER IS "<<httpserver->status();
    addToLog("SERVER IS "+QString::number(httpserver->status()));
}


//
// HTTP Server end
//

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
    for(int i=0;i<in.count();i++)
        if(in.at(i)->type()!="IFTTT.COM")
            inputsList.append(in.at(i));
    return inputsList;
}

QList<QObject*> MainWindow::getIFTTTList(){
    QList<QObject*> inputsList;

    QList<Blink1Input*> in=inputs.values();
    qSort(in.begin(),in.end(),MainWindow::compareInputsFunction);
    for(int i=0;i<in.count();i++)
        if(in.at(i)->type()=="IFTTT.COM")
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
QString MainWindow::getBlinkKey(){
    return blinkKey;
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
    inputs.value(name)->setType(type);
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
    bp->setType("IFTTT.COM");
    bp->setArg1("My Rule Name");
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
    bp->setType("FILE");
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
    dI->setType(inputs[key]->type().toUpper());
    dI->setInput(inputs[key]);
    dI->setPatternList(patterns.keys());
    dI->setIftttKey(iftttKey);
    dI->start();
    inputs[key]->isChecking=true;
}

void MainWindow::changeLed(int l){
    this->led=l;
}
bool MainWindow::isMk2(){
    return mk2;
}
QString MainWindow::selectFile(QString name){
    if(inputs.contains(name)){
        if(inputs.value(name)->type()=="FILE"){
            QString tmp=QFileDialog::getOpenFileName(this,tr("Select File"),"", tr(""));
            return tmp;
        }else{
            QString tmp=QFileDialog::getOpenFileName(this,tr("Select Script"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr(""));
            QFileInfo fi(tmp);
            return fi.fileName();
        }
    }
    return "";
}
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
    if(login.indexOf("@")==-1){
        if(server.indexOf(".")!=-1)
            email+="@"+server.mid(server.indexOf(".")+1);
    }
    e->setEmail(email);
    emails.insert(name,e);
    emit emailsUpdate();
    e->checkMail();
}
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
    if(logging){
        //qDebug()<<txt;
        (*out)<<txt<<"\n";
        out->flush();
    }
}
void MainWindow::resetAlertsOption(){
    if(patterns.contains(activePatternName)){
        patterns.value(activePatternName)->stop();
    }
    on_buttonOff_clicked();
}
/* 
   not needed now we use real titlebars

bool MainWindow::checkIfCorrectPositionX(int x){
    QRect desk=QApplication::desktop()->availableGeometry();
    if(x+100<desk.x()+desk.width() && x+viewer.width()-100>=desk.x())
        return true;
    return false;
}
bool MainWindow::checkIfCorrectPositionY(int y,int bar){
    QRect desk=QApplication::desktop()->availableGeometry();
    if(y>=desk.y() && y+bar-10<desk.y()+desk.height())
        return true;
    return false;
}
*/
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
QJsonArray MainWindow::getCatchedBlinkId(){
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
        logFile->close();
        delete logFile;
        delete out;
        logFile=NULL;
        out=NULL;
    }
    logging = log;
    if(logging){
        logFile=new QFile("log.txt");
        if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
            qDebug()<<"File open error";
            delete logFile;
            logFile=NULL;
        }else{
            out=new QTextStream(logFile);
        }
    }
}
bool MainWindow::getLogging(){
    return logging;
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
