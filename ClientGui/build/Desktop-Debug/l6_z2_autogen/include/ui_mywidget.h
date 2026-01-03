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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
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
    QLineEdit *msgLineEdit;
    QTextEdit *msgsTextEdit;
    QWidget *tab_lobbies;
    QWidget *tab_Game;

    void setupUi(QWidget *MyWidget)
    {
        if (MyWidget->objectName().isEmpty())
            MyWidget->setObjectName("MyWidget");
        MyWidget->resize(421, 368);
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

        msgLineEdit = new QLineEdit(talkGroup);
        msgLineEdit->setObjectName("msgLineEdit");

        gridLayout->addWidget(msgLineEdit, 1, 0, 1, 1);

        msgsTextEdit = new QTextEdit(talkGroup);
        msgsTextEdit->setObjectName("msgsTextEdit");
        msgsTextEdit->setReadOnly(true);

        gridLayout->addWidget(msgsTextEdit, 0, 0, 1, 2);

        tabWidget->addTab(tab_login, QString());
        tab_lobbies = new QWidget();
        tab_lobbies->setObjectName("tab_lobbies");
        tabWidget->addTab(tab_lobbies, QString());
        tab_Game = new QWidget();
        tab_Game->setObjectName("tab_Game");
        tabWidget->addTab(tab_Game, QString());

        verticalLayout->addWidget(tabWidget);

        QWidget::setTabOrder(portSpinBox, groupLineEdit);
        QWidget::setTabOrder(groupLineEdit, joinBtn);
        QWidget::setTabOrder(joinBtn, msgLineEdit);
        QWidget::setTabOrder(msgLineEdit, sendBtn);
        QWidget::setTabOrder(sendBtn, msgsTextEdit);

        retranslateUi(MyWidget);

        tabWidget->setCurrentIndex(0);


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
        tabWidget->setTabText(tabWidget->indexOf(tab_login), QCoreApplication::translate("MyWidget", "Tab 1", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_lobbies), QCoreApplication::translate("MyWidget", "Tab 2", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Game), QCoreApplication::translate("MyWidget", "Page", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyWidget: public Ui_MyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYWIDGET_H
