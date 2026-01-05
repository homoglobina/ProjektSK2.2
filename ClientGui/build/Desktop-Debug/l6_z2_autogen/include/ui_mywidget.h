/********************************************************************************
** Form generated from reading UI file 'mywidget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYWIDGET_H
#define UI_MYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QColumnView>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyWidget
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab_login;
    QGroupBox *joinGroup;
    QHBoxLayout *horizontalLayout;
    QLineEdit *groupLineEdit;
    QSpinBox *portSpinBox;
    QPushButton *joinBtn;
    QGroupBox *talkGroup;
    QGridLayout *gridLayout;
    QPushButton *sendBtn;
    QLineEdit *msgNickEdit;
    QTextEdit *msgsTextEdit;
    QWidget *tab_lobbies;
    QListWidget *listWidget;
    QPushButton *disconectButton;
    QPushButton *refreshButton;
    QPushButton *joinButton;
    QLabel *label_2;
    QWidget *tab_Game;
    QColumnView *columnView;
    QPushButton *leaveButton;
    QLabel *label;
    QPushButton *startGameButton;
    QTextEdit *gameTextEdit;
    QGroupBox *groupBox;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLineEdit *answerEdit1;
    QPushButton *sendAnswersButton;
    QComboBox *categoryBox;
    QTableWidget *scoreTable;
    QTableWidget *categoryWidget;

    void setupUi(QWidget *MyWidget)
    {
        if (MyWidget->objectName().isEmpty())
            MyWidget->setObjectName("MyWidget");
        MyWidget->resize(968, 621);
        verticalLayout = new QVBoxLayout(MyWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(MyWidget);
        tabWidget->setObjectName("tabWidget");
        tab_login = new QWidget();
        tab_login->setObjectName("tab_login");
        joinGroup = new QGroupBox(tab_login);
        joinGroup->setObjectName("joinGroup");
        joinGroup->setGeometry(QRect(0, 40, 400, 50));
        horizontalLayout = new QHBoxLayout(joinGroup);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        groupLineEdit = new QLineEdit(joinGroup);
        groupLineEdit->setObjectName("groupLineEdit");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupLineEdit->sizePolicy().hasHeightForWidth());
        groupLineEdit->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(groupLineEdit);

        portSpinBox = new QSpinBox(joinGroup);
        portSpinBox->setObjectName("portSpinBox");
        portSpinBox->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::UpDownArrows);
        portSpinBox->setMinimum(1);
        portSpinBox->setMaximum(65535);
        portSpinBox->setValue(2000);

        horizontalLayout->addWidget(portSpinBox);

        joinBtn = new QPushButton(joinGroup);
        joinBtn->setObjectName("joinBtn");

        horizontalLayout->addWidget(joinBtn);

        talkGroup = new QGroupBox(tab_login);
        talkGroup->setObjectName("talkGroup");
        talkGroup->setEnabled(false);
        talkGroup->setGeometry(QRect(0, 90, 400, 231));
        talkGroup->setFlat(false);
        gridLayout = new QGridLayout(talkGroup);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName("gridLayout");
        sendBtn = new QPushButton(talkGroup);
        sendBtn->setObjectName("sendBtn");

        gridLayout->addWidget(sendBtn, 1, 1, 1, 1);

        msgNickEdit = new QLineEdit(talkGroup);
        msgNickEdit->setObjectName("msgNickEdit");

        gridLayout->addWidget(msgNickEdit, 1, 0, 1, 1);

        msgsTextEdit = new QTextEdit(talkGroup);
        msgsTextEdit->setObjectName("msgsTextEdit");
        msgsTextEdit->setReadOnly(true);

        gridLayout->addWidget(msgsTextEdit, 0, 0, 1, 2);

        tabWidget->addTab(tab_login, QString());
        tab_lobbies = new QWidget();
        tab_lobbies->setObjectName("tab_lobbies");
        listWidget = new QListWidget(tab_lobbies);
        listWidget->setObjectName("listWidget");
        listWidget->setGeometry(QRect(10, 50, 256, 192));
        disconectButton = new QPushButton(tab_lobbies);
        disconectButton->setObjectName("disconectButton");
        disconectButton->setGeometry(QRect(490, 10, 84, 26));
        refreshButton = new QPushButton(tab_lobbies);
        refreshButton->setObjectName("refreshButton");
        refreshButton->setGeometry(QRect(490, 50, 84, 26));
        joinButton = new QPushButton(tab_lobbies);
        joinButton->setObjectName("joinButton");
        joinButton->setGeometry(QRect(490, 90, 84, 26));
        label_2 = new QLabel(tab_lobbies);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 20, 57, 16));
        tabWidget->addTab(tab_lobbies, QString());
        tab_Game = new QWidget();
        tab_Game->setObjectName("tab_Game");
        columnView = new QColumnView(tab_Game);
        columnView->setObjectName("columnView");
        columnView->setEnabled(false);
        columnView->setGeometry(QRect(10, 80, 256, 192));
        leaveButton = new QPushButton(tab_Game);
        leaveButton->setObjectName("leaveButton");
        leaveButton->setGeometry(QRect(110, 50, 84, 26));
        label = new QLabel(tab_Game);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 50, 111, 16));
        startGameButton = new QPushButton(tab_Game);
        startGameButton->setObjectName("startGameButton");
        startGameButton->setGeometry(QRect(110, 20, 84, 26));
        gameTextEdit = new QTextEdit(tab_Game);
        gameTextEdit->setObjectName("gameTextEdit");
        gameTextEdit->setGeometry(QRect(400, 20, 291, 231));
        groupBox = new QGroupBox(tab_Game);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(0, 400, 251, 171));
        formLayoutWidget = new QWidget(groupBox);
        formLayoutWidget->setObjectName("formLayoutWidget");
        formLayoutWidget->setGeometry(QRect(0, 20, 163, 151));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        answerEdit1 = new QLineEdit(formLayoutWidget);
        answerEdit1->setObjectName("answerEdit1");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, answerEdit1);

        sendAnswersButton = new QPushButton(formLayoutWidget);
        sendAnswersButton->setObjectName("sendAnswersButton");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, sendAnswersButton);

        categoryBox = new QComboBox(groupBox);
        categoryBox->setObjectName("categoryBox");
        categoryBox->setGeometry(QRect(170, 80, 75, 22));
        scoreTable = new QTableWidget(tab_Game);
        if (scoreTable->columnCount() < 3)
            scoreTable->setColumnCount(3);
        scoreTable->setObjectName("scoreTable");
        scoreTable->setGeometry(QRect(400, 290, 321, 192));
        scoreTable->setColumnCount(3);
        categoryWidget = new QTableWidget(tab_Game);
        categoryWidget->setObjectName("categoryWidget");
        categoryWidget->setGeometry(QRect(10, 300, 221, 71));
        tabWidget->addTab(tab_Game, QString());

        verticalLayout->addWidget(tabWidget);

        QWidget::setTabOrder(portSpinBox, groupLineEdit);
        QWidget::setTabOrder(groupLineEdit, joinBtn);
        QWidget::setTabOrder(joinBtn, msgNickEdit);
        QWidget::setTabOrder(msgNickEdit, sendBtn);
        QWidget::setTabOrder(sendBtn, msgsTextEdit);

        retranslateUi(MyWidget);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MyWidget);
    } // setupUi

    void retranslateUi(QWidget *MyWidget)
    {
        MyWidget->setWindowTitle(QCoreApplication::translate("MyWidget", "Simple UDP multicast client", nullptr));
        joinGroup->setTitle(QString());
        groupLineEdit->setText(QCoreApplication::translate("MyWidget", "239.0.0.1", nullptr));
        joinBtn->setText(QCoreApplication::translate("MyWidget", "connect", nullptr));
        talkGroup->setTitle(QString());
        sendBtn->setText(QCoreApplication::translate("MyWidget", "send", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_login), QCoreApplication::translate("MyWidget", "Join", nullptr));
        disconectButton->setText(QCoreApplication::translate("MyWidget", "Disconnect", nullptr));
        refreshButton->setText(QCoreApplication::translate("MyWidget", "Refresh", nullptr));
        joinButton->setText(QCoreApplication::translate("MyWidget", "Join", nullptr));
        label_2->setText(QCoreApplication::translate("MyWidget", "Lobbies", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_lobbies), QCoreApplication::translate("MyWidget", "Lobbies", nullptr));
        leaveButton->setText(QCoreApplication::translate("MyWidget", "Leave", nullptr));
        label->setText(QCoreApplication::translate("MyWidget", "Players in Lobby", nullptr));
        startGameButton->setText(QCoreApplication::translate("MyWidget", "Start", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MyWidget", "Odpowiedzi", nullptr));
        sendAnswersButton->setText(QCoreApplication::translate("MyWidget", "Wyslij", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Game), QCoreApplication::translate("MyWidget", "Game", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyWidget: public Ui_MyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYWIDGET_H
