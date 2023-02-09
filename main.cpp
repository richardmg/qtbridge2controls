#include <QtGui>
#include <QtQml>
#include <QtQuick>

#include "qtbridgereader.h"

int main(int argc, char **argv){
    QGuiApplication app(argc, argv);

    // if (argc < 2) {
    //     qDebug() << "Usage: appname <--images-only> [.qtbridge]";
    //     return 0;
    // }

    // const QString fileName(argv[1]);
    const QString fileName(u":/figma/style.qtbridge"_qs);

    const QString suffix = QFileInfo(fileName).suffix();
    if (suffix.compare(u"qtbridge"_qs) != 0) {
        qWarning() << "The file is not a .qtbridge file!";
        return -1;
    }

    QtBridgeReader bridgeReader(fileName);
    if (bridgeReader.hasError()) {
        qWarning() << bridgeReader.errorMessage();
        return -1;
    }

    qDebug() << bridgeReader.metaData();

    // QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    // return app.exec();
    return 0;
}
