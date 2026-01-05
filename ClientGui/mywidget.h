#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidgetItem>
#include <QStandardItemModel>

#include <QTimer>
#include <QMap>
#include <QVector>
#include <QChar>

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
    void onStartButtonClicked();
    void onSendAnswersClicked();
    void updateTimer();

    // All time
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);



private:

    QMap<QString, int> totalScores;


    QVector<int> currentCategories;
    QChar currentLetter;

    bool roundActive = false;
    bool gameRunning = false;
    bool isAdmin = false;

    Ui::MyWidget *ui;
    QTcpSocket *sock;
    QString buffer;
    bool isLoggedIn;

    QStandardItemModel *playerModel;

    QTimer *gameTimer;
    int timeLeft;

    void updateStartButtonState();

    QString getCategoryName(int id);

    void handleMessage(const QString &command, const QStringList &args);
    void logToGui(const QString &text, const QString &color = "black");
    void logGame(const QString &text, const QString &color = "black");

};

#endif // MYWIDGET_H
