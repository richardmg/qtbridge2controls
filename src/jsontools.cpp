#include <stdexcept>
#include "jsontools.h"

namespace JsonTools {

QJsonObject lastObject;
QJsonArray lastArray;
QJsonValue lastValue;

QString lastArtboardSetName;
QString lastArtboardChildName;

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

    throw std::runtime_error("could not find object with name '" + name.toStdString() + "' in array!");
}

// Returns the object with the given key in the object. The
// object needs to have a "name" key, as such.
QJsonObject getObject(const QString &key, const QJsonObject object)
{
    const auto foundValue = object.value(key);
    if (foundValue.isUndefined())
        throw std::runtime_error("could not find object '" + key.toStdString() + "'");
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
        throw std::runtime_error("could not find array '" + key.toStdString() + "'");
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
        throw std::runtime_error("could not find value '" + key.toStdString() + "'");
    
    lastValue = foundValue;
    return lastValue;
}

/**
 * Get the json object that points to the root of the
 * tree that describes a control (quick) / template (figma) / artboard (qtbridge)
*/
QJsonObject getArtboardSet(const QString &artboardSetName, const QJsonDocument &doc)
{
    // TODO: When using real-life data, 'artboardSets' will probably not
    // be on the root node, so this function will need to be adjusted!
    lastArtboardSetName = artboardSetName;
    getArray("artboardSets", doc.object());
    return getObjectInArrayWithName(artboardSetName);
}

/**
 * \a object should point to an artboard set (e.g ButtonTemplate). This function will
 * then return the child artboard with the name that matches \a state.
*/
QJsonObject getArtboardWithState(const QString &state, const QJsonObject object)
{
    getArray("artboards", object);
    return getObjectInArrayWithName(QString("state=") + state);
}

/**
 * 'object' should point to an artboard (e.g an object that describes a control
 * for a certain state). This function will then return the child in the
 * children array with the 'name: name'.
*/
QJsonObject getArtboardChildWithName(const QString &name, const QJsonObject object)
{
    lastArtboardChildName = name;
    getArray("children", object);
    return getObjectInArrayWithName(name);
}

/**
 * Nodes with an image will store the path to that image inside the metadata
 * Since this strucutre seems to be the same for all nodes, it's factored out
 * to this function.
 * \sa https://www.qt.io/blog/qt-bridge-metadata-format
*/
QString getImagePathInMetaData(const QJsonObject object)
{
    getObject("metadata", object);
    getObject("assetData");
    return getValue("assetPath").toString();
}

} // namespace