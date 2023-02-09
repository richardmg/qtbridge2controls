#ifndef QTBRIDGEREADER_H 
#define QTBRIDGEREADER_H

#include <QtCore>
#include <QtGui/private/qzipreader_p.h>

class QtBridgeReader
{
public:
    QtBridgeReader(const QString &bridgeFile)
    {
        m_destDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
        + "/" + qApp->applicationName();

        QZipReader zip(bridgeFile);
        if (!zip.isReadable())
        {
            m_errorMessage = QStringLiteral("Could not read: %1").arg(bridgeFile);
            return;
        }

        if (!QDir().mkpath(m_destDir))
        {
            m_errorMessage = QStringLiteral("Could not create tmp path: %1").arg(m_destDir);
            return;
        }

        if (!zip.extractAll(m_destDir))
        {
            m_errorMessage = QStringLiteral("Could not extract bridge file: %1").arg(zip.status());
            return;
        }
    
        const auto fileList = QDir(m_destDir).entryList(QStringList("*.metadata"));
        if (fileList.isEmpty()) {
            m_errorMessage = QStringLiteral("Could not find a .metadata file inside the .qtbridge file: %1")
                .arg(entryList().join(u"; "_qs));
            return;
        }

        const QString metaDataFileName = m_destDir + "/" + fileList.first();
        QFile metaDataFile(metaDataFileName);
        if (!metaDataFile.exists())
        {
            m_errorMessage = QStringLiteral("File doesn't exists: %1").arg(metaDataFileName);
            return;
        }

        if (!metaDataFile.open(QFile::ReadOnly))
        {
            m_errorMessage = QStringLiteral("Could not open %1 for reading!").arg(metaDataFileName);
            return;
        }

        m_metaData = metaDataFile.readAll();
    }

    ~QtBridgeReader()
    {
        QDir(m_destDir).removeRecursively();
    }

    QString metaData() const{
        return m_metaData;
    }

    QString getFilePath(const QString &fileName) const
    {
        return m_destDir + "/" + fileName;
    }

    bool hasError() const
    {
        return !m_errorMessage.isEmpty();
    }

    QString errorMessage() const
    {
        return m_errorMessage;
    }

    QStringList entryList()
    {
        return QDir(m_destDir).entryList();
    }

private: 
    QString m_destDir;
    QString m_metaData;
    QString m_errorMessage;
};

#endif
