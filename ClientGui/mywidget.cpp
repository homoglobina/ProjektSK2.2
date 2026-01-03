#include "mywidget.h"
#include "ui_mywidget.h"
#include <QMessageBox>

MyWidget::MyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MyWidget) {
    ui->setupUi(this);

    sock = new QTcpSocket(this);
    isLoggedIn = false;

    // Tab 1 Joining
    connect(ui->joinBtn, &QPushButton::clicked, this, &MyWidget::joinBtnHit);
    connect(ui->groupLineEdit, &QLineEdit::returnPressed, this, &MyWidget::joinBtnHit);
    connect(ui->sendBtn, &QPushButton::clicked, this, &MyWidget::sendBtnHit);
    connect(ui->msgNickEdit, &QLineEdit::returnPressed, this, &MyWidget::sendBtnHit);

    // Tab 2 LobbyList
    connect(ui->refreshButton, &QPushButton::clicked, this, &MyWidget::onRefreshBtnClicked);
    connect(ui->joinButton, &QPushButton::clicked, this, &MyWidget::onJoinLobbyBtnClicked);
    connect(ui->disconectButton, &QPushButton::clicked, this, &MyWidget::onDisconnectBtnClicked);
    connect(ui->leaveButton, &QPushButton::clicked, this, &MyWidget::onLeaveBtnClicked);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MyWidget::onLobbyItemDoubleClicked);


    // Tab 3

    // Connection stuff
    connect(sock, &QTcpSocket::connected, this, &MyWidget::onConnected);
    connect(sock, &QTcpSocket::disconnected, this, &MyWidget::onDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &MyWidget::onReadyRead);
    connect(sock, &QTcpSocket::errorOccurred, this, &MyWidget::onErrorOccurred);

    // Initialization
    // Login = 0 Lobbies = 1 Game = 2
    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);

    ui->sendBtn->setEnabled(false);
    ui->msgNickEdit->setEnabled(false);
    ui->groupLineEdit->setText("127.0.0.1");
    ui->portSpinBox->setValue(12345);

    logToGui("<i>Welcome! Please connect to the server.</i>", "gray");
}

MyWidget::~MyWidget() {
    if(sock->isOpen()) sock->close();
    delete ui;
}


void MyWidget::joinBtnHit() {
    if (sock->state() == QAbstractSocket::ConnectedState) {
        sock->disconnectFromHost();
    } else {
        QString ip = ui->groupLineEdit->text();
        int port = ui->portSpinBox->value();
        ui->joinBtn->setEnabled(false);
        ui->joinBtn->setText("Connecting...");
        sock->connectToHost(ip, port);
    }
}

void MyWidget::onDisconnectBtnClicked() {
    if (sock->state() == QAbstractSocket::ConnectedState) {
        sock->disconnectFromHost();
    }
}


void MyWidget::onConnected() {
    ui->joinBtn->setEnabled(true);
    ui->joinBtn->setText("Disconnect");

    ui->groupLineEdit->setEnabled(false);
    ui->portSpinBox->setEnabled(false);
    ui->talkGroup->setEnabled(true);

    ui->sendBtn->setEnabled(true);
    ui->msgNickEdit->setEnabled(true);
    ui->msgNickEdit->setFocus();

    logToGui("<b>Connected! Please enter your nickname.</b>", "blue");
}

void MyWidget::onDisconnected() {
    ui->joinBtn->setEnabled(true);
    ui->joinBtn->setText("Connect");

    ui->groupLineEdit->setEnabled(true);
    ui->portSpinBox->setEnabled(true);
    ui->talkGroup->setEnabled(false);
    ui->sendBtn->setEnabled(false);
    ui->msgNickEdit->setEnabled(false);
    ui->listWidget->clear();

    // Tab 0
    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setCurrentIndex(0); // Jump back to login

    isLoggedIn = false;
    logToGui("<b>Disconnected.</b>", "red");
}

