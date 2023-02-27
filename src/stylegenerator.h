#include <QtCore>
#include <QJsonDocument>

namespace StyleGenerator
{

void setDocument(const QJsonDocument &doc);
void setResourcePath(const QString &path);
void setTargetPath(const QString path);
void setVerbose(bool verbose);

void generateStyle();

} // namespace
