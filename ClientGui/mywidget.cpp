#include "mywidget.h"
#include "ui_mywidget.h"
#include <QMessageBox>

#include <QHeaderView>
#include <QTableWidgetItem>
#include <QStandardItem>



QString MyWidget::getCategoryName(int id)
{
    QString catName;
    switch (id) {
    case 1: catName = "Państwa"; break;
    case 2: catName = "Miasta (PL)"; break;
    case 3: catName = "Miasta (świat)"; break;
    case 4: catName = "Jeziora"; break;
    case 5: catName = "Owoce/warzywa"; break;
    case 6: catName = "Imiona"; break;
    default: catName = "Kategoria " + QString::number(id); break;
    }
    return catName;
}

MyWidget::MyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    sock = new QTcpSocket(this);
    isLoggedIn = false;
    gameRunning = false;
    isAdmin = false;

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MyWidget::updateTimer);
    timeLeft = 0;



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

    if (ui->currentLetter) {
        ui->currentLetter->setReadOnly(true);
        ui->currentLetter->setFont(QFont("Arial", 24, QFont::Bold)); // Optional: Make text big
        ui->currentLetter->setAlignment(Qt::AlignCenter);            // Optional: Center text
    }


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

    if (ui->adminLed) {
        ui->adminLed->setStyleSheet("background-color: #404040; border-radius: 15px; border: 1px solid black;");
    }
    updateStartButtonState();

    // Settings changing

    if (ui->playersBox) {
        for(int i = 2; i <= 10; i++) ui->playersBox->addItem(QString::number(i));
        ui->playersBox->setCurrentText("10");
    }

    if (ui->roundTimeBox) {
        ui->roundTimeBox->addItem("30");
        ui->roundTimeBox->addItem("60");
        ui->roundTimeBox->addItem("90");
        ui->roundTimeBox->addItem("120");
        ui->roundTimeBox->addItem("180");
        ui->roundTimeBox->setCurrentText("60");
    }

    if (ui->maxRoundsBox) {
        for(int i = 1; i <= 10; i++) ui->maxRoundsBox->addItem(QString::number(i));
        ui->maxRoundsBox->setCurrentText("3");
    }

    connect(ui->settingsButton, &QPushButton::clicked, this, &MyWidget::onSettingsBtnClicked);

    updateAdminInterface();
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

void MyWidget::updateStartButtonState()
{
    if (gameRunning) {
        ui->startGameButton->setEnabled(false);
        ui->startGameButton->setStyleSheet("");
        return;
    }

    int playerCount = playerModel->rowCount();

    if (playerCount >= 2 && isAdmin) {
        ui->startGameButton->setEnabled(true);
        ui->startGameButton->setStyleSheet("background-color: green; color: white; font-weight: bold;");
    } else {
        ui->startGameButton->setEnabled(false);
        ui->startGameButton->setStyleSheet("");
    }
}

