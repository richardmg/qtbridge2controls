#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include <QtGui/private/qzipreader_p.h>

QString unzipDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation)
        + "/" + qApp->applicationName();
}

bool extractBridgeFile(const QString &path)
{
    const QFileInfo fileInfo(path);
    const QString suffix = fileInfo.suffix();

    if (suffix.compare(u"qtbridge"_qs) != 0) {
        qDebug() << "The file is not a .qtbridge file!";
        return false;
    }

    QZipReader zip(path);
    if (!zip.isReadable()) {
        qDebug() << "Could not read:" << path;
        return false;
    }

    const QString destDir = unzipDir();
    if (!QDir().mkpath(destDir)) {
        qDebug() << "Could not create tmp path:" << destDir;
        return false;
    }

    if (!zip.extractAll(destDir)) {
        qDebug() << "Could not extract bridge file:" << zip.status();
        return false;
    }

    return true;
}

bool readMetadata()
{
    // Read json file:
    const QString dir = unzipDir();
    QFile metadata(dir +  "/" + "Learning_Figma.metadata");
    if (!metadata.exists()) {
        qDebug() << "Could not find .metadata file!";
        return false;
    }

    if (!metadata.open(QFile::ReadOnly)) {
        qDebug() << "Could not find open .metadata file for reading!";
        return false;
    }

    qDebug() << metadata.readAll();

    return true;
}

int main(int argc, char **argv){
    QGuiApplication app(argc, argv);

    // if (argc < 2) {
    //     qDebug() << "Usage: appname <--images-only> [.qtbridge]";
    //     return 0;
    // }

    // const QString fileName(argv[1]);
    const QString fileName(u":/figma/style.qtbridge"_qs);

    if (!extractBridgeFile(fileName))
        return -1;

    if (!readMetadata())
        return -1;

    // QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    // return app.exec();
    return 0;
}
