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

void debug(const QString &msg)
{
    if (!verboseOptionSet)
        return;
    qDebug().noquote() << msg;
}

void debugHeader(const QString &msg)
{
    debug("");
    debug("*** " + msg + " ***");
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

void copyImageToStyleFolder(const QString &baseName, const QString state, const QString srcName)
{
    // Require the images to be png for now. While we could convert svg's to
    // png's on the fly, we should rather investigate how we can do this during build
    // time (with the work done to create png icons from svg from cmake).
    if (!srcName.endsWith(".png"))
        throw std::runtime_error("The image needs to be png: " + srcName.toStdString());

    // Special case: the 'idle' state should not be included in the file name
    const QString targetState = (state == "idle") ? "" : "-" + state;
    const QString srcPath = resourcePath + '/' + srcName;
    const QString targetName = "images/" + baseName + targetState + ".png";
    copyFileToStyleFolder(srcPath, targetName);
}

void generateQmlDir()
{
    const QString path = styleDir + "/qmldir";
    const QString styleName = QFileInfo(styleDir).fileName();
    debug("generating qmldir: " + path);

    const QString version(" 1.0 ");
    QString qmldir;
    qmldir += "module " + styleName + "\n";
    qmldir += "Button" + version + "Button.qml\n";
    qmldir += "CheckBox" + version + "CheckBox.qml\n";
    qmldir += "Switch" + version + "Switch.qml\n";

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
    debugHeader("generating Button");
    copyFileToStyleFolder(":/Button.qml");

    const auto buttonArtboardSet = getArtboardSet("ButtonTemplate", doc);
    generateImages(
        "button-background",
        {"idle", "pressed", "checked", "hovered"},
        [&buttonArtboardSet](const QString &state) {
            getArtboardWithState(state, buttonArtboardSet);
            getArtboardChildWithName("background");
            return getImagePathInMetaData();
        });
}

void generateCheckBox(const QJsonDocument &doc)
{
    debugHeader("generating CheckBox");
    copyFileToStyleFolder(":/CheckBox.qml");

    const auto backgroundArtboardSet = getArtboardSet("CheckboxBackground", doc);
    generateImages(
        "checkbox-background",
        {"idle", "pressed", "checked", "hovered"},
        [&backgroundArtboardSet](const QString &state) {
            getArtboardWithState(state, backgroundArtboardSet);
            getArtboardChildWithName("background");
            return getImagePathInMetaData();
        });

    const auto indicatorArtboardSet = getArtboardSet("CheckboxIndicator", doc);
    generateImages(
        "checkbox-indicator",
        {"idle", "pressed", "checked", "hovered"},
        [&indicatorArtboardSet](const QString &state) {
            getArtboardWithState(state, indicatorArtboardSet);
            getArtboardChildWithName("checkBackground");
            return getImagePathInMetaData();
        });
}

void generateSwitch(const QJsonDocument &doc)
{
    debugHeader("generating Switch");
    copyFileToStyleFolder(":/Switch.qml");

    const auto backgroundArtboardSet = getArtboardSet("SwitchBackground", doc);
    generateImages(
        "switch-background",
        {"idle", "pressed", "checked", "hovered"},
        [&backgroundArtboardSet](const QString &state) {
            getArtboardWithState(state, backgroundArtboardSet);
            getArtboardChildWithName("background");
            return getImagePathInMetaData();
        });

    const auto indicatorArtboardSet = getArtboardSet("CheckboxIndicator", doc);
    generateImages(
        "checkbox-indicator",
        {"idle", "pressed", "checked", "hovered"},
        [&indicatorArtboardSet](const QString &state) {
            getArtboardWithState(state, indicatorArtboardSet);
            getArtboardChildWithName("checkBackground");
            return getImagePathInMetaData();
        });
}
void generateStyle(const QJsonDocument &doc)
{
    generateButton(doc);
    generateCheckBox(doc);
    generateSwitch(doc);
    generateQmlDir();
}

} // namespace