#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

#include "stylegenerator.h"
#include "jsontools.h"

using namespace JsonTools;

namespace StyleGenerator {

QString resourcePath;
QString styleDir;

void setResourcePath(const QString &path)
{
    resourcePath = path;
}

void setTargetPath(const QString path)
{
    styleDir = path;
    if (!QDir().mkpath(path))
        throw std::runtime_error("Could not create target path: " + path.toStdString());
}

QJsonObject getTemplateObject(const QString &templateName, const QJsonDocument &doc)
{
    // TODO: When using real-life data, 'artboardSets' will probably not
    // be on the root node, so this function will need to be adjusted!
    getArray("artboardSets", doc.object());
    return getObjectInArrayWithName(templateName);
}

QString getImageFileName(const QJsonObject &templateObject, const QString &state)
{
    getArray("artboards", templateObject);
    getObjectInArrayWithName(state);
    getArray("children");
    getObjectInArrayWithName("background");
    getObject("metadata");
    getObject("assetData");
    return getValue("assetPath").toString();
}

void copyImage(const QString srcName, const QString targetName)
{
    const QString srcFilePath = resourcePath + '/' + srcName;
    QFile srcFile = QFile(srcFilePath);
    if (!srcFile.exists())
        throw std::runtime_error("File doesn't exist: " + srcFilePath.toStdString());
    srcFile.copy(styleDir + "/" + targetName);
}

void generateButton(const QJsonDocument &doc)
{
    const QJsonObject buttonTemplate = getTemplateObject("ButtonTemplate", doc);
    const QString bgFileName = getImageFileName(buttonTemplate, "state=idle");
    copyImage(bgFileName, "buttonBackground.svg");
}

} // namespace