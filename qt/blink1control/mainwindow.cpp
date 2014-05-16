#include "mainwindow.h"

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

enum {
    NONE = 0,
    RGBSET,
    ON,
    OFF,
    RANDOM,
    RGBCYCLE,
    MOODLIGHT
};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    closing=true;

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

    cc = QColor(0,0,0);

    QIcon ico = QIcon(":/images/blink1-icon0.png");
    setWindowIcon(ico);

    blink1timer = new QTimer(this);
    connect(blink1timer, SIGNAL(timeout()), this, SLOT(updateBlink1()));

    blink1_disableDegamma();  // for mk2 only

    int n = blink1_enumerate();
    blink1dev =  blink1_open();

    if( n ) {
        char ser[10];
        char iftttkey2[20];
        sprintf(ser,"%s",blink1_getCachedSerial(0));
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
    trayIcon->setToolTip("Blink1Control");
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();

    mode=NONE;

    counter=0;


    nam = new QNetworkAccessManager(this);

    checkTimer=new QTimer();
    connect(nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));


    activePatternName="";
    updateBlink1();
    qmlRegisterType<QsltCursorShapeArea>("CursorTools", 1, 0, "CursorShapeArea");
    viewer.rootContext()->setContextProperty("mw", this);
    viewer.setMainQmlFile(QStringLiteral("qml/qml/main.qml"));
    viewer.setFlags(Qt::WindowMaximizeButtonHint | Qt::MSWindowsFixedSizeDialogHint |Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);
    viewer.rootContext()->setContextProperty("viewerWidget", &viewer);
    viewer.setMinimumHeight(760);
    viewer.setMaximumHeight(760);
    viewer.setMinimumWidth(1185);
    viewer.setMaximumWidth(1185);
    viewer.setTitle("Blink(1) Control");
    inputsAmount=0;
    it=new QMapIterator<QString,Blink1Input*>(inputs);
    slotCheckTimeout();


    this->setAttribute(Qt::WA_DeleteOnClose);

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
    connect(&viewer,SIGNAL(closing(QQuickCloseEvent*)),this,SLOT(qu(QQuickCloseEvent*)));
    if(mac()) connect(&viewer,SIGNAL(visibleChanged(bool)),this,SLOT(qu2(bool)));

    it2 = new QMapIterator<QString, Email*>(emails);
    it3 = new QMapIterator<QString, HardwareMonitor*>(hardwareMonitors);
}

void MainWindow::qu(QQuickCloseEvent*){
    quit();
}
void MainWindow::qu2(bool v){
    if(!v && closing){
        quit();
    }
}
void MainWindow::deleteDataInput(DataInput *dI)
{
    if(dI->responseTo){
        QJsonObject ob;
        ob.insert("input",dI->input->toJson3());
        ob.insert("status",QString("input "+dI->input->type().toLower()));
        QJsonDocument jd(ob);
        QByteArray ba=jd.toJson();
        dI->responseTo->write(ba);
        dI->responseTo=NULL;
    }
    delete dI;
}

void MainWindow::runPattern(QString name, bool fromQml)
{
    if(!patterns.value(name))
        return;

    if(!fromQml){
        if(patterns.contains(name)) patterns.value(name)->play();
    }else
    {
        QColor c = QColor("#000000");
        if(patterns.value(name)->getColorList().count() <= 0)
        {
            setColor(c);
            setvalueRet("NO VALUE");
        }
        else
        {
            c = patterns.value(name)->getColorList().at(0);
            setColor(c);
            setvalueRet(c.name());
        }
    }
}

void MainWindow::setColor(QColor color)
{
    colorChanged(color);
}

