#include <QtCore>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

#include "stylegenerator.h"
#include "jsontools.h"

using namespace JsonTools;

namespace StyleGenerator {

static QString resourcePath;
static QString styleDir;
static bool verboseOptionSet = false;

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

void setVerbose(bool verbose)
{
    verboseOptionSet = verbose;
}

void debug(const QString &msg = "")
{
    if (!verboseOptionSet)
        return;
    if (msg.isEmpty())
        qDebug().noquote() << "\n";
    else
        qDebug() << msg;
}

/**
 * Copies the given file into the style folder.
 * If destFileName is empty, the file name of the src will be used.
 */
void copyFileToStyleFolder(const QString srcPath, const QString destFileName = "")
{
    QFile srcFile = QFile(srcPath);
    if (!srcFile.exists())
        throw std::runtime_error("File doesn't exist: " + srcPath.toStdString());
    QString targetName = destFileName;
    if (targetName.isEmpty())
        targetName = QFileInfo(srcFile).fileName();
    const QString targetPath = styleDir + "/" + targetName;
    debug("copying " + srcPath + " to " + targetPath);
    srcFile.copy(targetPath);
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
 * image into the style folder using the given baseName and fileNameState.
*/
void generateImage(const QString &baseName
    , const QString fileNameState
    , const QString &jsonState
    , const QJsonObject templateObject)
{
    debug("generating image for state " + jsonState);
    try {

        const QString objectName = QString("state=") + jsonState;
        const QString srcName = getImageFileName(templateObject, objectName);
        // Require the images to be png for now. While we could convert svg's to
        // png's on the fly, we should rather investigate how we can do this during build
        // time (with the work done to create png icons from svg from cmake).
        if (!srcName.endsWith(".png"))
            throw std::runtime_error("The image needs to be png: " + srcName.toStdString());

        const QString srcPath = resourcePath + '/' + srcName;
        const QString targetName = baseName + fileNameState + ".png";
        copyFileToStyleFolder(srcPath, targetName);

    } catch (std::exception &e)
    {
        qWarning() << "Warning:" << e.what();
    }
}

/**
 * Convenience function for the cases where the state in the filename
 * should be the same as the state in the json file.
*/
void generateImage(const QString &baseName
    , const QString &state
    , const QJsonObject templateObject)
{
    generateImage(baseName, "-" + state, state, templateObject);
}

void generateImages(const QString &baseName, const QJsonObject templateObject)
{
    // states: disabled, pressed, checked, checkable, focused, highlighted, flat, mirrored, hovered
    generateImage(baseName, "pressed", templateObject);
    generateImage(baseName, "checked", templateObject);
    generateImage(baseName, "hovered", templateObject);

    // TODO: For the remaining states, there is a mismatch between the name of
    // the state in the imagine style and the name of the state in the figma
    // template (which we should fix in the template)!
    generateImage(baseName, "-disabled", "blocked", templateObject);
    generateImage(baseName, "", "idle", templateObject);

    // TODO: The following states have no design in Figma yet:
    // checkable, focused, highlighted, flat, mirrored, (and dark mode)
    // They should follow the naming in
    // https://doc.qt.io/qt-6/qtquickcontrols2-imagine.html
}

void generateQmlDir()
{
    const QString styleName = QFileInfo(styleDir).fileName();
    QString qmldir;
    qmldir += "module " + styleName + "\n";
    qmldir += "Button 1.0 Button.qml\n";
    const QString path = styleDir + "/qmldir";
    debug("generating qmldir: " + path);
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
        throw std::runtime_error("Could not open file for writing: " + path.toStdString());

    QTextStream out(&file);
    out << qmldir;
}

void generateButton(const QJsonDocument &doc)
{
    debug();
    debug("generating button");

    copyFileToStyleFolder(":/Button.qml");
    const QJsonObject buttonTemplate = getTemplateRootObject("ButtonTemplate", doc);
    generateImages("button-background", buttonTemplate);
}

void generateStyle(const QJsonDocument &doc)
{
    generateButton(doc);
    generateQmlDir();
}

} // namespace