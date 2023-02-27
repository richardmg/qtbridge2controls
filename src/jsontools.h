#include <stdexcept>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace JsonTools
{

// Json general API.
// Whenever one of the search functions below are called, the
// return value will also be stored in one of the following variables.
extern QJsonObject lastObject;
extern QJsonArray lastArray;
extern QJsonValue lastValue;

QJsonObject getObject(const QString &key, const QJsonObject object = lastObject);
QJsonArray getArray(const QString &key, const QJsonObject object = lastObject);
QJsonValue getValue(const QString &key, const QJsonObject object = lastObject);

// QtBridge specific API.
extern QString lastArtboardSetName;
extern QString lastArtboardChildName;

QJsonObject getArtboardSet(const QString &templateName, const QJsonDocument &doc);
QJsonObject getArtboardWithState(const QString &state, const QJsonObject object = lastObject);
QJsonObject getArtboardChildWithName(const QString &name, const QJsonObject object = lastObject);
QJsonObject getObjectInArrayWithName(const QString &name, const QJsonArray &array = lastArray);
QString getImagePathInMetaData(const QJsonObject object = lastObject);

} // namespace