void MainWindow::updateInputs()
{
    if(blink1dev!=NULL){
        blink1_close(blink1dev);
        blink1dev=NULL;
    }
    blink1_disableDegamma();
    int n=blink1_enumerate();
    blink1dev =  blink1_open();
    if( n ) {
        char ser[10];
        char iftttkey2[20];
        sprintf(ser,"%s",blink1_getCachedSerial(0));
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
    blinkIdAction->setText("Blink1 id: "+blinkKey);
    iftttKeyAction->setText("IFTTT.COM ID: "+iftttKey);

    emit iftttUpdate();
    QString type;
    QString key;
    delete it;
    it = new QMapIterator<QString, Blink1Input*>(inputs);
    it->toFront();
    while(it->hasNext())
    {
        it->next();
        key = it->key();
        type = inputs[key]->type();

        if(type.toUpper() == "URL")
        {
            if(inputs[key]->freqCounter()==0)//inputTimerCounter == 0)
            {
                DataInput *dI = new DataInput(this);
                connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
                connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
                connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColor(QColor)));
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
                        if(!isIftttChecked)
                        {
                            isIftttChecked = true;
                            DataInput *dI = new DataInput(this);
                            connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
                            connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
                            connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColor(QColor)));
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
                        connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColor(QColor)));
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

    delete it2;
    it2 = new QMapIterator<QString, Email*>(emails);
    it2->toFront();
    while(it2->hasNext())
    {
        it2->next();
        QString mailname=it2->key();
        qDebug()<<mailname;
        addToLog(mailname);
        if(emails.value(mailname)->getFreqCounter()==0){
            emails.value(mailname)->checkMail();
        }
        emails.value(mailname)->changeFreqCounter();
    }

    delete it3;
    it3 = new QMapIterator<QString, HardwareMonitor*>(hardwareMonitors);
    it3->toFront();
    while(it3->hasNext())
    {
        it3->next();
        QString name=it3->key();
        qDebug()<<name;
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

void MainWindow::checkIfttt(QString txt)
{
    int date=-1;
    QString dateString = "";
    QString name = "";
    QString source="";
    int index = txt.indexOf("\"event_count\":");
    int count=0;
    if(index != -1)
    {
        dateString.clear();
        index += 14;
        while(txt[index].isDigit()) {
            dateString.append(txt[index]);
            index++;
        }
        count = dateString.toInt();
    }

    delete it;
    it = new QMapIterator<QString, Blink1Input*>(inputs);
    QString key;

    int startIdx = 0;
    int tmpIdx = 0;
    for(int i=0; i<count; i++)
    {
        tmpIdx = index = txt.indexOf("\"date\":", startIdx);
        dateString.clear();
        if(index != -1)
        {
            index += 8;
            while(txt[index].isDigit()) {
                dateString.append(txt[index]);
                index++;
            }
            date = dateString.toInt();
        }

        index = txt.indexOf("\"name\":", startIdx);
        if (index > tmpIdx)
            tmpIdx = index;
        name.clear();
        if(index != -1)
        {
            index += 8;
            while(txt[index] != '\"') {
                name.append(txt[index]);
                index ++;
            }
        }

        index = txt.indexOf("\"source\":", startIdx);
        if (index > tmpIdx)
            tmpIdx = index;
        source.clear();
        if(index != -1)
        {
            index += 10;
            while(txt[index] != '\"') {
                source.append(txt[index]);
                index ++;
            }
        }

        if(!dateString.isEmpty() && !name.isEmpty() && name != "Default")
        {
            it->toFront();
            while(it->hasNext())
            {
                it->next();
                key = it->key();
                if(inputs[key]->arg1() == name)
                {
                    if(inputs[key]->date() != date) {
                        inputs[key]->setDate(date);
                        if(patterns.contains(inputs[key]->patternName()))
                            patterns.value(inputs[key]->patternName())->play();
                        addRecentEvent(date, name, "IFTTT");
                        inputs[key]->setArg2(source);//inputs[key]->patternName());
                    }
                    break;
                }
            }
        }

        startIdx = tmpIdx+1;
    }
}
void MainWindow::checkIfttt(QString txt, Blink1Input *in)
{
    int date=-1;
    QString dateString = "";
    QString name = "";
    QString source="";
    int index = txt.indexOf("\"event_count\":");
    int count=0;
    if(index != -1)
    {
        dateString.clear();
        index += 14;
        while(txt[index].isDigit()) {
            dateString.append(txt[index]);
            index++;
        }
        count = dateString.toInt();
    }

    delete it;
    it = new QMapIterator<QString, Blink1Input*>(inputs);
    QString key;

    int startIdx = 0;
    int tmpIdx = 0;
    for(int i=0; i<count; i++)
    {
        tmpIdx = index = txt.indexOf("\"date\":", startIdx);
        dateString.clear();
        if(index != -1)
        {
            index += 8;
            while(txt[index].isDigit()) {
                dateString.append(txt[index]);
                index++;
            }
            date = dateString.toInt();
        }

        index = txt.indexOf("\"name\":", startIdx);
        if (index > tmpIdx)
            tmpIdx = index;
        name.clear();
        if(index != -1)
        {
            index += 8;
            while(txt[index] != '\"') {
                name.append(txt[index]);
                index ++;
            }
        }

        index = txt.indexOf("\"source\":", startIdx);
        if (index > tmpIdx)
            tmpIdx = index;
        source.clear();
        if(index != -1)
        {
            index += 10;
            while(txt[index] != '\"') {
                source.append(txt[index]);
                index ++;
            }
        }

        if(!dateString.isEmpty() && !name.isEmpty() && name != "Default")
        {
                if(in->arg1() == name)
                {
                    if(in->date() != date) {
                        in->setDate(date);
                        in->setArg2(source);//inputs[key]->patternName());
                    }
                    break;
                }
            //}
        }

        startIdx = tmpIdx+1;
    }
}

void MainWindow::addRecentEvent(int date, QString name, QString from)
{
    qDebug()<<name<<from;
    addToLog(name+" "+from);
    QString text = getTimeFromInt(QDateTime::currentDateTime().toTime_t()/*date*/) + "-" + name + " from " + from;
    recentEvents.prepend(text);
    qDebug()<<date;
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

void MainWindow::fromQmlRequest(QString type, QString rule)
{
    DataInput *dI = new DataInput(this);
    connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
    connect(dI, SIGNAL(runPattern(QString, bool)), this, SLOT(runPattern(QString, bool)));
    connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColor(QColor)));
    connect(dI, SIGNAL(setValueRet(QString)), this, SLOT(setVRet(QString)));
    dI->setType(type);
    dI->setRule(rule);
    dI->setPatternList(patterns.keys());
    dI->setIftttKey(iftttKey);

    dI->startQml();
}

void MainWindow::setVRet(QString name)
{
    setvalueRet(name);
    emit changeValRef();
}
QString MainWindow::valueRet() const
{
    return mvalueRet;
}
void MainWindow::setvalueRet(const QString &valueRet)
{
    mvalueRet = valueRet;
}

void MainWindow::replyFinished(QNetworkReply* r){
    QString reply=r->readAll();
    int idx=reply.indexOf(QRegExp("#([0-9a-fA-F]{6})"));
    QColor c=QColor("#000000");
    if(idx!=-1){
        c=QColor(reply.mid(idx,7));
    }
}

void MainWindow::slotCheckTimeout(){
    counter=0;
    it->toFront();
}

