#include <QtGui>
#include <QtQml>
#include <QtQuick>

#include "jsontools.h"
#include "qtbridgereader.h"
#include "stylegenerator.h"

int main(int argc, char **argv){
    QGuiApplication app(argc, argv);

    // if (argc < 2) {
    //     qDebug() << "Usage: appname <--images-only> [.qtbridge]";
    //     return 0;
    // }

    // const QString fileName(argv[1]);
    const QString fileName(u":/data/testdata.qtbridge"_qs);

    try {
        using namespace JsonTools;

        QtBridgeReader bridgeReader(fileName);
        QJsonDocument doc = bridgeReader.metaData();

        getControlTemplate("ButtonTemplate", doc);
        array("artboards");
        objectInArrayWithName("state=idle");
        array("children");
        objectInArrayWithName("background");
        object("metadata");
        object("assetData");
        value("assetPath");

        qDebug() << "background image:" << lastValue.toString();

    } catch (std::exception &e) {
        qWarning() << e.what();
        return -1;
    }

    // QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    // return app.exec();
    return 0;
}
