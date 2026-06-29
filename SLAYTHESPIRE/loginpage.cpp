#include "loginpage.h"
#include <QVBoxLayout>
#include <QBoxLayout>

loginpage::loginpage(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("loginPage");
    setStyleSheet("QWidget#loginPage {border-image: url(:/loginpage.png); }"
                  "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
                  "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
                  "}"
                  );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 20 ,0);
    mainLayout->addStretch();

    QVBoxLayout *inputLayout = new QVBoxLayout();
    inputLayout->setAlignment(Qt::AlignCenter);

    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Username");
    usernameInput->setFixedSize(250, 40);
    usernameInput->setStyleSheet("QLineEdit { background-color: rgba(31, 41, 55, 0.8);"
                                 "color: white;"
                                 "border: 1px solid #5a5a54;"
                                 "padding: 5px;"
                                 "font-weight: bold;"
                                 "}");

    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setFixedSize(250, 40);
    passwordInput->setStyleSheet("QLineEdit { background-color: rgba(31, 41, 55, 0.8);"
                                 "color: white;"
                                 "border: 1px solid #5a5a54;"
                                 "padding: 5px;"
                                 "font-weight: bold;"
                                 "}");

    inputLayout->addWidget(usernameInput);
    inputLayout->addWidget(passwordInput);
    inputLayout->setSpacing(10);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setAlignment(Qt::AlignCenter);

    loginBtn = new QPushButton(this);
    loginBtn->setFixedSize(120, 120);
    loginBtn->setToolTip("If you have an account, Press this.");
    loginBtn->setStyleSheet("QWidget {border-image: url(:/LoginBtn.png); }");

    registerBtn = new QPushButton(this);
    registerBtn->setFixedSize(120, 120);
    registerBtn->setToolTip("IF you dont have account, Press this.");
    registerBtn->setStyleSheet("QWidget {border-image: url(:/RegisterBtn.png); }");

    buttonsLayout->addWidget(loginBtn);
    buttonsLayout->addWidget(registerBtn);

    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addStretch();

    connect(loginBtn, &QPushButton::clicked, this, [this]()
            {emit loginSuccess();});
}
