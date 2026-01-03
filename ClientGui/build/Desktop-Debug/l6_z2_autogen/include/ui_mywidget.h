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
    QWidget *tab_7;
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
    QWidget *tab_8;

    void setupUi(QWidget *MyWidget)
    {
        if (MyWidget->objectName().isEmpty())
            MyWidget->setObjectName("MyWidget");
        MyWidget->resize(609, 435);
        verticalLayout = new QVBoxLayout(MyWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(MyWidget);
        tabWidget->setObjectName("tabWidget");
        tab_7 = new QWidget();
        tab_7->setObjectName("tab_7");
        joinGroup = new QGroupBox(tab_7);
        joinGroup->setObjectName("joinGroup");
        joinGroup->setGeometry(QRect(-500, 0, 985, 37));
        horizontalLayout = new QHBoxLayout(joinGroup);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        groupLineEdit = new QLineEdit(joinGroup);
        groupLineEdit->setObjectName("groupLineEdit");

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
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(joinBtn->sizePolicy().hasHeightForWidth());
        joinBtn->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(joinBtn);

        talkGroup = new QGroupBox(tab_7);
        talkGroup->setObjectName("talkGroup");
        talkGroup->setEnabled(false);
        talkGroup->setGeometry(QRect(-280, 110, 621, 231));
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

        tabWidget->addTab(tab_7, QString());
        tab_8 = new QWidget();
        tab_8->setObjectName("tab_8");
        tabWidget->addTab(tab_8, QString());

        verticalLayout->addWidget(tabWidget);

        QWidget::setTabOrder(portSpinBox, groupLineEdit);
        QWidget::setTabOrder(groupLineEdit, joinBtn);
        QWidget::setTabOrder(joinBtn, msgLineEdit);
        QWidget::setTabOrder(msgLineEdit, sendBtn);
        QWidget::setTabOrder(sendBtn, msgsTextEdit);

        retranslateUi(MyWidget);

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
        tabWidget->setTabText(tabWidget->indexOf(tab_7), QCoreApplication::translate("MyWidget", "Tab 1", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_8), QCoreApplication::translate("MyWidget", "Tab 2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyWidget: public Ui_MyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYWIDGET_H
