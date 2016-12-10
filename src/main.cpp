#include "inc/mainwindow.h"
#include "inc/model_manager.h"
#include <QApplication>

QVector<Model*> ModelManager::models(0);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
