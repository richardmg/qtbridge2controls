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
        const auto artboardSets = array("artboardSets", json.object());
        const auto buttonTemplate = object("ButtonTemplate", artboardSets);
        const auto artboards = array("artboards", buttonTemplate);
        const auto state_idle = object("state=idle", artboards);
        const auto children = array("children", state_idle);
        const auto background = object("background", children);
        const auto metadata = object("metadata", background);
        const auto assetData = object("assetData", metadata);
        const auto assetPath = value("assetPath", assetData);

        qDebug() << "background image:" << assetPath.toString();

    } catch (std::exception &e) {
        qWarning() << "Failed to parse qtbridge:" << e.what();
        return -1;
    }

    // QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    // return app.exec();
    return 0;
}
