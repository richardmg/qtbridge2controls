#include <QtCore>
#include <QJsonDocument>

Q_DECLARE_LOGGING_CATEGORY(lcStyleGenerator)

namespace StyleGenerator
{

void setResourcePath(const QString &path);
void setTargetPath(const QString path);
void generateButton(const QJsonDocument &doc);

} // namespace
