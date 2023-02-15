#ifndef QTBRIDGEREADER_H 
#define QTBRIDGEREADER_H

#include <QtCore>
#include <QJsonDocument>
#include <QtGui/private/qzipreader_p.h>

#include <stdexcept>

class QtBridgeReader
{
public:
    QtBridgeReader(const QString &bridgeFile)
    {
        m_destDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
        + "/" + qApp->applicationName();

        if (QFileInfo(bridgeFile).suffix().compare(u"qtbridge"_qs) != 0)
            throw std::invalid_argument("Input is not a .qtbridge file!");

        QZipReader zip(bridgeFile);
        if (!zip.isReadable())
            throw std::invalid_argument("Could not read input file: " + bridgeFile.toStdString());
        if (!QDir().mkpath(m_destDir))
            throw std::invalid_argument("Could not create tmp path: " + m_destDir.toStdString());
        if (!zip.extractAll(m_destDir))
            throw std::invalid_argument("Could not unzip input file: " + std::to_string(zip.status()));
    
        const auto fileList = QDir(m_destDir).entryList(QStringList("*.metadata"));
        if (fileList.isEmpty())
            throw std::invalid_argument("Could not fine a .metadata inside the input file!");

        const QString metaDataFileName = m_destDir + "/" + fileList.first();
        QFile metaDataFile(metaDataFileName);
        if (!metaDataFile.exists())
            throw std::invalid_argument("File doesn't exist: " + metaDataFileName.toStdString());
        if (!metaDataFile.open(QFile::ReadOnly))
            throw std::invalid_argument("Could not open file for reading: " + metaDataFileName.toStdString());

        QJsonParseError error;
        m_metaData = QJsonDocument::fromJson(metaDataFile.readAll(), &error);
        if (m_metaData.isNull())
            throw std::invalid_argument("Could not parse json file: " + error.errorString().toStdString());
    }

    ~QtBridgeReader()
    {
        QDir(m_destDir).removeRecursively();
    }

    QJsonDocument metaData() const
    {
        return m_metaData;
    }

    QString getFilePath(const QString &fileName) const
    {
        return m_destDir + "/" + fileName;
    }

    QStringList entryList()
    {
        return QDir(m_destDir).entryList();
    }

private: 
    QString m_destDir;
    QJsonDocument m_metaData;
};

#endif
