#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include <QtGui/private/qzipreader_p.h>

int main(int argc, char **argv){
    QGuiApplication app(argc, argv);

    if (argc < 2) {
        qDebug() << "Usage: appname <--images-only> [.qtbridge]";
        return 0;
    }

    const QString fileName(argv[1]);
    const QFileInfo fileInfo(fileName);
    const QString suffix = fileInfo.suffix();

    // const QFile file(QString(argv[1]));
   
    // qDebug() << "filename:" << file.fileName();

    // QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    // return app.exec();
    return 0;
}
