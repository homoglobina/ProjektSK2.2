#include "mywidget.h"
#include "ui_mywidget.h"
#include <QMessageBox>

MyWidget::MyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    sock = new QTcpSocket(this);
    isLoggedIn = false;

    playerModel = new QStandardItemModel(this);
    ui->columnView->setModel(playerModel);

    connect(ui->sendAnswersButton, &QPushButton::clicked,
            this, &MyWidget::onSendAnswersClicked);
    ui->sendAnswersButton->setEnabled(false);

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

    connect(ui->startGameButton, &QPushButton::clicked, this, &MyWidget::onStartButtonClicked);

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

    ui->scoreTable->setColumnCount(2);
    ui->scoreTable->setHorizontalHeaderLabels({"Gracz", "Punkty"});
    ui->scoreTable->horizontalHeader()->setStretchLastSection(true);
    ui->scoreTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->scoreTable->setSelectionMode(QAbstractItemView::NoSelection);
}

MyWidget::~MyWidget()
{
    if (sock->isOpen())
        sock->close();
    delete ui;
}

void MyWidget::joinBtnHit()
{
    if (sock->state() == QAbstractSocket::ConnectedState)
    {
        sock->disconnectFromHost();
    }
    else
    {
        QString ip = ui->groupLineEdit->text();
        int port = ui->portSpinBox->value();
        ui->joinBtn->setEnabled(false);
        ui->joinBtn->setText("Connecting...");
        sock->connectToHost(ip, port);
    }
}

void MyWidget::onDisconnectBtnClicked()
{
    if (sock->state() == QAbstractSocket::ConnectedState)
    {
        sock->disconnectFromHost();
    }
}

void MyWidget::onConnected()
{
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

void MyWidget::onDisconnected()
{
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
    ui->tabWidget->setCurrentIndex(0); 

    isLoggedIn = false;
    logToGui("<b>Disconnected.</b>", "red");
}

void MyWidget::onErrorOccurred(QAbstractSocket::SocketError)
{
    logToGui("Socket Error: " + sock->errorString(), "red");
    if (isLoggedIn || sock->state() != QAbstractSocket::ConnectedState)
    {
        onDisconnected();
    }
}

void MyWidget::onStartButtonClicked()
{
    QByteArray data = ("LobbyStart()\n");
    sock->write(data);
}

void MyWidget::sendBtnHit()
{
    QString txt = ui->msgNickEdit->text().trimmed();
    if (txt.isEmpty())
        return;
    QByteArray data = ("PlayerName(" + txt + ")\n").toUtf8();
    sock->write(data);

    ui->msgNickEdit->clear();
    ui->msgNickEdit->setFocus();
}

void MyWidget::onRefreshBtnClicked()
{
    ui->listWidget->clear();
    sock->write("ShowLobbies()\n");
}

void MyWidget::onLeaveBtnClicked()
{
    sock->write("LeaveLobby()\n");
}

void MyWidget::onJoinLobbyBtnClicked()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item)
    {
        QMessageBox::warning(this, "Warning", "Please select a lobby first.");
        return;
    }

    QString cmd = "LobbyName(" + item->text() + ")\n";
    sock->write(cmd.toUtf8());
}

void MyWidget::onLobbyItemDoubleClicked(QListWidgetItem *item)
{
    if (item)
    {
        ui->listWidget->setCurrentItem(item); 
        onJoinLobbyBtnClicked();
    }
}

void MyWidget::onReadyRead()
{
    QByteArray data = sock->readAll();
    buffer.append(QString::fromUtf8(data));

    buffer.remove(QChar('\0'));

    while (buffer.contains('\n'))
    {
        int lineEnd = buffer.indexOf('\n');
        QString line = buffer.left(lineEnd).trimmed();
        buffer.remove(0, lineEnd + 1);
        if (line.isEmpty())
            continue;

        int openParen = line.indexOf('(');
        int closeParen = line.lastIndexOf(')');

        if (openParen != -1 && closeParen != -1 && closeParen > openParen)
        {
            QString command = line.left(openParen);
            QString content = line.mid(openParen + 1, closeParen - openParen - 1);
            QStringList args = content.split(',', Qt::KeepEmptyParts);

            handleMessage(command, args);
        }
    }
}

