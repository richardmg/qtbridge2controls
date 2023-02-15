#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

#include "stylegenerator.h"
#include "jsontools.h"

using namespace JsonTools;

namespace StyleGenerator {

QJsonObject getControlTemplate(const QString &templateName, const QJsonDocument &doc)
{
    // TODO: When using real-life data, 'artboardSets' will probably not
    // be on the root node, so this function will need to be adjusted!
    array("artboardSets", doc.object());
    return objectInArrayWithName(templateName);
}

QString styleFolder;

void generateStyleFolder(const QString path)
{
    styleFolder = path;
    if (!QDir().mkpath(path))
        throw std::invalid_argument("Could not create style directory: " + path.toStdString());
}

void generateButton(const QJsonDocument &doc)
{
    getControlTemplate("ButtonTemplate", doc);
    array("artboards");
    objectInArrayWithName("state=idle");
    array("children");
    objectInArrayWithName("background");
    object("metadata");
    object("assetData");
    value("assetPath");

    qDebug() << "background image:" << lastValue.toString();
}

} // namespace