void MyWidget::onErrorOccurred(QAbstractSocket::SocketError) {
    logToGui("Socket Error: " + sock->errorString(), "red");
    if(isLoggedIn || sock->state() != QAbstractSocket::ConnectedState) {
        onDisconnected();
    }
}


void MyWidget::sendBtnHit() {
    QString txt = ui->msgNickEdit->text().trimmed();
    if (txt.isEmpty()) return;
    QByteArray data = ("PlayerName(" + txt + ")\n").toUtf8();
    sock->write(data);

    ui->msgNickEdit->clear();
    ui->msgNickEdit->setFocus();
}

void MyWidget::onRefreshBtnClicked() {
    ui->listWidget->clear();
    sock->write("ShowLobbies()\n");
}


void MyWidget::onLeaveBtnClicked(){
    sock->write("LeaveLobby()\n");
}


void MyWidget::onJoinLobbyBtnClicked() {
    QListWidgetItem *item = ui->listWidget->currentItem();
    if(!item) {
        QMessageBox::warning(this, "Warning", "Please select a lobby first.");
        return;
    }

    // Try joining
    QString cmd = "LobbyName(" + item->text() + ")\n";
    sock->write(cmd.toUtf8());
}

void MyWidget::onLobbyItemDoubleClicked(QListWidgetItem *item) {
    if(item) {
        ui->listWidget->setCurrentItem(item); // Ensure it's selected
        onJoinLobbyBtnClicked();
    }
}

void MyWidget::onReadyRead() {
    QByteArray data = sock->readAll();
    buffer.append(QString::fromUtf8(data));

    while (buffer.contains('\n')) {
        int lineEnd = buffer.indexOf('\n');
        QString line = buffer.left(lineEnd).trimmed();
        buffer.remove(0, lineEnd + 1);
        if (line.isEmpty()) continue;

        int openParen = line.indexOf('(');
        int closeParen = line.lastIndexOf(')');

        if (openParen != -1 && closeParen != -1 && closeParen > openParen) {
            QString command = line.left(openParen);
            QString content = line.mid(openParen + 1, closeParen - openParen - 1);
            QStringList args = content.split(',', Qt::KeepEmptyParts);

            handleMessage(command, args);
        }
    }
}

void MyWidget::handleMessage(const QString &command, const QStringList &args) {
    if (command == "Welcome") {
        QString msgContent = args.join(", ");
        if (!isLoggedIn ) {
            isLoggedIn = true;
            logToGui("<b>Login Successful!</b>", "green");


            ui->tabWidget->setTabEnabled(0, false);
            ui->tabWidget->setTabEnabled(1, true);
            ui->tabWidget->setCurrentIndex(1);

            // refresh
            ui->listWidget->clear();
            sock->write("ShowLobbies()\n");
        }
        else {
            logToGui("[Server]: " + msgContent);
        }
    }

    else if (command == "Lobby") {
        if (!args.isEmpty()) {
            QString roomName = args.first().trimmed();

            // if (ui->listWidget->findItems(roomName, Qt::MatchExactly).isEmpty()) {
            ui->listWidget->addItem(roomName);
            // }
        }
    }

    else if (command == "Joined") {
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setCurrentIndex(2);
    }

    else if (command == "LeftLobby"){
        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2,false);
        ui->tabWidget->setCurrentIndex(1);

    }

    // --- ERRORS ---
    else if (command == "Error") {
        QString errText = args.join(", ");
        logToGui("<b>Error: " + errText + "</b>", "red");
        if (!isLoggedIn) {
            QMessageBox::critical(this, "Login Error", errText + "\nPlease try a different nickname.");
            ui->msgNickEdit->setFocus();
        }
    }



    // TODO:
}

void MyWidget::logToGui(const QString &text, const QString &color) {
    ui->msgsTextEdit->append(QString("<font color='%1'>%2</font>").arg(color).arg(text));
}
