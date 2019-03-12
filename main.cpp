#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QDir>

#define SOURCE_FILE "1629936492_0_v1_1545203239277.csv"
#define SOURCE_DIR "C:\\Users\\Robert\\Desktop\\Cplusplus\\IlluminationLog"

static QFile outFile;
static QString strCurrentHour = "";
static QDateTime dateCurrent;
static double dLuxInHour = 0.0;
static int iCurrentCountMeasure = 0;
static bool bNeedWrite = false;
static double dMiddleLux;


void process_line(QString input)
{

    QStringList qsl = input.split(',');

    if(qsl.size() == 3) {

        // Illumination
        double dLux = qsl.at(0).toDouble();
        QString qsTemp; // = QString::number(dLux,'f',6);

        // Current time
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(qsl.at(1).toLongLong()));
        QString strHour = dt.toString("HH");

        if (strCurrentHour != strHour) {
            if (iCurrentCountMeasure == 60) { // First time or full 60 minuts
                dMiddleLux = dLuxInHour / iCurrentCountMeasure;
                bNeedWrite = true;
                qsTemp.sprintf("%012.6f;%s\n", dMiddleLux, dateCurrent.toString("dd.MM.yyyy HH:mm").toStdString().c_str());
            }
            dateCurrent = dt;
            strCurrentHour = strHour;
            iCurrentCountMeasure = 0;
            dLuxInHour = 0.0;
        }

        dLuxInHour += dLux;
        iCurrentCountMeasure++;

        if (bNeedWrite) {
            outFile.write(qPrintable(qsTemp));
            bNeedWrite = false;
        }
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString message;
    //if (argc==2){
    QString qsFileName = SOURCE_FILE; //argv[1];
    QFile file(qsFileName);

    QString strCurrDir = QDir::currentPath();
    qDebug() << strCurrDir;
    if (!QDir::setCurrent(QStringLiteral(SOURCE_DIR))) { // Need change for real directory
        message = "Can't change the current working directory to %1";
        message = message.arg(SOURCE_DIR);
        qDebug() << message;
        return -1;
    }
    qDebug() << QDir::currentPath();

    if (!file.exists()){
        message = "Can't find file %1 to read";
        message = message.arg(qsFileName);
        qDebug() << message;
        return -1;
    }

    if(!file.open(QFile::ReadOnly | QFile::Text))  {
        message = "Can't open the file %1 for reading";
        message = message.arg(qsFileName);
        qDebug() << message;
        return -1;
    }

    QString outFileName = qsFileName + ".log";
    outFile.setFileName(outFileName);
    outFile.open(QFile::WriteOnly | QFile::Text); //QFile::Append | QFile::Text);

    message = "Start parsing the file %1, write output to file %2";
    message = message.arg(qsFileName).arg(outFileName);
    qDebug() << message;

    QString line;

    while (!file.atEnd()) {
        line = file.readLine().trimmed();

        process_line(line);
    }

    line = "0,0,0";
    process_line(line);

    outFile.close();
    message = "Finish parsing the file %1, write output to file %2";
    message = message.arg(qsFileName).arg(outFileName);
    qDebug() << message;
    return 0;
    //}
    /*
    message = "Enter input csv file name";
    qDebug() << message;
    */
    //return a.exec();
}
