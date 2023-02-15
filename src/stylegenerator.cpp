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

QString styleDir;

void createStyleDirectory(const QString path)
{
    styleDir = path;
    if (!QDir().mkpath(path))
        throw std::runtime_error("Could not create style directory: " + path.toStdString());
}

void generateButton(const QJsonDocument &doc, const QString &resourcePath)
{
    getControlTemplate("ButtonTemplate", doc);
    array("artboards");
    objectInArrayWithName("state=idle");
    array("children");
    objectInArrayWithName("background");
    object("metadata");
    object("assetData");
    value("assetPath");

    const QString bgFileName = lastValue.toString();
    const QString bgPath = resourcePath + '/' + bgFileName;
    QFile bgFile = QFile(bgPath);
    if (!bgFile.exists())
        throw std::runtime_error("File doesn't exist: " + bgPath.toStdString());

    bgFile.copy(styleDir + u"/buttonBackground.svg"_qs);
}

} // namespace