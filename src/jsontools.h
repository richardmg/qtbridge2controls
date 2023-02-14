#include <stdexcept>
#include <QJsonDocument>

namespace JsonTools {

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

// Returns the object with the given name in the array. The
// object needs to have a "name" key, as such.
static QJsonObject object(const QString &name, const QJsonArray &array)
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
        if (foundName == name)
            return object;
    }

    throw std::invalid_argument("could not find '" + name.toStdString() + "'");
}

// Returns the object with the given name in the object. The
// object needs to have a "name" key, as such.
static QJsonObject object(const QString &name, const QJsonObject object)
{
    const auto foundValue = object.value(name);
    if (foundValue.isUndefined())
        throw std::invalid_argument("could not find '" + name.toStdString() + "'");
    if (!foundValue.isObject())
        throw std::invalid_argument("'" + name.toStdString() + "' is not an object!");
    return foundValue.toObject();
}

// Returns the array of the given key in the object
static QJsonArray array(const QString &name, const QJsonObject object)
{
    const auto foundValue = object.value(name);
    if (foundValue.isUndefined())
        throw std::invalid_argument("could not find '" + name.toStdString() + "'");
    if (!foundValue.isArray())
        throw std::invalid_argument("'" + name.toStdString() + "' is not an array!");
    return foundValue.toArray();
}

// Returns the value of the given key in the object. Same as
// object.value(), but throws an exception if not found.
static QJsonValue value(const QString &name, const QJsonObject object)
{
    const auto foundValue = object.value(name);
    if (foundValue.isUndefined())
        throw std::invalid_argument("could not find '" + name.toStdString() + "'");
    return foundValue;
}

} // namespace