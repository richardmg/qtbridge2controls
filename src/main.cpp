#include <QtGui>

#include "jsontools.h"
#include "qtbridgereader.h"
#include "stylegenerator.h"

using namespace StyleGenerator;

int main(int argc, char **argv){
    QGuiApplication app(argc, argv);

    // if (argc < 2) {
    //     qDebug() << "Usage: appname <--images-only> [.qtbridge]";
    //     return 0;
    // }

    // const QString fileName(argv[1]);
    const QString fileName(u":/data/testdata.qtbridge"_qs);
    const QString targetPath("myStyle");

    try {

        const QtBridgeReader bridgeReader(fileName);
        const QJsonDocument doc = bridgeReader.metaData();

        setResourcePath(bridgeReader.unzippedPath());
        setTargetPath(targetPath);
        generateButton(doc);

    } catch (std::exception &e) {
        qWarning() << "ERROR:" << e.what();
        return -1;
    }

    return 0;
}
