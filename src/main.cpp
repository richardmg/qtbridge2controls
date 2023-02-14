#include <QtGui>
#include <QtQml>
#include <QtQuick>

#include "qtbridgereader.h"

// Find the first object (depth-first) that has the given key, and return its value
QJsonValue findJsonValue(const QString &key, const QJsonValue &fromValue)
{
    if (fromValue.isArray()) {
        const auto array = fromValue.toArray();
        for (auto it = array.constBegin(); it != array.constEnd(); ++it) {
            const auto value = *it;
            const auto resultValue = findJsonValue(key, value);
            if (!resultValue.isNull())
                return resultValue;
        }
    } else if (fromValue.isObject()) {
        const auto objectValue = fromValue.toObject();
        for (auto it = objectValue.constBegin(); it != objectValue.constEnd(); ++it) {
            const auto value = *it;
            if (it.key() == key)
                return objectValue;
            if (value.isObject() || value.isArray()) {
                const auto resultObject = findJsonValue(key, value);
                if (!resultObject.isNull())
                    return resultObject;
            }
        }
    }

    return QJsonObject();
}

// Find the first object with the given qmlId
QJsonObject findJsonObjectFromQmlId(const QString &qmlId, const QJsonValue &fromValue)
{
    if (fromValue.isArray()) {
        const auto array = fromValue.toArray();
        for (auto it = array.constBegin(); it != array.constEnd(); ++it) {
            const auto value = *it;
            const auto resultValue = findJsonObjectFromQmlId(qmlId, value);
            if (!resultValue.isEmpty())
                return resultValue;
        }
    } else if (fromValue.isObject()) {
        const auto objectValue = fromValue.toObject();
        for (auto it = objectValue.constBegin(); it != objectValue.constEnd(); ++it) {
            const auto value = *it;
            if (it.key() == u"qmlId"_qs && value.isString() && value.toString() == qmlId)
                return objectValue;
            if (value.isObject() || value.isArray()) {
                const auto resultObject = findJsonObjectFromQmlId(qmlId, value);
                if (!resultObject.isEmpty())
                    return resultObject;
            }
        }
    }

    return QJsonObject();
}

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
    Q_ASSERT(!json.isArray());

    const auto artboardSets = findJsonValue(u"artboardSets"_qs, json.object());

    const auto buttonTemplate = findJsonObject(u"buttonTemplate"_qs, json.object());
    if (buttonTemplate.isEmpty()) {
        qWarning() << "Could not find 'buttonTemplate'";
        return -1;
    }

    const auto state_idle = findJsonObject(u"state_idle"_qs, buttonTemplate);
    if (state_idle.isEmpty()) {
        qWarning() << "Could not find 'state_idle'";
        return -1;
    }

    qDebug() << "state_idle:" << state_idle[u"name"_qs];

// TODO: først finn ArtBoardSets. Og dette er ikke qmlId !
    // TODO: implement try/catch

    // QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    // return app.exec();
    return 0;
}
