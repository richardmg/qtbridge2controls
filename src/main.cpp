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
    const QString fileName(u":/data/testdata.qtbridge"_qs);

    QtBridgeReader bridgeReader(fileName);
    if (bridgeReader.hasError()) {
        qWarning() << bridgeReader.errorMessage();
        return -1;
    }

    QJsonDocument json = bridgeReader.metaData();
    qDebug() << json;

    // QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    // return app.exec();
    return 0;
}
