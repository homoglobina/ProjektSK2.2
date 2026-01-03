#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTime>

namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget {
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

private slots:
    void joinBtnHit();
    void sendBtnHit();


    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    Ui::MyWidget *ui;
    QTcpSocket *sock;
    QString buffer;


    void handleMessage(const QString &command, const QStringList &args);
    void logToGui(const QString &text, const QString &color = "black");
};

#endif // MYWIDGET_H
