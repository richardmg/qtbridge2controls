#include <QJsonDocument>
#include <QJsonObject>

#include "stylegenerator.h"
#include "jsontools.h"

using namespace JsonTools;

QJsonObject getControlTemplate(const QString &templateName, const QJsonDocument &doc)
{
    // TODO: When using real-life data, 'artboardSets' will probably not
    // be on the root node, so this function will need to be adjusted!
    array("artboardSets", doc.object());
    return objectInArrayWithName(templateName);
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
