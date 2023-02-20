#include <QtCore>
#include <QJsonDocument>

namespace StyleGenerator
{

void setResourcePath(const QString &path);
void setTargetPath(const QString path);
void setVerbose(bool verbose);
void generateStyle(const QJsonDocument &doc);

} // namespace
