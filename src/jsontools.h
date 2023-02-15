#include <stdexcept>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace JsonTools
{

extern QJsonObject lastObject;
extern QJsonArray lastArray;
extern QJsonValue lastValue;

QJsonObject objectInArrayWithName(const QString &name, const QJsonArray &array = lastArray);
QJsonObject object(const QString &key, const QJsonObject object = lastObject);
QJsonArray array(const QString &key, const QJsonObject object = lastObject);
QJsonValue value(const QString &key, const QJsonObject object = lastObject);

} // namespace