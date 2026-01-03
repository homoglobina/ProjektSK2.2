#include "mywidget.h"
#include "ui_mywidget.h"

#include <QMessageBox>
#include <QNetworkDatagram>

MyWidget::MyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MyWidget) {
    ui->setupUi(this);

    connect(ui->joinBtn, &QPushButton::clicked, this, &MyWidget::joinBtnHit);
    connect(ui->groupLineEdit, &QLineEdit::returnPressed, ui->joinBtn, &QPushButton::click);

    connect(ui->sendBtn, &QPushButton::clicked, this, &MyWidget::sendBtnHit);
    connect(ui->msgLineEdit, &QLineEdit::returnPressed, ui->sendBtn, &QPushButton::click);
}

MyWidget::~MyWidget() {
    sock->close();
    delete ui;
}

void MyWidget::joinBtnHit() {
    sock = new QUdpSocket(this);

    /* TODO:
     *  - Ustalić adres gniazda sock na dowolny adres IPv4 i wybrany port
     *    uwaga: Qt domyślnie używa IPv6+4; należy podać QHostAddress::AnyIPv4
     *           jako adres na którym ma być wykonany bind
     *    uwaga: do testowania programu z localhosta należy podać
     *           QUdpSocket::ShareAddress jako trzeci argument metody bind
     *  - Połączyć zdarzenia z funkcjami które je obsługują
     *  - Zażądać dołączenia do wybranej grupy multicastowej
     *  Pamiętać żeby sprawdzić czy udało się ustalić adres i dołączyć do grupy!
     */

    ui->msgsTextEdit->append("<b>Joined multiast group " + ui->groupLineEdit->text() + ":" + QString::number(ui->portSpinBox->value()) + "</b>");
    ui->joinGroup->setEnabled(false);
    ui->talkGroup->setEnabled(true);
}

void MyWidget::socketError(QUdpSocket::SocketError) {
    QMessageBox::critical(this, "Error", sock->errorString());
    ui->msgsTextEdit->append("<b>Socket error: " + sock->errorString() + "</b>");
    ui->talkGroup->setEnabled(false);
}

void MyWidget::sendBtnHit() {
    auto txt = ui->msgLineEdit->text().trimmed();

    /* TODO: wysłać tekst na adres grupy multicastowej */

    ui->msgLineEdit->clear();
    ui->msgLineEdit->setFocus();
}
