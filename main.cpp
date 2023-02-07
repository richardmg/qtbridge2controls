#include <QtGui>
#include <QtQml>
#include <QtQuick>

int main(int argc, char **argv){
    QGuiApplication app(argc, argv);
//    QQmlEngine engine;
//    QQmlComponent component(&engine);
//    component.loadUrl(QUrl("qrc:///main.qml"));
//    QWindow *w = qobject_cast<QWindow *>(component.create());
//    w->show();
    QQmlApplicationEngine engine(QUrl("qrc:///main.qml"));
    return app.exec();
}

