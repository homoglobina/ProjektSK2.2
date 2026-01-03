#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidgetItem>

namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget {
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

private slots:
    // Tab 1 (Login)
    void joinBtnHit();
    void sendBtnHit();

    // Tab 2 (Lobbies)
    void onRefreshBtnClicked();
    void onJoinLobbyBtnClicked();
    void onDisconnectBtnClicked();
    void onLobbyItemDoubleClicked(QListWidgetItem *item);

    // Tab 3 (Game)
    void onLeaveBtnClicked();



    // All time
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    Ui::MyWidget *ui;
    QTcpSocket *sock;
    QString buffer;
    bool isLoggedIn;

    void handleMessage(const QString &command, const QStringList &args);
    void logToGui(const QString &text, const QString &color = "black");
};

#endif // MYWIDGET_H
