#include <QtGui>

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

        QtBridgeReader bridgeReader(fileName);
        QJsonDocument doc = bridgeReader.metaData();

        using namespace StyleGenerator;
        generateButton(doc);

    } catch (std::exception &e) {
        qWarning() << e.what();
        return -1;
    }

    return 0;
}
