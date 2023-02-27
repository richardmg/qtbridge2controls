#ifndef QSTYLEREADER_H 
#define QSTYLEREADER_H

#include <QtCore>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

#include "jsontools.h"

using namespace JsonTools;

class StyleGenerator {

public:
    StyleGenerator(const QJsonDocument &document, const QString &resourcePath, const QString &targetPath)
        : m_document(document)
        , m_resourcePath(resourcePath)
        , m_targetPath(targetPath)
    {
    }

    void setVerbose(bool verbose)
    {
        m_verbose = verbose;
    }

    void generateStyle()
    {
        generateButton();
        generateCheckBox();
        generateSwitch();
        generateQmlDir();
    }

private:

    void debug(const QString &msg)
    {
        if (!m_verbose)
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

        QString fileName = destFileName;
        if (fileName.isEmpty())
            fileName = QFileInfo(srcFile).fileName();

        const QString targetFilePath = m_targetPath + "/" + fileName;
        QFileInfo targetFilePathInfo(targetFilePath);
        const QDir targetDir = targetFilePathInfo.absoluteDir();
        if (!targetDir.exists())
        {
            if (!QDir().mkpath(targetDir.path()))
                throw std::runtime_error("Could not create target path: " + m_targetPath.toStdString());
        }

        debug("copying " + srcPath + " to " + targetFilePath);
        srcFile.copy(targetFilePath);
    }

    void copyImageToStyleFolder(const QString &name, const QString &state,
        const QString resourceName)
    {
        // Require the images to be png for now. While we could convert svg's to
        // png's on the fly, we should rather investigate how we can do this during build
        // time (with the work done to create png icons from svg from cmake).
        if (!resourceName.endsWith(".png"))
            throw std::runtime_error("The image needs to be png: " + resourceName.toStdString());

        const QString srcPath = m_resourcePath + '/' + resourceName;
        const QString targetState = (state == "idle") ? "" : "-" + state;
        const QString targetName = "images/" + name + targetState + ".png";
        copyFileToStyleFolder(srcPath, targetName);
    }

    void generateQmlDir()
    {
        const QString path = m_targetPath + "/qmldir";
        const QString styleName = QFileInfo(m_targetPath).fileName();
        debug("generating qmldir: " + path);

        const QString version(" 1.0 ");
        QString qmldir;
        qmldir += "module " + styleName + "\n";
        qmldir += "Button" + version + "Button.qml\n";
        qmldir += "CheckBox" + version + "CheckBox.qml\n";
        qmldir += "Switch" + version + "Switch.qml\n";

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly))
            throw std::runtime_error("Could not open file for writing: " + path.toStdString());

        QTextStream out(&file);
        out << qmldir;
    }

    template <typename SearchFunction>
    void generateImages(
        const QStringList &imageStates,
        const QStringList &jsonStates,
        SearchFunction search)
    {
        if (imageStates.count() != jsonStates.count())
            throw std::runtime_error("imageStates list and jsonStates list have different count!");

        QString name;
        QString state;
        QString jsonState;

        for (int i = 0; i < imageStates.count(); ++i) {
            state = imageStates[i];
            jsonState = jsonStates[i];

            try {
                search(state, jsonState);
            } catch (std::exception &e) {
                qWarning().nospace().noquote()
                    << "Warning: could not generate image. Json path: "
                    << lastArtboardSetName << ", "
                    << jsonState << ", "
                    << lastArtboardChildName << ". "
                    << "Reason: " << e.what();
            }
        }
    }

    template <typename SearchFunction>
    void generateImages(
        const QStringList &states,
        SearchFunction search)
    {
        // TODO: when the Figma images have the same state names as the imagine
        // style images, we no longer need to do this mapping between the states, and
        // we can collapse the following function into this one.
        generateImages(states, states, search);
    }

    void generateButton()
    {
        debugHeader("generating Button");
        copyFileToStyleFolder(":/Button.qml");

        const auto buttonArtboardSet = getArtboardSet("ButtonTemplate", m_document);
        generateImages(
            {"idle", "pressed", "checked", "hovered"},
            [&buttonArtboardSet, this]
            (const QString &state, const QString &jsonState)
            {
                getArtboardWithState(jsonState, buttonArtboardSet);
                getArtboardChildWithName("background");
                copyImageToStyleFolder("button-background", state, getImagePathInMetaData());
            });
    }

    void generateCheckBox()
    {
        debugHeader("generating CheckBox");
        copyFileToStyleFolder(":/CheckBox.qml");

        const auto backgroundArtboardSet = getArtboardSet("CheckboxBackground", m_document);
        generateImages(
            {"idle", "pressed", "checked", "hovered"},
            [&backgroundArtboardSet, this]
            (const QString &state, const QString &jsonState)
            {
                getArtboardWithState(jsonState, backgroundArtboardSet);
                getArtboardChildWithName("background");
                copyImageToStyleFolder("checkbox-background", state, getImagePathInMetaData());
            });

        const auto indicatorArtboardSet = getArtboardSet("CheckboxIndicator", m_document);
        generateImages(
            {"idle", "pressed", "checked", "hovered"},
            [&indicatorArtboardSet, this]
            (const QString &state, const QString &jsonState)
            {
                getArtboardWithState(jsonState, indicatorArtboardSet);
                getArtboardChildWithName("checkBackground");
                copyImageToStyleFolder("checkbox-indicator", state, getImagePathInMetaData());
            });
    }

    void generateSwitch()
    {
        debugHeader("generating Switch");
        copyFileToStyleFolder(":/Switch.qml");

        const auto backgroundArtboardSet = getArtboardSet("SwitchBackground", m_document);
        generateImages(
            {"idle", "pressed", "hovered"},
            {"idleON", "pressedON", "hoveredON"},
            [&backgroundArtboardSet, this]
            (const QString &state, const QString &jsonState)
            {
                getArtboardWithState(jsonState, backgroundArtboardSet);
                getArtboardChildWithName("background");
                copyImageToStyleFolder("switch-background", state, getImagePathInMetaData());
            });

        const auto handleArtboardSet = getArtboardSet("SwitchHandle", m_document);
        generateImages(
            {"idle", "pressed", "hovered"},
            {"idleON", "pressedON", "hoveredON"},
            [&handleArtboardSet, this]
            (const QString &state, const QString &jsonState)
            {
                const auto artboard = getArtboardWithState(jsonState, handleArtboardSet);
                getArtboardChildWithName("handle", artboard);
                copyImageToStyleFolder("switch-handle", state, getImagePathInMetaData());
                getArtboardChildWithName("iconLeftON", artboard);
                copyImageToStyleFolder("switch-handle-lefticon-on", state, getImagePathInMetaData());
                getArtboardChildWithName("iconRightON", artboard);
                copyImageToStyleFolder("switch-handle-righticon-on", state, getImagePathInMetaData());
            });
    }

private:
    QJsonDocument m_document;
    QString m_resourcePath;
    QString m_targetPath;
    bool m_verbose = false;
};

#endif // QSTYLEREADER_H