void MyWidget::updateTimer()
{
    if (timeLeft > 0) {
        timeLeft--;
        ui->timeNumber->display(timeLeft);
    } else {
        gameTimer->stop();
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
    ui->tabWidget->setCurrentIndex(0); // Jump back to login

    // Tab 2
    if (ui->adminLed) {
        ui->adminLed->setStyleSheet("background-color: #404040; border-radius: 15px; border: 1px solid black;");
    }

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

    // Try joining
    QString cmd = "LobbyName(" + item->text() + ")\n";
    sock->write(cmd.toUtf8());
}

void MyWidget::onLobbyItemDoubleClicked(QListWidgetItem *item)
{
    if (item)
    {
        ui->listWidget->setCurrentItem(item); // Ensure it's selected
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
        if (!gameRunning) {
            ui->scoreTable->setRowCount(0);
            totalScores.clear();
        }

        gameRunning = true;
        updateStartButtonState();
        updateAdminInterface();

        logGame("<b>Gra rozpoczęta!</b>", "green");

        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setCurrentIndex(2);

        if (ui->msgsTextEdit) ui->msgsTextEdit->clear();
    }

    else if (command == "Joined")
    {
        resetLobbyUI();
        isAdmin = false;
        updateStartButtonState();
        updateAdminInterface();

        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setCurrentIndex(2);
    }

    else if (command == "PlayerRefresh")
    {

        if (playerModel)
            playerModel->clear();

        updateStartButtonState();
    }
    else if (command == "Player")
    {
        if (!args.isEmpty())
        {
            QString playerName = args.first().trimmed();
            if (playerModel)
            {
                playerModel->appendRow(new QStandardItem(playerName));
                updateStartButtonState();
            }
        }
    }

    else if (command == "LeftLobby")
    {
        if (ui->adminLed) {
            ui->adminLed->setStyleSheet("background-color: #404040; border-radius: 15px; border: 1px solid black;");
        }

        resetLobbyUI();

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

        if (errText == "Taken_Name"){
            QMessageBox::critical(this, errText, "Zajęta nazwa \nSpróbuj innej ");
            ui->msgNickEdit->setFocus();
        }
        else if (errText == "Invalid_Name"){
            QMessageBox::critical(this, errText, "Niepoprawna nazwa\nNie używaj spacji i pustych znaków");
        }
        else if (errText == "Not_Admin"){
            QMessageBox::critical(this, errText, "Nie jesteś adminem \ndo wykonania tej czynności potrzebujesz uprawnień administracyjnych");
        }
        else if (errText == "Game_Already_Started"){
            QMessageBox::critical(this, errText, "Gra już rozpoczęta");
        }
        else if (errText =="LobbyExists"){
            QMessageBox::critical(this, errText, "Zajęta nazwa \nLobby o tej nazwie już istnieje");
        }
        else {
            QMessageBox::critical(this, errText, "Error: ");
        }

    }

    else if (command == "Category")
    {
        currentCategories.clear();

        ui->categoryBox->clear();
        ui->categoryWidget->setRowCount(0);
        ui->categoryWidget->setColumnCount(1);
        ui->categoryWidget->setHorizontalHeaderLabels({"Aktywne Kategorie"});
        ui->categoryWidget->horizontalHeader()->setStretchLastSection(true);

        // "135" -> categories
        for (const QString &c : args)
        {
            for (QChar ch : c)
            {
                if (ch.isDigit())
                {
                    int catId = ch.digitValue();
                    currentCategories.push_back(catId);

                    QString name = getCategoryName(catId);

                    int row = ui->categoryWidget->rowCount();
                    ui->categoryWidget->insertRow(row);
                    ui->categoryWidget->setItem(row, 0, new QTableWidgetItem(name));

                    ui->categoryBox->addItem(name, catId);
                }
            }
        }

        logGame("<b>Kategorie:</b> " + args.join(" "), "blue");
    }

    else if (command == "Letter")
    {
        if (!args.isEmpty())
        {
            QString letter = args[0];
            currentLetter = letter[0];

            if (ui->currentLetter) {
                ui->currentLetter->setText(letter);
                ui->currentLetter->setAlignment(Qt::AlignCenter);
            }
        }

        logGame("<b>Litera:</b> " + args.join(" "), "purple");
    }

    else if (command == "Time")
    {
        roundActive = true;

        ui->answerEdit1->clear();
        ui->answerEdit1->setEnabled(true);
        ui->sendAnswersButton->setEnabled(true);

        if (!args.isEmpty()) {
            timeLeft = args[0].toInt();
            ui->timeNumber->display(timeLeft);
            gameTimer->start(1000);
        }



        logGame("<b>Czas:</b> " + args.join(" ") + " s", "red");
    }

    else if (command == "RoundEnd")
    {
        gameTimer->stop();
        logGame("<hr><b>Koniec rundy</b>", "black");
    }

    else if (command == "RoundEnd")
    {
        logGame("<hr><b>Koniec rundy</b>", "black");

        // ui->scoreTable->setRowCount(totalScores.size());

        // int row = 0;
        // for (auto it = totalScores.begin(); it != totalScores.end(); ++it)
        // {
        //     ui->scoreTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        //     ui->scoreTable->setItem(row, 1,
        //                             new QTableWidgetItem(QString::number(it.value())));
        //     row++;
        // }
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
        }
    }

    else if (command == "BecameAdmin")
    {
        isAdmin = true;
        updateStartButtonState();
        updateAdminInterface();

        if (ui->adminLed) {
            ui->adminLed->setStyleSheet("background-color: #00FF00; border-radius: 15px; border: 1px solid black;");
        }
        logToGui("<b>Zostałeś administratorem lobby!</b>", "gold");
    }

    else if (command == "GameEnd")
    {
        roundActive = false;
        gameRunning = false;
        logGame("<hr><b>KONIEC GRY</b>", "red");

        QMessageBox::information(
            this,
            "Koniec gry",
            "Gra zakończona! Sprawdź wyniki w tabeli.");

        // ui->tabWidget->setCurrentIndex(1); // powrót do lobby
        updateStartButtonState();
        updateAdminInterface();
    }

    // TODO:
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
    if (!roundActive) return;

    QString ans = ui->answerEdit1->text().trimmed();
    if (ans.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Wpisz odpowiedź!");
        return;
    }

    bool ok;
    int catId = ui->categoryBox->currentData().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Błąd", "Wybierz kategorię z listy!");
        return;
    }


    QString msg = "Guess(" + QString::number(catId) + "," + ans + ")\n";
    sock->write(msg.toUtf8());

    ui->answerEdit1->clear();
    ui->answerEdit1->setFocus();

    logToGui("<i>Wysłano: " + getCategoryName(catId) + " - " + ans + "</i>", "gray");
}