MainWindow::~MainWindow()
{
    delete minimizeAction;
    delete restoreAction;
    delete quitAction;
    delete blinkIdAction;
    delete iftttKeyAction;

    delete trayIcon;
    delete trayIconMenu;

    delete blink1timer;
    delete checkTimer;

    if(logging){
        delete logFile;
        delete out;
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    quit();
}

void MainWindow::quit()
{
    if(server.isListening())
        server.close();
    if(logging)
        logFile->close();
    if(blink1dev!=NULL){
        led=0;
        blink1_fadeToRGBN(blink1dev, 0, 0,0,0 ,led);
        blink1_close(blink1dev);
    }
    saveSettings();
    trayIcon->hide();
    foreach (QString name, patterns.keys()) {
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
    QTimer::singleShot(500, qApp, SLOT(quit()));
}

void MainWindow::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ThingM", "Blink1Control");

    settings.setValue("iftttKey", iftttKey);//sText);
    settings.setValue("autorun",autorunAction->isChecked());
    settings.setValue("dockIcon",dockIconAction->isChecked());
    settings.setValue("startmin",startmin);
    settings.setValue("server",serverAction->isChecked());
    settings.setValue("logging",logging);
    // save patterns
    QJsonArray qarrp;
    foreach (QString nm, patterns.keys() ) {
        if(!patterns.value(nm)) continue;
        QJsonObject obj = patterns.value(nm)->toJson();
        qarrp.append(obj);
    }
    QString patternsstr = QJsonDocument(qarrp).toJson();
    settings.setValue("patterns", patternsstr);
    // save inputs
    QJsonArray qarri;
    foreach (QString nm, inputs.keys() ) {
        QJsonObject obj = inputs.value(nm)->toJson();
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
    qDebug()<<sIftttKey;
    addToLog(sIftttKey);
    qDebug()<<re.exactMatch(sIftttKey.toLower());
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
    autorun=settings.value("autorun","").toBool();
    dockIcon=settings.value("dockIcon",true).toBool();
    startmin=settings.value("startmin","").toBool();
    enableServer=settings.value("server","").toBool();
    logging=settings.value("logging","").toBool();
    QString sPatternStr = settings.value("patterns","").toString();
    if( sPatternStr.length() ) {
        QJsonDocument doc = QJsonDocument::fromJson( sPatternStr.toLatin1() );
        QJsonArray qarr = doc.array();
        for( int i=0; i< qarr.size(); i++ ) {
            Blink1Pattern* bp = new Blink1Pattern();
            bp->fromJson( qarr.at(i).toObject() );
            patterns.insert( bp->name(), bp );
            connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlink(QColor,QString,int)));
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


void MainWindow::updateBlink1()
{

    bool setBlink1 = false;
    if(mode!=RGBCYCLE && mode!=RANDOM) blink1timer->stop();

    if( mode == RANDOM ) {
        cc = QColor( rand() % 255, rand() % 255, rand() % 255 );
        fadeSpeed = 200;
        setBlink1 = true;
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
        rgbCounter+=15;
        setBlink1 = true;
        fadeSpeed = 200;
    }
    else if( mode == ON ) {
        cc = QColor(255,255,255); //cr = cg = cb = 255;
        setBlink1 = true;
        mode = NONE;
        fadeSpeed = 0;

    }
    else if( mode == OFF ) {
        cc = QColor(0,0,0); //cr = cg = cb = 0;
        setBlink1 = true;
        mode = NONE;
        fadeSpeed = 0;
    }
    else if( mode == RGBSET ) {
        mode = NONE;
        setBlink1 = true;
    }

    if( setBlink1 ) {
        if(blink1dev!=NULL)
            blink1_fadeToRGBN( blink1dev, fadeSpeed , cc.red(), cc.green(), cc.blue() ,led);

        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor", Q_ARG(QVariant, cc.name()));
    }
}

void MainWindow::colorChanged(QColor c)
{
    cc = c; // cr = c.red(); cg = c.green(); cb = c.blue();
            fadeSpeed=0;
    mode=RGBSET;
    updateBlink1();
}


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
    blinkStatusAction=new QAction(blinkStatus,this);
    blinkIdAction=new QAction("Blink1 id: "+blinkKey,this);
    iftttKeyAction=new QAction("IFTTT.COM ID: "+iftttKey,this);
    #ifdef Q_OS_MAC
    blinkStatusAction->setDisabled(true);
    blinkIdAction->setDisabled(true);
    iftttKeyAction->setDisabled(true);
    #endif
    minimizeAction = new QAction(tr("Start minimize"), this);
    connect(minimizeAction,SIGNAL(triggered()),this,SLOT(minimalize()));
    minimizeAction->setCheckable(true);
    minimizeAction->setChecked(startmin);
    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction,SIGNAL(triggered()),this,SLOT(normal()));
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
    connect(settingAction,SIGNAL(triggered()),this,SLOT(normal()));
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

}


void MainWindow::on_buttonRGBcycle_clicked()
{
    if(mode==RGBCYCLE) return;
    blink1timer->stop();
    mode = RGBCYCLE;
    rgbCycle=0;
    rgbCounter=0;
    blink1timer->start(200);
    updateBlink1();
}

void MainWindow::on_buttonMoodlight_clicked()
{
    if(mode==RANDOM) return;
    blink1timer->stop();
    mode = RANDOM;
    blink1timer->start(200);
    updateBlink1();
}

void MainWindow::on_buttonOff_clicked()
{
    mode = OFF;
    led=0;
    emit ledsUpdate();
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
    QString cname = QString("background: rgb(%1,%2,%3)").arg(qc.red()).arg(qc.green()).arg(qc.blue());
    colorChanged(qc);

}

void MainWindow::on_buttonBusyColorSpot_clicked()
{

}

int MainWindow::getSize(){
    return inputsAmount;
}
void MainWindow::setColorToBlink(QColor c,QString s,int f){
    if(s!="")
        cc=c;
    mode=RGBSET;
    activePatternName=s;
    fadeSpeed=f;
    emit updatePatternName();
    if(patterns.contains(s)){
        led=patterns.value(s)->getCurrentLed();
        emit ledsUpdate();
    }
    updateBlink1();
    if(s!="")
        QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc));
}
void MainWindow::setColorToBlink2(QColor c,int f){
    cc=c;
    mode=RGBSET;
    activePatternName="";
    fadeSpeed=f;
    emit updatePatternName();
    updateBlink1();
    QMetaObject::invokeMethod((QObject*)viewer.rootObject(),"changeColor2", Q_ARG(QVariant, cc));
}
void MainWindow::minimalize(){
    startmin=!startmin;
}
void MainWindow::minButton(){
    closing=false;
    viewer.showMinimized();
    viewer.hide();
    closing=true;
}
void MainWindow::normal(){
    viewer.show();
}
void MainWindow::playBigButton(int idx){
    blink1timer->stop();
    QString tmp=bigButtons2.at(idx)->getPatternName();
    this->led=bigButtons2.at(idx)->getLed();
    emit ledsUpdate();
    if(tmp==""){
        cc=bigButtons2.at(idx)->getCol();
        mode=RGBSET;
        updateBlink1();
    }else{
        if(patterns.contains(tmp))
            patterns.value(tmp)->play();
    }
}
void MainWindow::setAutorun(){
    if(autorunAction->isChecked()){
#ifdef Q_OS_MAC
        QStringList arg;
        arg.append("unload");
        arg.append(QDir::homePath()+"/Library/LaunchAgents/test2.plist");
        QProcess *myProcess = new QProcess();
        myProcess->start("launchctl", arg);
        QFile file(QDir::homePath()+"/Library/LaunchAgents/test2.plist");
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
            QFile file(QDir::homePath()+"/Library/LaunchAgents/test2.plist");
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file);
            out <<"<plist version=\"1.0\"><dict><key>Label</key><string>test2</string><key>RunAtLoad</key><true/><key>Program</key><string>/Applications/test2.app/Contents/MacOS/test2</string></dict></plist>";
            file.close();

            QProcess *myProcess = new QProcess();
            QStringList arg;
            arg.append("load");
            arg.append(QDir::homePath()+"/Library/LaunchAgents/test2.plist");
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
}
void MainWindow::startStopServer(){
    if(!serverAction->isChecked()){
        for(int i=0;i<clientConnections.count();i++)
            clientConnections.at(i)->abort();
        clientConnections.clear();
        server.close();
    }else{
        connect(&server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
        server.listen(QHostAddress::LocalHost, 8934);
    }
    qDebug()<<"SERVES IS "<<server.isListening();
    addToLog("SERVER IS "+QString::number(server.isListening()));
}
void MainWindow::acceptConnection()
{
    while(server.hasPendingConnections()){
      QTcpSocket *client=server.nextPendingConnection();
      connect(client, SIGNAL(readyRead()),this, SLOT(startRead()));
      connect(client, SIGNAL(disconnected()),this,SLOT(discardClient()));
      clientConnections.append(client);
    }
}
void MainWindow::startRead()
{
    QTcpSocket *client=(QTcpSocket*)sender();
    QString mssg=client->readLine();
    mssg=mssg.replace("%23","#");
    bool invalid=false;
    if(mssg.indexOf("/blink1/")!=-1){
        //mssg=mssg.mid(8);
        qDebug()<<"MESSAGE: "<<mssg;
        addToLog("MESSAGE: "+mssg);

        mssg=QString(mssg).split(QRegExp(" "))[1];        
        mssg=mssg.mid(8);
        QStringList tokens = QString(mssg).split(QRegExp("(\\?|\\=|\\&)"));
        QList<QPair<QString,QString> > args;
        if(mssg.indexOf("?")!=-1)
            args=QUrlQuery(mssg.mid(mssg.indexOf("?")+1)).queryItems();
        QString request=mssg.left(mssg.indexOf("?"));
        qDebug()<<request;
        qDebug()<<args;
        if(request=="id"){
            QJsonObject ob;
            ob.insert("blink_id",iftttKey);
            QJsonArray ja;
            int n=blink1_getCachedCount();
            if(n>0){
                for(int i=0;i<n;i++)
                    ja.append(QJsonValue(QString(blink1_getCachedSerial(i))));
            }
            ob.insert("blink1_serialnums",ja);
            ob.insert("status",QString("blink1 id"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="regenerateblink1id"){
            QJsonObject ob;
            ob.insert("blink_id_old",iftttKey);
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

            ob.insert("blink_id",iftttKey);
            ob.insert("status",QString("regenerate id"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
            emit iftttUpdate();
        }else if(request=="enumerate"){
            QJsonObject ob;
            QJsonArray ja;
            ob.insert("blink_id_old",iftttKey);
            QString ifttt_tmp="";
            ifttt_tmp=iftttKey.left(8);
            if(blink1dev!=NULL){
                blink1_close(blink1dev);
                blink1dev=NULL;
            }
            blink1_disableDegamma();
            int n=blink1_enumerate();
            blink1dev =  blink1_open();
            if( n ) {
                char ser[10];
                char iftttkey2[20];
                sprintf(ser,"%s",blink1_getCachedSerial(0));
                sprintf(iftttkey2, "%s",ser);
                blinkStatus="blink(1) connected";
                blinkKey=ser;
                iftttKey=ifttt_tmp+iftttkey2;
                mk2=blink1_isMk2(blink1dev);
                emit deviceUpdate();
            }else {
                blinkStatus="no blink(1) found";
                iftttKey=ifttt_tmp+"00000000";
                blinkKey="none";
            }
            ob.insert("blink_id",iftttKey);

            if(n>0){
                for(int i=0;i<n;i++)
                    ja.append(QJsonValue(QString(blink1_getCachedSerial(i))));
            }

            ob.insert("blink1_serialnums",ja);
            ob.insert("status",QString("enumerate"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
            emit iftttUpdate();

        }else if(request=="fadeToRGB"){
            //qDebug()<<tokens;
            if(args.count()==2 && args.at(0).first=="rgb" && checkIfColor(args.at(0).second) && args.at(1).first=="time" && checkIfNumber(args.at(1).second,1)){
                QJsonObject ob;
                QString cc=args.at(0).second;
                ob.insert("rgb",cc);
                double time=0.1;
                time=args.at(1).second.toDouble();
                ob.insert("status",request+": "+cc+" t:"+QString::number(time));
                ob.insert("time",QString::number(time));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
                setColorToBlink2(QColor(cc),time*1000);
            }else{
                invalid=true;
            }
            /*QJsonObject ob;
            ob.insert("rgb",tokens[2].replace("%23","#"));
            double time=0.1;
            if(tokens.length()==5) time=tokens[4].toDouble();
            ob.insert("status",tokens[0]+": "+tokens[2]+" t:"+QString::number(time));
            ob.insert("time",QString::number(time));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
            setColorToBlink(QColor(tokens[2].replace("%23","#"))," ",time*1000);*/
        }else if(request=="on"){
            stopPattern(activePatternName);
            QJsonObject ob;
            QString cc="#FFFFFF";
            ob.insert("rgb",cc);
            double time=0.1;
            ob.insert("status",request+": "+cc+" t:"+QString::number(time));
            ob.insert("time",QString::number(time));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
            setColorToBlink2(QColor(cc),time*1000);
        }else if(request=="off"){
            stopPattern(activePatternName);
            QJsonObject ob;
            QString cc="#000000";
            ob.insert("rgb",cc);
            double time=0.1;
            ob.insert("status",request+": "+cc+" t:"+QString::number(time));
            ob.insert("time",QString::number(time));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
            setColorToBlink2(QColor(cc),time*1000);
        }else if(request=="lastColor"){
            QJsonObject ob;
            ob.insert("lastColor",cc.name());
            ob.insert("status",QString("last color"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="logging"){
            if(args.count()==1 && args.at(0).first=="loglevel" && (args.at(0).second=="0" || args.at(0).second=="1")){
                QJsonObject ob;
                if(logging){
                    logFile->close();
                    delete logFile;
                    delete out;
                    logFile=NULL;
                    out=NULL;
                }
                logging=args.at(0).second.toInt();
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
                ob.insert("loglevel",logging);
                ob.insert("status",QString("logging"));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }else{
                invalid=true;
            }

            /*QJsonObject ob;
            if(logging){
                logFile->close();
                delete logFile;
                delete out;
                logFile=NULL;
                out=NULL;
            }
            logging=tokens[2].toInt();
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
            ob.insert("loglevel",tokens[2]);
            ob.insert("status",QString("logging"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);*/
        }else if(request=="inputs"){
            int enable=-1;
            /*if(tokens.count()==3)
                enable=(tokens[2]=="on")?0:1;*/
            if(args.count()==0 || (args.count()==1 && args.at(0).first=="enable" && (args.at(0).second=="on" || args.at(0).second=="off"))){
                if(args.count()==1)
                    enable=(args.at(0).second=="on")?0:1;
                QJsonArray qarrp;
                foreach (QString nm, inputs.keys() ) {
                    qDebug()<<nm;
                    if(enable==-1){
                        QJsonObject obj = inputs.value(nm)->toJson2();
                        qarrp.append(obj);
                    }else if(enable==0){
                        if(!inputs.value(nm)->pause()){
                            QJsonObject obj = inputs.value(nm)->toJson2();
                            qarrp.append(obj);
                        }
                    }else if(enable==1){
                        if(inputs.value(nm)->pause()){
                            QJsonObject obj = inputs.value(nm)->toJson2();
                            qarrp.append(obj);
                        }
                    }
                }
                QJsonObject jo;
                jo.insert("inputs",qarrp);
                QByteArray ba=QJsonDocument(jo).toJson();
                client->write(ba);
            }else{
                invalid=true;
            }
            /*QJsonArray qarrp;
            foreach (QString nm, inputs.keys() ) {
                qDebug()<<nm;
                if(enable==-1){
                    QJsonObject obj = inputs.value(nm)->toJson2();
                    qarrp.append(obj);
                }else if(enable==0){
                    if(!inputs.value(nm)->pause()){
                        QJsonObject obj = inputs.value(nm)->toJson2();
                        qarrp.append(obj);
                    }
                }else if(enable==1){
                    if(inputs.value(nm)->pause()){
                        QJsonObject obj = inputs.value(nm)->toJson2();
                        qarrp.append(obj);
                    }
                }
            }
            QJsonObject jo;
            jo.insert("inputs",qarrp);
            QByteArray ba=QJsonDocument(jo).toJson();
            client->write(ba);*/
        }else if(request=="input/del"){
            /*removeInput(tokens[2]);
            QJsonObject ob;
            ob.insert("status",QString("input del"));*/
            QJsonObject ob;
            ob.insert("status",QString("input not implemented yet"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="input/delall"){
            //inputs.clear();
            //emit inputsUpdate();
            /*foreach (QString nm, inputs.keys() ) {
                removeInput(nm);
            }
            QJsonObject ob;
            ob.insert("status",QString("input delall"));*/
            QJsonObject ob;
            ob.insert("status",QString("input not implemented yet"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="input/ifttt"){
            /*Blink1Input *bp=new Blink1Input();
            bp->setName(tokens[2].replace("+"," "));
            bp->setType("IFTTT.COM");
            bool test=false;
            if(tokens[3]=="pname"){
                bp->setPatternName(tokens[4]);
                bp->setArg1(tokens[6].replace("+"," "));
                if(tokens.count()==9){
                    test=(tokens[8]=="on" || tokens[8]=="true")?true:false;
                }
            }else{
                bp->setPatternName(tokens[2]);
                bp->setArg1(tokens[4].replace("+"," "));
                if(tokens.count()==7){
                    test=(tokens[6]=="on" || tokens[6]=="true")?true:false;
                }
            }
            bp->setArg2("no-value");
            if(!test){
                inputs.insert(bp->name(),bp);
                emit inputsUpdate();
                QJsonObject ob;
                ob.insert("input",bp->toJson3());
                ob.insert("status",QString("input "+bp->type().toLower()));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }
            counter++;
            if(test){
                checkInput2(bp,client);
            }*/
            QJsonObject ob;
            ob.insert("status",QString("input not implemented yet"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="input/url"){
            /*Blink1Input *bp=new Blink1Input();
            bp->setName(tokens[2].replace("+"," "));
            bp->setType("URL");
            bool test=false;
            if(tokens[3]=="pname"){
                bp->setPatternName(tokens[4]);
                bp->setArg1(tokens[6]);
                if(tokens.count()==9){
                    test=(tokens[8]=="on" || tokens[8]=="true")?true:false;
                }
            }else{
                bp->setPatternName(tokens[2]);
                bp->setArg1(tokens[4]);
                if(tokens.count()==7){
                    test=(tokens[6]=="on" || tokens[6]=="true")?true:false;
                }
            }
            bp->setArg2("no-value");
            if(!test){
                inputs.insert(bp->name(),bp);
                emit inputsUpdate();
                QJsonObject ob;
                ob.insert("input",bp->toJson3());
                ob.insert("status",QString("input "+bp->type().toLower()));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }
            counter++;
            if(test){
                checkInput2(bp,client);
            }*/
            QJsonObject ob;
            ob.insert("status",QString("input not implemented yet"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);

        }else if(request=="input/file"){
            /*Blink1Input *bp=new Blink1Input();
            bp->setName(tokens[2].replace("+"," "));
            bp->setType("FILE");
            bool test=false;
            if(tokens[3]=="pname"){
                bp->setPatternName(tokens[4]);
                bp->setArg1(tokens[6]);
                if(tokens.count()==9){
                    test=(tokens[8]=="on" || tokens[8]=="true")?true:false;
                }
            }else{
                bp->setPatternName(tokens[2]);
                bp->setArg1(tokens[4]);
                if(tokens.count()==7){
                    test=(tokens[6]=="on" || tokens[6]=="true")?true:false;
                }
            }
            bp->setArg2("no-value");
            if(!test){
                inputs.insert(bp->name(),bp);
                emit inputsUpdate();
                QJsonObject ob;
                ob.insert("input",bp->toJson3());
                ob.insert("status",QString("input "+bp->type().toLower()));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }
            counter++;
            if(test){
                checkInput2(bp,client);
            }*/
            QJsonObject ob;
            ob.insert("status",QString("input not implemented yet"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="input/script"){
            /*Blink1Input *bp=new Blink1Input();
            bp->setName(tokens[2].replace("+"," "));
            bp->setType("SCRIPT");
            bool test=false;
            if(tokens[3]=="pname"){
                bp->setPatternName(tokens[4]);
                bp->setArg1(tokens[6]);
                if(tokens.count()==9){
                    test=(tokens[8]=="on" || tokens[8]=="true")?true:false;
                }
            }else{
                bp->setPatternName(tokens[2]);
                bp->setArg1(tokens[4]);
                if(tokens.count()==7){
                    test=(tokens[6]=="on" || tokens[6]=="true")?true:false;
                }
            }
            bp->setArg2("no-value");
            if(!test){
                inputs.insert(bp->name(),bp);
                emit inputsUpdate();
                QJsonObject ob;
                ob.insert("input",bp->toJson3());
                ob.insert("status",QString("input "+bp->type().toLower()));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }
            counter++;
            if(test){
                checkInput2(bp,client);
            }*/
            QJsonObject ob;
            ob.insert("status",QString("input not implemented yet"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="input/cpuload"){
            QJsonObject ob;
            ob.insert("status",QString("input not implemented yet"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="input/netload"){
            QJsonObject ob;
            ob.insert("status",QString("input not implemented yet"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }else if(request=="patterns"){
            QJsonArray qarrp;
            foreach (QString nm, patterns.keys() ) {
                QJsonObject obj = patterns.value(nm)->toJson2();
                qarrp.append(obj);
            }
            QJsonObject jo;
            jo.insert("patterns",qarrp);
            QByteArray ba=QJsonDocument(jo).toJson();
            client->write(ba);
        }else if(request=="pattern/add"){
            if(args.count()==2 && args.at(0).first=="pname" && args.at(1).first=="pattern" && checkIfPatternsStr(args.at(1).second)){
                Blink1Pattern *bp=new Blink1Pattern();
                bp->setName(args.at(0).second);
                bp->fromPatternStrWithLeds(args.at(1).second);
                patterns.insert(bp->name(),bp);
                connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlink(QColor,QString,int)));
                emit patternsUpdate();
                emit patternsUpdate2();
                QJsonObject ob;
                ob.insert("status",QString("pattern add"));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }else{
                invalid=true;
            }
            /*Blink1Pattern *bp=new Blink1Pattern();
            bp->setName(tokens[2]);
            QString tmp=tokens[4].replace("%23","#");
            bp->fromPatternStrWithLeds(tmp);
            patterns.insert(bp->name(),bp);
            connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlink(QColor,QString,int)));
            emit patternsUpdate();
            emit patternsUpdate2();
            QJsonObject ob;
            ob.insert("status",QString("pattern add"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);*/
        }else if(request=="pattern/del"){
            if(args.count()==1 && args.at(0).first=="pname"){
                QString pname=args.at(0).second;
                pname=pname.replace("+"," ");
                removePattern(pname);
                update2();
                QJsonObject ob;
                ob.insert("status",QString("pattern del"));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }else{
                invalid=true;
            }
            /*removePattern(tokens[2]);
            update2();
            QJsonObject ob;
            ob.insert("status",QString("pattern del"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);*/
        }else if(request=="pattern/delall"){
                foreach(QString pname,patterns.keys()){
                    pname=pname.replace("+"," ");
                    removePattern(pname);
                }
                update2();
                QJsonObject ob;
                ob.insert("status",QString("pattern delall"));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
        }else if(request=="pattern/play"){
            if(args.count()==1 && args.at(0).first=="pname"){
                QString pname=args.at(0).second;
                pname=pname.replace("+"," ");
                playPattern2(pname);
                QJsonObject ob;
                ob.insert("status",QString("pattern play"));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }else{
                invalid=true;
            }
            /*playPattern2(tokens[2]);
            QJsonObject ob;
            ob.insert("status",QString("pattern play"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);*/
        }else if(request=="pattern/stop"){
            if(args.count()==1 && args.at(0).first=="pname"){
                QString pname=args.at(0).second;
                pname=pname.replace("+"," ");
                stopPattern(pname);
                QJsonObject ob;
                ob.insert("status",QString("pattern stop"));
                QJsonDocument jd(ob);
                QByteArray ba=jd.toJson();
                client->write(ba);
            }else{
                invalid=true;
            }
            /*stopPattern(tokens[2]);
            QJsonObject ob;
            ob.insert("status",QString("pattern stop"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);*/
        }else{
            QJsonObject ob;
            ob.insert("status",QString("unknown command"));
            QJsonDocument jd(ob);
            QByteArray ba=jd.toJson();
            client->write(ba);
        }
    }else{
        QJsonObject ob;
        ob.insert("status",QString("unknown command"));
        QJsonDocument jd(ob);
        QByteArray ba=jd.toJson();
        client->write(ba);
    }
    if(invalid){
        QJsonObject ob;
        ob.insert("status",QString("invalid command"));
        QJsonDocument jd(ob);
        QByteArray ba=jd.toJson();
        client->write(ba);
    }
    client->close();
}
void MainWindow::discardClient(){
    QTcpSocket *s=(QTcpSocket*)sender();
    if(!s) return;
    clientConnections.removeAll(s);
    s->deleteLater();
}

QList<QObject*> MainWindow::getList(){
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
        patterns.value(key)->stop();
        patterns.remove(key);
        disconnect(tmp);
        delete tmp;
        emit patternsUpdate2();
    }
}
void MainWindow::removeInputAndPattern(QString key){
    inputs.remove(key);
    emit inputsUpdate();
}
void MainWindow::removeInput(QString key,bool update){
    if(inputs.contains(key)){
        Blink1Input *tmp=inputs.value(key);
        inputs.remove(key);
        disconnect(tmp);
        delete tmp;
        if(!mac() || update)
            emit inputsUpdate();
    }
}

void MainWindow::new_input_and_pattern(QString name,QString type,QString rule,QString pname,int repeats){
    Blink1Input *bi=new Blink1Input();
    bi->setName(name);
    bi->setType(type);
    bi->setArg1(rule);
    bi->setPatternName("");
    inputs.insert(name,bi);
    inputsAmount++;
    qDebug()<<pname<<" "<<repeats<<" ";
    addToLog(pname+" "+QString::number(repeats));
}
void MainWindow::edit_input_and_pattern(QString name,QString type,QString rule,QString pname,int repeats,QString old_name){
    Blink1Input *bi=new Blink1Input();
    bi->setName(name);
    bi->setType(type);
    bi->setArg1(rule);
    bi->setPatternName(pname);
    bi->setArg2(inputs.value(name)->arg2());
    bi->setDate(inputs.value(name)->date());
    inputs.insert(name,bi);
    Blink1Pattern *bp=new Blink1Pattern();
    bp->setName(pname);
    bp->setRepeats(repeats);
    patterns.insert(pname,bp);
    inputsAmount++;
    connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlink(QColor,QString,int)));
    if(old_name!=name){
        patterns.remove(old_name);
        inputs.remove(old_name);
    }
}
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
    connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlink(QColor,QString,int)));
    emit patternsUpdate();
    emit patternsUpdate2();
}

void MainWindow::update(){
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

QVariantList MainWindow::getPattern(QString key){
    return patterns.value(key)->getColors();
}
QVariantList MainWindow::getPatternTimes(QString key){
    return patterns.value(key)->getTimes();
}
int MainWindow::getPatternColorSize(QString key){
    return patterns.value(key)->getColors().count();
}
int MainWindow::getPatternRepeats(QString key){
    return patterns.value(key)->repeats();
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
void MainWindow::playPattern(QString name){
    if(!patterns.contains(name)) return;
    if(!patterns.value(name)->playing()) patterns.value(name)->play();
    else patterns.value(name)->stop();
}
void MainWindow::playPattern2(QString name){
    if(!patterns.contains(name)) return;
    if(!patterns.value(name)->playing()) patterns.value(name)->play();
}

void MainWindow::stopPattern(QString name){
    if(!patterns.contains(name)) return;
    patterns.value(name)->stop();
}

void MainWindow::removeColorAndTimeFromPattern(QString name,int idx){
    patterns.value(name)->removeColorAndTime(idx);
}
void MainWindow::changeRepeats(QString name){
    patterns.value(name)->changeRepeats();
}
void MainWindow::changeRepeatsTo(QString name, int to){
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
    while(inputs.contains("Nazwa"+QString::number(counter)))
        counter++;
    bp->setName("Nazwa"+QString::number(counter));
    bp->setType("IFTTT.COM");
    bp->setArg1("RULE");
    bp->setArg2("NO VALUE");
    bp->setPatternName("");
    inputs.insert(bp->name(),bp);
    emit inputsUpdate();
    counter++;
}
void MainWindow::createNewInput(){
    Blink1Input *bp=new Blink1Input();
    while(inputs.contains("Nazwa"+QString::number(counter)))
        counter++;
    bp->setName("Nazwa"+QString::number(counter));
    bp->setType("FILE");
    bp->setArg1("Click double times to change path");
    bp->setArg2("NO VALUE");
    bp->setFreqCounter(1);
    bp->setPatternName("");
    inputs.insert(bp->name(),bp);
    emit inputsUpdate();
    counter++;
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
    emit patternsUpdate2();
}
void MainWindow::update2(){
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
    connect(dI, SIGNAL(setColor(QColor)), this, SLOT(setColor(QColor)));
    connect(dI, SIGNAL(addReceiveEvent(int,QString,QString)), this, SLOT(addRecentEvent(int,QString,QString)));
    dI->setType(inputs[key]->type().toUpper());
    dI->setInput(inputs[key]);
    dI->setPatternList(patterns.keys());
    dI->setIftttKey(iftttKey);
    dI->start();
}
void MainWindow::checkInput2(Blink1Input *in,QTcpSocket *client){
    DataInput *dI = new DataInput(this);
    connect(dI, SIGNAL(toDelete(DataInput*)), this, SLOT(deleteDataInput(DataInput*)));
    connect(dI, SIGNAL(iftttToCheck(QString,Blink1Input*)), this, SLOT(checkIfttt(QString,Blink1Input*)));
    dI->setType(in->type().toUpper());
    dI->setInput(in);
    dI->setPatternList(patterns.keys());
    dI->setIftttKey(iftttKey);
    dI->responseTo=client;
    dI->start();
}
void MainWindow::changeLed(int l){
    this->led=l;
}
bool MainWindow::getIsMk2(){
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
    led=l;
    emit ledsUpdate();
    mode = 1;
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
void MainWindow::mark(){
    closing=false;
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
void MainWindow::markEditing(QString s,bool e){
    if(emails.contains(s)){
        emails.value(s)->setEditing(e);
    }
}
void MainWindow::changePatternReadOnly(QString s, bool ro){
    if(patterns.contains(s)){
        patterns.value(s)->setReadOnly(ro);
        emit patternsUpdate();
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
    connect(bp,SIGNAL(setColor(QColor,QString,int)),this,SLOT(setColorToBlink(QColor,QString,int)));
    emit patternsUpdate();
    emit patternsUpdate2();
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
bool MainWindow::checkIfColor(QString s){
    QRegExp reg("#([0-9a-fA-F]{6})");
    return reg.exactMatch(s);
}
bool MainWindow::checkIfNumber(QString s, int a){
    bool ok;
    if(a==0){
        s.toInt(&ok);
    }else{
        s.toDouble(&ok);
    }
    return ok;
}
bool MainWindow::checkIfPatternsStr(QString s){
    QStringList list=s.split(",");
    if(list.count()==0) return false;
    if(!checkIfNumber(s.at(0),0)) return false;
    int ile=0;
    bool withLeds=false;
    if(list.count()>=4)
        if(checkIfNumber(list.at(3),0))
            withLeds=true;
    for(int i=1;i<list.count();i++){
        if(ile==0){
            if(!checkIfColor(list.at(i))) return false;
        }else if(ile==1){
            if(!checkIfNumber(list.at(i),1)) return false;
        }else if(ile==2){
            if(!checkIfNumber(list.at(i),0)) return false;
        }
        ile++;
        if(!withLeds)
            ile=ile%2;
        else
            ile=ile%3;
    }
    return true;
}
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
int MainWindow::checkWordWidth(QString s,int size){
    QFont f(QFont().defaultFamily(),size);
    return QFontMetrics(f).width(s);
}
