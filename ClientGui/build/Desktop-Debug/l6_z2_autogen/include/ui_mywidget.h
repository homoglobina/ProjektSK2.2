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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
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
    QWidget *tab_Game;
    QColumnView *columnView;
    QPushButton *leaveButton;

    void setupUi(QWidget *MyWidget)
    {
        if (MyWidget->objectName().isEmpty())
            MyWidget->setObjectName("MyWidget");
        MyWidget->resize(637, 521);
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
        portSpinBox->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::PlusMinus);
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
        talkGroup->setAlignment(Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignHCenter);
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
        tabWidget->addTab(tab_lobbies, QString());
        tab_Game = new QWidget();
        tab_Game->setObjectName("tab_Game");
        columnView = new QColumnView(tab_Game);
        columnView->setObjectName("columnView");
        columnView->setGeometry(QRect(40, 30, 256, 192));
        leaveButton = new QPushButton(tab_Game);
        leaveButton->setObjectName("leaveButton");
        leaveButton->setGeometry(QRect(500, 80, 84, 26));
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
        tabWidget->setTabText(tabWidget->indexOf(tab_lobbies), QCoreApplication::translate("MyWidget", "Lobbies", nullptr));
        leaveButton->setText(QCoreApplication::translate("MyWidget", "Leave", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Game), QCoreApplication::translate("MyWidget", "Game", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyWidget: public Ui_MyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYWIDGET_H