void MyWidget::onSettingsBtnClicked()
{
    if (!isAdmin) return;

    int maxPlayers = ui->playersBox->currentText().toInt();
    int roundTime = ui->roundTimeBox->currentText().toInt();
    int maxRounds = ui->maxRoundsBox->currentText().toInt();

    QStringList catIds;
    if (ui->checkCountry->isChecked())   catIds << "1";
    if (ui->checkCityPl->isChecked())    catIds << "2";
    if (ui->checkCityWorld->isChecked()) catIds << "3";
    if (ui->checkLake->isChecked())      catIds << "4";
    if (ui->checkFruit->isChecked())     catIds << "5";
    if (ui->checkName->isChecked())      catIds << "6";

    if (catIds.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Wybierz przynajmniej jedną kategorię!");
        return;
    }


    QString cmd = QString("changeSettings(%1,%2,%3,%4)\n")
                      .arg(maxPlayers)
                      .arg(roundTime)
                      .arg(maxRounds)
                      .arg(catIds.join(","));

    sock->write(cmd.toUtf8());
}

void MyWidget::updateAdminInterface()
{
    bool enableSettings = (isAdmin && !gameRunning);

    QList<QWidget*> adminWidgets = {
        ui->settingsButton,
        ui->playersBox,
        ui->roundTimeBox,
        ui->maxRoundsBox,
        ui->checkCountry,
        ui->checkCityPl,
        ui->checkCityWorld,
        ui->checkLake,
        ui->checkFruit,
        ui->checkName,
        ui->label_9,
        ui->label_10,
        ui->label_11,
        ui->label_12
    };

    for (QWidget* w : adminWidgets) {
        if (w) {
            w->setVisible(enableSettings);
            // w->setEnabled(enableSettings); // grey out instead of hiding
        }
    }
}


void MyWidget::resetLobbyUI()
{
    currentCategories.clear();
    totalScores.clear();
    timeLeft = 0;
    if (gameTimer->isActive()) gameTimer->stop();

    if (ui->gameTextEdit) ui->gameTextEdit->clear();
    if (ui->currentLetter) ui->currentLetter->clear();
    if (ui->timeNumber) ui->timeNumber->display(0);

    if (ui->scoreTable) ui->scoreTable->setRowCount(0);
    if (ui->categoryWidget) ui->categoryWidget->setRowCount(0);
    if (ui->categoryBox) ui->categoryBox->clear();
    if (playerModel) playerModel->clear();

    ui->answerEdit1->clear();
    ui->answerEdit1->setEnabled(false);
    ui->sendAnswersButton->setEnabled(false);

    roundActive = false;
    gameRunning = false;
    isAdmin = false;

    updateStartButtonState();
    updateAdminInterface();

    if (ui->adminLed) {
        ui->adminLed->setStyleSheet("background-color: #404040; border-radius: 15px; border: 1px solid black;");
    }
}
