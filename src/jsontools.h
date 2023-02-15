#include <stdexcept>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace JsonTools
{

// Whenever one of the search functions below are called, the
// return value will also be stored in one of the following variables.
extern QJsonObject lastObject;
extern QJsonArray lastArray;
extern QJsonValue lastValue;

QJsonObject getObjectInArrayWithName(const QString &name, const QJsonArray &array = lastArray);
QJsonObject getObject(const QString &key, const QJsonObject object = lastObject);
QJsonArray getArray(const QString &key, const QJsonObject object = lastObject);
QJsonValue getValue(const QString &key, const QJsonObject object = lastObject);

} // namespace