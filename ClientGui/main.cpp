#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include "mywidget.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MyWidget w;
    w.show();

    return a.exec();
}
