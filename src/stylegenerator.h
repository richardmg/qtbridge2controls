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

    void copyImageToStyleFolder(const QString resourceName, const QString &subName = "")
    {
        // Require the images to be png for now. While we could convert svg's to
        // png's on the fly, we should rather investigate how we can do this during build
        // time (with the work done to create png icons from svg from cmake).
        if (!resourceName.endsWith(".png"))
            throw std::runtime_error("The image needs to be png: " + resourceName.toStdString());

        const QString srcPath = m_resourcePath + '/' + resourceName;
        const QString targetState = (m_currentState == "idle") ? "" : "-" + m_currentState;
        const QString targetSubName = (subName == "") ? "" : "-" + subName;
        const QString targetName = "images/" + m_currentBaseName + targetSubName + targetState + ".png";
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
        const QString &baseName,
        const QStringList &imageStates,
        const QStringList &jsonStates,
        SearchFunction search)
    {
        if (imageStates.count() != jsonStates.count())
            throw std::runtime_error("imageStates list and jsonStates list have different count!");

        m_currentBaseName = baseName;

        for (int i = 0; i < imageStates.count(); ++i) {
            m_currentState = imageStates[i];
            const QString jsonState = jsonStates[i];

            debug("generating image '" + baseName + "' for state '"
                + m_currentState + "' ('" + jsonState + "')");

            try {
                search(jsonState);
            } catch (std::exception &e) {
                qWarning() << "Warning: could not generate image:" << baseName
                    << ", state:" << m_currentState << "reason:" << e.what();
            }
        }

        m_currentState = "";
        m_currentBaseName = "";
    }

    template <typename SearchFunction>
    void generateImages(
        const QString &baseName,
        const QStringList &states,
        SearchFunction search)
    {
        // TODO: when the Figma images have the same state names as the imagine
        // style images, we no longer need to do this mapping between the states, and
        // we can collapse the following function into this one.
        generateImages(baseName, states, states, search);
    }

    void generateButton()
    {
        debugHeader("generating Button");
        copyFileToStyleFolder(":/Button.qml");

        const auto buttonArtboardSet = getArtboardSet("ButtonTemplate", m_document);
        generateImages(
            "button-background",
            {"idle", "pressed", "checked", "hovered"},
            [&buttonArtboardSet, this](const QString &state)
            {
                getArtboardWithState(state, buttonArtboardSet);
                getArtboardChildWithName("background");
                copyImageToStyleFolder(getImagePathInMetaData());
            });
    }

    void generateCheckBox()
    {
        debugHeader("generating CheckBox");
        copyFileToStyleFolder(":/CheckBox.qml");

        const auto backgroundArtboardSet = getArtboardSet("CheckboxBackground", m_document);
        generateImages(
            "checkbox-background",
            {"idle", "pressed", "checked", "hovered"},
            [&backgroundArtboardSet, this](const QString &state)
            {
                getArtboardWithState(state, backgroundArtboardSet);
                getArtboardChildWithName("background");
                copyImageToStyleFolder(getImagePathInMetaData());
            });

        const auto indicatorArtboardSet = getArtboardSet("CheckboxIndicator", m_document);
        generateImages(
            "checkbox-indicator",
            {"idle", "pressed", "checked", "hovered"},
            [&indicatorArtboardSet, this](const QString &state)
            {
                getArtboardWithState(state, indicatorArtboardSet);
                getArtboardChildWithName("checkBackground");
                copyImageToStyleFolder(getImagePathInMetaData());
            });
    }

    void generateSwitch()
    {
        debugHeader("generating Switch");
        copyFileToStyleFolder(":/Switch.qml");

        const auto backgroundArtboardSet = getArtboardSet("SwitchBackground", m_document);
        generateImages(
            "switch-background",
            {"idle", "pressed", "hovered"},
            {"idleON", "pressedON", "hoveredON"},
            [&backgroundArtboardSet, this](const QString &state)
            {
                getArtboardWithState(state, backgroundArtboardSet);
                getArtboardChildWithName("background");
                copyImageToStyleFolder(getImagePathInMetaData());
            });

        const auto handleArtboardSet = getArtboardSet("SwitchHandle", m_document);
        generateImages(
            "switch-handle",
            {"idle", "pressed", "hovered"},
            {"idleON", "pressedON", "hoveredON"},
            [&handleArtboardSet, this](const QString &state)
            {
                const auto artboard = getArtboardWithState(state, handleArtboardSet);
                getArtboardChildWithName("handle", artboard);
                copyImageToStyleFolder(getImagePathInMetaData());
                // getArtboardChildWithName("iconLeftON", artboard);
                // copyImageToStyleFolder(getImagePathInMetaData(), "iconLeftOn");
                // getArtboardChildWithName("iconRightON", artboard);
                // copyImageToStyleFolder(getImagePathInMetaData(), "iconRightOn");
            });
    }

private:
    QJsonDocument m_document;
    QString m_resourcePath;
    QString m_targetPath;
    bool m_verbose = false;

    QString m_currentBaseName;
    QString m_currentState;
};

#endif // QSTYLEREADER_H
