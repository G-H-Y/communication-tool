#include "server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SERVER w;
    w.show();

    return a.exec();
}
