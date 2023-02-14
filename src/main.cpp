#include <QtGui>
#include <QtQml>
#include <QtQuick>

#include "jsontools.h"
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

    try {
        // TODO: When using real-life data, 'artboardSets' will probably not
        // be on the root node, so this will need to be adjusted!
        using namespace JsonTools;

        array("artboardSets", json.object());
        objectInArrayWithName("ButtonTemplate");
        array("artboards");
        objectInArrayWithName("state=idle");
        array("children");
        objectInArrayWithName("background");
        object("metadata");
        object("assetData");
        value("assetPath");

        qDebug() << "background image:" << lastValue.toString();

    } catch (std::exception &e) {
        qWarning() << "Failed to parse qtbridge:" << e.what();
        return -1;
    }

    // QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    // return app.exec();
    return 0;
}
