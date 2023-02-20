#include <QtGui>

#include "jsontools.h"
#include "qtbridgereader.h"
#include "stylegenerator.h"

using namespace StyleGenerator;

int main(int argc, char **argv){
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Creates a Qt Quick Controls style from a .qtbridge file.");
    parser.addHelpOption();
    parser.addOptions({
        {{"d", "directory"},
            QCoreApplication::translate("main", "The target directory where the style will be created."),
            QCoreApplication::translate("main", "directory"),
            "."},
        {{"v", "verbose"},
            QCoreApplication::translate("main", "Debug out what gets generated.")}
    });
    parser.addPositionalArgument("qtbridge",
        QCoreApplication::translate("main", "The .qtbridge file to create a style from."));

    if (!parser.parse(QCoreApplication::arguments())) {
        qWarning() << parser.errorText();
        return -1;
    }

    setVerbose(parser.isSet("verbose"));

    if (parser.positionalArguments().length() != 1) {
        parser.showHelp();
        return -1;
    }

    const QString src = parser.positionalArguments().first();
    const QString dest = parser.value("d");

    try {

        const QtBridgeReader bridgeReader(src);
        const QJsonDocument doc = bridgeReader.metaData();

        setResourcePath(bridgeReader.unzippedPath());
        setTargetPath(dest);
        generateStyle(doc);

    } catch (std::exception &e) {
        qWarning() << "Error:" << e.what();
        return -1;
    }

    return 0;
}
