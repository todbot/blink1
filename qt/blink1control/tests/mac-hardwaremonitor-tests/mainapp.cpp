
#include "mainapp.h"

#include <QRegularExpression>
#include <QDebug>

MainApp::MainApp(int &argc, char **argv) 
    : QCoreApplication ( argc, argv ) 
{

};

bool MainApp::checkValue(int value){
    bool result=false;
    if(action==0){
        if(value<lvl)
            result=true;
    }else if(action==1){
        if(value<=lvl)
            result=true;
    }else if(action==2){
        if(value==lvl)
            result=true;
    }else if(action==3){
        if(value>lvl)
            result=true;
    }else if(action==4){
        if(value>=lvl)
            result=true;
    }
    return result;
}

void MainApp::checkBattery()
{
#ifdef Q_OS_MAC
    qDebug() << "checkBattery";
    //if(p3!=NULL) return;
    p3=new QProcess();
    p3->start("pmset -g batt");
    connect(p3,SIGNAL(finished(int)),this,SLOT(readyReadBattery(int)));
    //qDebug() << "checkBattery done";
#endif
}

#ifdef Q_OS_MAC
void MainApp::readyReadBattery(int exitCode) // st == exitcode
{
    qDebug() << "readyReadBattery";
    if( exitCode == 0 ) { 

        QString p3out = p3->readAllStandardOutput();
        //qDebug() << "p3out: "<<p3out;
    
        if( p3out.contains("InternalBattery") ) { 
            QRegularExpression   re("InternalBattery.+\\s(\\d+)%");
            QRegularExpressionMatch match = re.match(p3out);
            if( match.hasMatch() ) {
                QString percentstr = match.captured( match.lastCapturedIndex() );
                qDebug() << "match! " <<  percentstr;
            }
            else { 
                qDebug() << "bad battery read";
            }
        }
        else { 
            qDebug() << "no battery";
        }
        
    } else { 
        qDebug() << "bad exit code "<< exitCode;
    }

    p3->close();
    disconnect(p3);
    p3=NULL;

}
#endif


void MainApp::onError()
{
    qDebug() << "onError";

}


