#include <stdexcept>
#include "jsontools.h"

namespace JsonTools {

QJsonObject lastObject;
QJsonArray lastArray;
QJsonValue lastValue;

// Returns the object with the given name in the array. The
// object needs to have a "name" key, as such.
QJsonObject getObjectInArrayWithName(const QString &name, const QJsonArray &array)
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
            lastObject = object;
            return lastObject;
        }
    }

    throw std::runtime_error("could not find '" + name.toStdString() + "'");
}

// Returns the object with the given key in the object. The
// object needs to have a "name" key, as such.
QJsonObject getObject(const QString &key, const QJsonObject object)
{
    const auto foundValue = object.value(key);
    if (foundValue.isUndefined())
        throw std::runtime_error("could not find '" + key.toStdString() + "'");
    if (!foundValue.isObject())
        throw std::runtime_error("'" + key.toStdString() + "' is not an object!");
    
    lastObject = foundValue.toObject();
    return lastObject;
}

// Returns the array of the given key in the object
QJsonArray getArray(const QString &key, const QJsonObject object)
{
    const auto foundValue = object.value(key);
    if (foundValue.isUndefined())
        throw std::runtime_error("could not find '" + key.toStdString() + "'");
    if (!foundValue.isArray())
        throw std::runtime_error("'" + key.toStdString() + "' is not an array!");
    
    lastArray = foundValue.toArray();
    return lastArray;
}

// Returns the value of the given key in the object. Same as
// object.value(), but throws an exception if not found.
QJsonValue getValue(const QString &key, const QJsonObject object)
{
    const auto foundValue = object.value(key);
    if (foundValue.isUndefined())
        throw std::runtime_error("could not find '" + key.toStdString() + "'");
    
    lastValue = foundValue;
    return lastValue;
}

} // namespace