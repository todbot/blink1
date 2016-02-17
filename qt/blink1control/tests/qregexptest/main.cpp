#include <QCoreApplication>

#include <QRegularExpression>
#include <QColor>
#include <QFile>
#include <QDebug>


QString readColorPatternOld(QString str)
{
    QString patt;
    // try match json: '{"pattern":"my_pattern_name"}'
    QRegExp rx("\"?pattern\"?\\s*:\\s*\"(.+)\""); 
    int pos = rx.indexIn(str);
    if( pos != -1 ) { // match
        //qDebug() << "readColorPattern: match1";
        patt = rx.cap(1);
    }
    else {  // not json so try format: 'pattern: "my pattern name"'
        QRegExp rx2("pattern\\s*:\\s*(.+)\\s");
        pos = rx2.indexIn(str);
        if( pos != -1 ) { 
            //qDebug() << "readColorPattern: match2";
            patt = rx2.cap(1);
        }
    }
    //patt.replace("\"",""); // shouldn't capture doublequotes but does
    patt.remove(QRegExp("^\\s+\"*|\"*\\s+$")); // stupid qregexp doesn't allow non-greedy text capture
    return patt;
}
/**
 * Read potential color code in string
 * @param str string to parse
 * @return valid QColor or invalid QColor if parsing failed
 */
QColor readColorCodeOld(QString str)
{
    QColor c;
    QRegExp rx("(#[A-Fa-f0-9]{6})"); // look for "#cccccc" style hex colorcode
    if( rx.indexIn(str) != -1 ) { 
        //qDebug() << "color match! " << rx.cap(1);
        c.setNamedColor( rx.cap(1) );
    }
    return c;
}


QString readColorPattern(QString str)
{
    QString patt;
    QRegularExpression   re("\"?pattern\"?:\\s*((\"(.+)\")|((.+)\\s))");
    QRegularExpressionMatch match = re.match(str);
    if( match.hasMatch() ) {
        patt = match.captured( match.lastCapturedIndex() );
    }
    return patt;
}
QColor readColorCode(QString str)
{
    QColor c;
    QRegularExpression re("(#[A-Fa-f0-9]{6})"); // look for "#cccccc" style hex colorcode
    QRegularExpressionMatch match = re.match(str);
    if( match.hasMatch() ) {
        c.setNamedColor( match.captured( match.lastCapturedIndex()) );
    }
    return c;
}

/**
 * Given a string, parse either a color pattern or color code
 * and trigger system based on that.
 *
 * @param str string to parse
 * @param type type of monitor running ("ifttt", "url", "file", "script")
 * @param lastModTime time parsing occurred
 */
void parsePatternOrColor(QString str) 
{
    // look for pattern
    QString patternName = readColorPattern( str );
    bool patternFound = !patternName.isEmpty();

    if( patternFound ) { 
        qDebug() << "pattern found: " << patternName;
    } 
    else { // look for hex color
        QColor c = readColorCode( str );
        bool colorFound = c.isValid();
        if( colorFound ) { 
            QString colorstr = c.name().toUpper();
            qDebug() << "color found: "<< colorstr;
        }
        else { 
            qDebug() << "parsePatternOrColor no color found";
        }
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString str;

    if( argc != 2 ) {
        printf("usage: qregextest <filename>\n");
        return -1;
    }
    qDebug() << "qregexptest!\n";

    QString fname = QString(argv[1]);
    QFile f(fname);
    if( fname=="-" ) {  
        f.open(stdin, QIODevice::ReadOnly | QIODevice::Text);
    }
    else if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf("bad file: %s\n", qPrintable(fname));
        return -1;
    }
    QTextStream in(&f);
    str.append(in.readAll());

#if 1

    parsePatternOrColor(str);

#else
    fprintf( stdout, "in str='%s'\n\n", qPrintable(str) );

    QString pstr = readColorPattern( str );
    fprintf( stdout, "patt str='%s'\n\n", qPrintable(pstr) );

    QColor colr = readColorCode( str );
    fprintf( stdout, "color str='%s' (%s)\n\n", qPrintable(colr.name()), (colr.isValid())?"valid":"invalid" );
#endif

    qDebug();
    return 0;
    //return a.exec();
}