void MyWidget::handleMessage(const QString &command, const QStringList &args)
{
    if (command == "Welcome")
    {
        QString msgContent = args.join(", ");
        if (!isLoggedIn)
        {
            isLoggedIn = true;
            logToGui("<b>Login Successful!</b>", "green");

            ui->tabWidget->setTabEnabled(0, false);
            ui->tabWidget->setTabEnabled(1, true);
            ui->tabWidget->setCurrentIndex(1);

            // refresh
            ui->listWidget->clear();
            sock->write("ShowLobbies()\n");
        }
        else
        {
            logToGui("[Server]: " + msgContent);
        }
    }

    else if (command == "Lobby")
    {
        if (!args.isEmpty())
        {
            QString roomName = args.first().trimmed();

            // if (ui->listWidget->findItems(roomName, Qt::MatchExactly).isEmpty()) {
            ui->listWidget->addItem(roomName);
            // }
        }
    }

else if (command == "StartGame")
{
    logGame("<b>Gra rozpoczęta!</b>", "green");

    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, true);
    ui->tabWidget->setCurrentIndex(2);

    if (!gameInitialized)
    {
        ui->scoreTable->setRowCount(0);

        for (int r = 0; r < playerModel->rowCount(); ++r)
        {
            QString playerName = playerModel->item(r)->text();

            int row = ui->scoreTable->rowCount();
            ui->scoreTable->insertRow(row);
            ui->scoreTable->setItem(row, 0, new QTableWidgetItem(playerName));
            ui->scoreTable->setItem(row, 1, new QTableWidgetItem("0"));
        }

        gameInitialized = true;
    }

}


    else if (command == "Joined")
    {
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setCurrentIndex(2);
    }

    else if (command == "PlayerRefresh")
    {
        if (playerModel)
            playerModel->clear();
    }
    else if (command == "Player")
    {
        if (!args.isEmpty())
        {
            QString playerName = args.first().trimmed();
            if (playerModel)
            {
                playerModel->appendRow(new QStandardItem(playerName));
            }
        }
    }

    else if (command == "LeftLobby")
    {
        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2, false);
        ui->tabWidget->setCurrentIndex(1);

        if (playerModel)
            playerModel->clear();

        onRefreshBtnClicked();
    }

    // --- ERRORS ---
    else if (command == "Error")
    {
        QString errText = args.join(", ");
        logToGui("<b>Error: " + errText + "</b>", "red");
        if (!isLoggedIn)
        {
            QMessageBox::critical(this, "Login Error", errText + "\nPlease try a different nickname.");
            ui->msgNickEdit->setFocus();
        }
    }

    else if (command == "Category")
    {
        currentCategories.clear();

        for (const QString &c : args)
        {
            for (QChar ch : c)
            {
                if (ch.isDigit())
                    currentCategories.push_back(ch.digitValue());
            }
        }

        logGame("<b>Kategorie:</b> " + args.join(" "), "blue");
    }

    else if (command == "Letter")
    {
        if (!args.isEmpty())
            currentLetter = args[0][0];

        logGame("<b>Litera:</b> " + args.join(" "), "purple");
    }

    else if (command == "Time")
    {
        roundActive = true;

        ui->answerEdit1->clear();
        ui->answerEdit2->clear();
        ui->answerEdit3->clear();

        ui->answerEdit1->setEnabled(true);
        ui->answerEdit2->setEnabled(true);
        ui->answerEdit3->setEnabled(true);
        ui->sendAnswersButton->setEnabled(true);

        logGame("<b>Czas:</b> " + args.join(" ") + " s", "red");
    }

    else if (command == "RoundEnd")
    {
        logGame("<hr><b>Koniec rundy</b>", "black");

    }

    else if (command == "Score")
    {
        QString player = args[0];
        int points = args[1].toInt();

        bool found = false;
        for (int r = 0; r < ui->scoreTable->rowCount(); ++r)
        {
            if (ui->scoreTable->item(r, 0)->text() == player)
            {
                int current = ui->scoreTable->item(r, 1)->text().toInt();
                ui->scoreTable->item(r, 1)->setText(QString::number(current + points));
                found = true;
                break;
            }
        }

        if (!found)
        {
            int row = ui->scoreTable->rowCount();
            ui->scoreTable->insertRow(row);
            ui->scoreTable->setItem(row, 0, new QTableWidgetItem(player));
            ui->scoreTable->setItem(row, 1, new QTableWidgetItem(QString::number(points)));

                    ui->scoreTable->item(row, 1)->setText(QString::number(points));

        }

        ui->scoreTable->sortItems(1, Qt::DescendingOrder);
    highlightWinners();

    }


    else if (command == "GameEnd")
    {
        logGame("<hr><b>KONIEC GRY</b>", "red");

        QMessageBox::information(
            this,
            "Koniec gry",
            "Gra zakończona! Sprawdź wyniki w tabeli.");

     gameInitialized = false;
        

    }


}

void MyWidget::logToGui(const QString &text, const QString &color)
{
    ui->msgsTextEdit->append(QString("<font color='%1'>%2</font>").arg(color).arg(text));
}

void MyWidget::logGame(const QString &text, const QString &color)
{
    ui->gameTextEdit->append(
        QString("<font color='%1'>%2</font>").arg(color, text));
}

void MyWidget::onSendAnswersClicked()
{
    if (!roundActive)
        return;

    QVector<QLineEdit *> edits = {
        ui->answerEdit1,
        ui->answerEdit2,
        ui->answerEdit3};

    for (int i = 0; i < currentCategories.size() && i < edits.size(); ++i)
    {
        QString ans = edits[i]->text().trimmed();
        if (ans.isEmpty())
            continue;

        QString msg = "Guess(" +
                      QString::number(currentCategories[i]) +
                      "," + ans + ")\n";

        sock->write(msg.toUtf8());
    }

    roundActive = false;

    ui->sendAnswersButton->setEnabled(false);
    ui->answerEdit1->setEnabled(false);
    ui->answerEdit2->setEnabled(false);
    ui->answerEdit3->setEnabled(false);

    logToGui("<i>Odpowiedzi wysłane</i>", "gray");
}

void MyWidget::highlightWinners()
{
    int rows = ui->scoreTable->rowCount();
    if (rows == 0)
        return;

    int maxScore = INT_MIN;

    for (int r = 0; r < rows; ++r)
    {
        int score = ui->scoreTable->item(r, 1)->text().toInt();
        maxScore = std::max(maxScore, score);
    }

    for (int r = 0; r < rows; ++r)
    {
        int score = ui->scoreTable->item(r, 1)->text().toInt();
        bool isWinner = (score == maxScore);

        QColor bg = isWinner ? QColor(180, 255, 180) : QColor(Qt::white);

        for (int c = 0; c < ui->scoreTable->columnCount(); ++c)
        {
            QTableWidgetItem *item = ui->scoreTable->item(r, c);
            if (!item)
                continue;

            item->setBackground(bg);

            QFont f = item->font();
            f.setBold(isWinner);
            item->setFont(f);
        }
    }
}

