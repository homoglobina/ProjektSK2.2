#include "mywidget.h"
#include "ui_mywidget.h"

#include <QMessageBox>
#include <QHostAddress>
#include <QDateTime>
#include <QRegularExpression>

MyWidget::MyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MyWidget) {
    ui->setupUi(this);

    sock = new QTcpSocket(this);

    connect(ui->joinBtn, &QPushButton::clicked, this, &MyWidget::joinBtnHit);
    connect(ui->groupLineEdit, &QLineEdit::returnPressed, this, &MyWidget::joinBtnHit);

    connect(ui->sendBtn, &QPushButton::clicked, this, &MyWidget::sendBtnHit);
    connect(ui->msgLineEdit, &QLineEdit::returnPressed, this, &MyWidget::sendBtnHit);

    connect(sock, &QTcpSocket::connected, this, &MyWidget::onConnected);
    connect(sock, &QTcpSocket::disconnected, this, &MyWidget::onDisconnected);
    connect(sock, &QTcpSocket::readyRead, this, &MyWidget::onReadyRead);
    connect(sock, &QTcpSocket::errorOccurred, this, &MyWidget::onErrorOccurred);

    ui->talkGroup->setEnabled(false);

    ui->groupLineEdit->setText("127.0.0.1");
    ui->portSpinBox->setValue(12345);

    logToGui("<i>Welcome! Enter IP and Port to connect.</i>", "gray");
}

MyWidget::~MyWidget() {
    if(sock->isOpen())
        sock->close();
    delete ui;
}


void MyWidget::joinBtnHit() {
    if (sock->state() == QAbstractSocket::ConnectedState) {
        sock->disconnectFromHost();
    } else {
        // Connect logic
        QString ip = ui->groupLineEdit->text();
        int port = ui->portSpinBox->value();

        ui->joinBtn->setEnabled(false);
        ui->joinBtn->setText("Connecting...");

        sock->connectToHost(ip, port);
    }
}

void MyWidget::onConnected() {
    ui->joinBtn->setEnabled(true);
    ui->joinBtn->setText("Disconnect");
    ui->joinGroup->setEnabled(true);
    ui->talkGroup->setEnabled(true);
    ui->groupLineEdit->setEnabled(false);
    ui->portSpinBox->setEnabled(false);

    logToGui("<b>Connected to server!</b>", "green");
    ui->msgsTextEdit->append("Type <b>LobbyName(MyLobby)</b> to join a lobby.");

    ui->msgLineEdit->setFocus();
}

void MyWidget::onDisconnected() {
    ui->joinBtn->setEnabled(true);
    ui->joinBtn->setText("Connect");
    ui->talkGroup->setEnabled(false);
    ui->groupLineEdit->setEnabled(true);
    ui->portSpinBox->setEnabled(true);

    logToGui("<b>Disconnected from server.</b>", "red");
}

void MyWidget::onErrorOccurred(QAbstractSocket::SocketError) {
    logToGui("<b>Socket Error: " + sock->errorString() + "</b>", "red");
    ui->joinBtn->setEnabled(true);
    ui->joinBtn->setText("Connect");
}

void MyWidget::sendBtnHit() {
    QString txt = ui->msgLineEdit->text().trimmed();
    if (txt.isEmpty()) return;

    // Protocol expects newline at the end
    QByteArray data = (txt + "\n").toUtf8();
    sock->write(data);

    // Echo to own screen if it's not a command (optional style choice)
    // if(!txt.contains("(")) {
    //    logToGui("Me: " + txt, "gray");
    // }

    ui->msgLineEdit->clear();
    ui->msgLineEdit->setFocus();
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
        } else {
            logToGui("[RAW] " + line, "gray");
        }
    }
}

void MyWidget::handleMessage(const QString &command, const QStringList &args) {

    //  CHAT
    if (command == "Msg") {
        if (!args.isEmpty()) {
            logToGui("[CHAT] " + args.join(", "));
        }
    }
    // --- ERRORS ---
    else if (command == "Error") {
        logToGui("[ERROR] " + args.join(", "), "red");
    }
    // --- CATEGORIES ---
    else if (command == "Category") {
        QStringList catNames;
        QString ids = args.value(0);

        for (QChar c : ids) {
            if (c == '1') catNames << "Państwa";
            else if (c == '2') catNames << "Miasta (PL)";
            else if (c == '3') catNames << "Miasta (World)";
            else if (c == '4') catNames << "Jeziora";
            else if (c == '5') catNames << "Owoce/Warzywa";
            else if (c == '6') catNames << "Imiona";
        }

        logToGui("<br><b>=== NEW ROUND ===</b>", "blue");
        logToGui("Categories: " + catNames.join(" | "), "darkblue");
    }
    // --- LETTER ---
    else if (command == "Letter") {
        logToGui("<h1>LETTER: " + args.value(0) + "</h1>", "darkmagenta");
    }
    // --- TIME ---
    else if (command == "Time") {
        // Just show time in standard text to avoid spamming formatted logs
        // Or update a separate QLabel if you added one to the UI
        ui->msgsTextEdit->append("<i>Time left: " + args.value(0) + "s</i>");
    }
    // --- GAME STATUS ---
    else if (command == "GuessOK") {
        logToGui("<i>Answer accepted!</i>", "green");
    }
    else if (command == "GuessErr") {
        logToGui("<i>Answer rejected: " + args.value(0) + "</i>", "orange");
    }
    else if (command == "RoundEnd") {
        logToGui("<b>=== ROUND END ===</b><br>", "blue");
    }
    else if (command == "Score" || command == "TotalScore") {
        if(args.size() >= 2)
            logToGui("Score [" + args[0] + "]: " + args[1] + " pts");
    }
    else if (command == "FinalScore") {
        if(args.size() >= 3)
            logToGui("<b>#" + args[0] + " PLACE: " + args[1] + " (" + args[2] + " pts)</b>", "purple");
    }
    else if (command == "GameEnd") {
        logToGui("<br><b>=== GAME OVER ===</b>", "red");
    }
    else if (command == "Admin") {
        logToGui("Admin is: " + args.value(0), "darkcyan");
    }
    else {
        logToGui("Unknown: " + command + " " + args.join(", "), "gray");
    }
}

// Helper to print nice HTML to the text edit
void MyWidget::logToGui(const QString &text, const QString &color) {
    ui->msgsTextEdit->append(QString("<font color='%1'>%2</font>").arg(color).arg(text));
}
