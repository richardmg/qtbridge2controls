#include <QtCore>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

#include "stylegenerator.h"
#include "jsontools.h"

Q_LOGGING_CATEGORY(lcStyleGenerator, "qt.stylegenerator")

using namespace JsonTools;

namespace StyleGenerator {

QString resourcePath;
QString styleDir;

/**
 * Sets the patch to the resource directory inside
 * the unzipped qtbridge folder.
 */
void setResourcePath(const QString &path)
{
    resourcePath = path;
}

/**
 * Sets the path to the target style folder
 * that should be created.
*/
void setTargetPath(const QString path)
{
    styleDir = path;
    if (!QDir().mkpath(path))
        throw std::runtime_error("Could not create target path: " + path.toStdString());
}

/**
 * Copies the given srcName image from the unzipped
 * qtbridge folder and into the target style folder.
 */
void copyImage(const QString srcName, const QString targetName)
{
    const QString srcFilePath = resourcePath + '/' + srcName;
    const QString targetFilePath = styleDir + "/" + targetName;
    qCDebug(lcStyleGenerator) << "  copy" << srcFilePath << "to" << targetFilePath;
    QFile srcFile = QFile(srcFilePath);
    if (!srcFile.exists())
        throw std::runtime_error("File doesn't exist: " + srcFilePath.toStdString());
    srcFile.copy(targetFilePath);
}

/**
 * Get the json object that points to the root of the
 * tree that describes a control / template
*/
QJsonObject getTemplateRootObject(const QString &templateName, const QJsonDocument &doc)
{
    // TODO: When using real-life data, 'artboardSets' will probably not
    // be on the root node, so this function will need to be adjusted!
    getArray("artboardSets", doc.object());
    return getObjectInArrayWithName(templateName);
}

/**
 * Get the file name (inside the unzipped qtbridget file) of the
 * image that is produced for the given state.
*/
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

/**
 * Resolve the image file name for the current state, and copy the
 * image into the style folder using the given targetFileNameBase and fileNameState.
*/
void generateImage(const QString &targetFileNameBase
    , const QString fileNameState
    , const QString &state
    , const QJsonObject templateObject)
{
    qCDebug(lcStyleGenerator) << " generate image for state" << state;
    const QString objectName = QString("state=") + state;
    const QString srcName = getImageFileName(templateObject, objectName);
    const QString targetName = targetFileNameBase + "-" + fileNameState + ".svg";
    copyImage(srcName, targetName);
}

/**
 * Convenience function for the cases where the state in the filename
 * should be the same as the state in the json file.
*/
void generateImage(const QString &targetFileNameBase
    , const QString &state
    , const QJsonObject templateObject)
{
    generateImage(targetFileNameBase, state, state, templateObject);
}

void generateImages(const QString &targetFileNameBase, const QJsonObject templateObject)
{
    generateImage(targetFileNameBase, "pressed", templateObject);
    generateImage(targetFileNameBase, "hovered", templateObject);
    generateImage(targetFileNameBase, "checked", templateObject);
    generateImage(targetFileNameBase, "", "idle", templateObject);
}

void generateButton(const QJsonDocument &doc)
{
    qCDebug(lcStyleGenerator) << "starting to generate button";

    const QJsonObject buttonTemplate = getTemplateRootObject("ButtonTemplate", doc);
    generateImages("button-background", buttonTemplate);
}

} // namespace