#include <stdexcept>
#include <QJsonDocument>

namespace JsonTools {

QJsonValue::Type lastType = QJsonValue::Undefined;
QJsonObject lastObject;
QJsonArray lastArray;
QJsonValue lastValue;

// Returns the object with the given name in the array. The
// object needs to have a "name" key, as such.
static QJsonObject objectInArrayWithName(const QString &name, const QJsonArray &array = lastArray)
{
    for (auto it = array.constBegin(); it != array.constEnd(); ++it)
    {
        const auto value = *it;
        if (!value.isObject())
            continue;

        const auto object = value.toObject();
        const auto nameValue = object.value(u"name"_qs);
        if (nameValue.isNull())
            continue;
        const QString foundName = nameValue.toString();
        if (foundName == name) {
            lastType = QJsonValue::Object;
            lastObject = object;
            return lastObject;
        }
    }

    throw std::invalid_argument("could not find '" + name.toStdString() + "'");
}

// Returns the object with the given key in the object. The
// object needs to have a "name" key, as such.
static QJsonObject object(const QString &key, const QJsonObject object = lastObject)
{
    const auto foundValue = object.value(key);
    if (foundValue.isUndefined())
        throw std::invalid_argument("could not find '" + key.toStdString() + "'");
    if (!foundValue.isObject())
        throw std::invalid_argument("'" + key.toStdString() + "' is not an object!");
    
    lastType = QJsonValue::Object;
    lastObject = foundValue.toObject();
    return lastObject;
}

// Returns the array of the given key in the object
static QJsonArray array(const QString &key, const QJsonObject object = lastObject)
{
    const auto foundValue = object.value(key);
    if (foundValue.isUndefined())
        throw std::invalid_argument("could not find '" + key.toStdString() + "'");
    if (!foundValue.isArray())
        throw std::invalid_argument("'" + key.toStdString() + "' is not an array!");
    
    lastType = QJsonValue::Array;
    lastArray = foundValue.toArray();
    return lastArray;
}

// Returns the value of the given key in the object. Same as
// object.value(), but throws an exception if not found.
static QJsonValue value(const QString &key, const QJsonObject object = lastObject)
{
    const auto foundValue = object.value(key);
    if (foundValue.isUndefined())
        throw std::invalid_argument("could not find '" + key.toStdString() + "'");
    
    lastType = QJsonValue::String;
    lastValue = foundValue;
    return lastValue;
}

} // namespace