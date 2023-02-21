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

typedef QString (*JsonSearchFunction)(const QString &state, const QJsonObject &root);

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
    QFileInfo targetPathInfo(targetPath);
    const QDir targetDir = targetPathInfo.absoluteDir();
    if (!targetDir.exists()) {
        if (!QDir().mkpath(targetDir.path()))
            throw std::runtime_error("Could not create target path: " + targetPath.toStdString());
    }

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

void copyImageToStyleFolder(const QString &baseName, const QString state, const QString srcName)
{
    // Require the images to be png for now. While we could convert svg's to
    // png's on the fly, we should rather investigate how we can do this during build
    // time (with the work done to create png icons from svg from cmake).
    if (!srcName.endsWith(".png"))
        throw std::runtime_error("The image needs to be png: " + srcName.toStdString());

    const QString srcPath = resourcePath + '/' + srcName;
    const QString targetName = "images/" + baseName + "-" + state + ".png";
    copyFileToStyleFolder(srcPath, targetName);
}

void generateQmlDir()
{
    const QString path = styleDir + "/qmldir";
    const QString styleName = QFileInfo(styleDir).fileName();
    debug("generating qmldir: " + path);

    QString qmldir;
    qmldir += "module " + styleName + "\n";
    qmldir += "Button 1.0 Button.qml\n";
    qmldir += "CheckBox 1.0 CheckBox.qml\n";

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
        throw std::runtime_error("Could not open file for writing: " + path.toStdString());

    QTextStream out(&file);
    out << qmldir;
}

template <typename SearchFunction>
void generateImages(
    const QString &baseName,
    const QStringList &states,
    SearchFunction search)
{
    for (const auto state : states) {
        try {
            debug("generating image for state " + state);
            const auto assetPath = search(state);
            copyImageToStyleFolder(baseName, state, assetPath);
        } catch (std::exception &e) {
            qWarning() << "Warning: could not generate image:" << baseName << "," << state << "reason:" << e.what();
        }
    }
}

void generateButton(const QJsonDocument &doc)
{
    debug();
    debug("generating Button");
    copyFileToStyleFolder(":/Button.qml");
    generateImages(
        "button-background",
        {"idle", "pressed", "checked", "hovered"},
        [&doc](const QString &state) {
            getArray("artboardSets", doc.object());
            getObjectInArrayWithName("ButtonTemplate");
            getArray("artboards");
            getObjectInArrayWithName(QString("state=") + state);
            getArray("children");
            getObjectInArrayWithName("background");
            getObject("metadata");
            getObject("assetData");
            return getValue("assetPath").toString();
        });
}

void generateCheckBox(const QJsonDocument &doc)
{
    debug();
    debug("generating CheckBox");

    copyFileToStyleFolder(":/CheckBox.qml");
    // const QJsonObject checkBoxBackground = getTemplateRootObject("CheckboxBackground", doc);
    // const QJsonObject checkBoxIndicator = getTemplateRootObject("CheckboxIndicator", doc);
    // generateImages("checkbox-background", checkBoxBackground);
    // generateImages("checkbox-indicator", checkBoxIndicator);
}

void generateStyle(const QJsonDocument &doc)
{
    generateButton(doc);
    generateCheckBox(doc);
    generateQmlDir();
}

} // namespace