#include <QtCore>
#include <QJsonDocument>

namespace StyleGenerator
{

extern QJsonDocument document;
extern QString resourcePath;
extern QString targetPath;
extern bool verbose;

void generateStyle();

} // namespace
