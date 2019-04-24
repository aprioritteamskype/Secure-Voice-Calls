#include <QCoreApplication>
#include <iostream>
#include <QtDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    qDebug() << "hello";

    return a.exec();
}
