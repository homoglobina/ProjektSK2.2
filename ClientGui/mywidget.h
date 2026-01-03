#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QTimer>
#include <QUdpSocket>
#include <QWidget>

namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget {
    Q_OBJECT

  public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

  protected:
    QUdpSocket *sock;
    void joinBtnHit();
    void socketError(QUdpSocket::SocketError);
    void sendBtnHit();
    /* TODO: dodać funkcję obsługującą pojawienie się danych do odbioru (o ile nie będzie użyta do tego lambda)*/

  private:
    Ui::MyWidget *ui;
};

#endif // MYWIDGET_H
