#include "mainpage.h"
#include "audiomanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QStackedWidget>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QCoreApplication>
#include <QDir>

mainpage::mainpage(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("MainMenuPage");

    QLabel *bg = new QLabel(this);
    bg->setGeometry(0, 0, 1280, 720);
    bg->setScaledContents(true);
    bg->lower();
    QString exeDir = QCoreApplication::applicationDirPath();
    QString gifPath = QDir(exeDir).filePath("assets/page/MainMenu.gif");
    QMovie *movie = new QMovie(gifPath);
    bg->setMovie(movie);
    movie->start();

    setStyleSheet(
        "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
        "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 650 , 50);
    mainLayout->addStretch();

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignCenter);

    singlePlayerBtn = new QPushButton(this);
    singlePlayerBtn->setCursor(Qt::PointingHandCursor);
    singlePlayerBtn->setFixedSize(100, 75);
    singlePlayerBtn->setStyleSheet(
        "QPushButton { border-image: url(:/singlePlayerBtn.png); }"
        "QPushButton:pressed { "
        "   margin: 5px 8px 5px 8px; "
        "}"
        );
    singlePlayerBtn->setToolTip("Start Battle!");

    multiPlayerBtn = new QPushButton(this);
    multiPlayerBtn->setCursor(Qt::PointingHandCursor);
    multiPlayerBtn->setFixedSize(100, 75);
    multiPlayerBtn->setStyleSheet(
        "QPushButton { border-image: url(:/multiPalyerBtn.png); }"
        "QPushButton:pressed { "
        "   margin: 5px 8px 5px 8px; "
        "}");
    multiPlayerBtn->setToolTip("Battle in the Tower with Friends!");

    leaderBoardBtn = new QPushButton(this);
    leaderBoardBtn->setCursor(Qt::PointingHandCursor);
    leaderBoardBtn->setFixedSize(100, 75);
    leaderBoardBtn->setStyleSheet(
        "QPushButton { border-image: url(:/leaderBoardBtn.png); }"
        "QPushButton:pressed { "
        "   margin: 5px 8px 5px 8px; "
        "}");
    leaderBoardBtn->setToolTip("Top Tower Slayers!");

    settingBtn = new QPushButton(this);
    settingBtn->setCursor(Qt::PointingHandCursor);
    settingBtn->setFixedSize(100, 75);
    settingBtn->setStyleSheet(
        "QPushButton { border-image: url(:/settingBtn.png); }"
        "QPushButton:pressed { "
        "   margin: 5px 8px 5px 8px; "
        "}");
    settingBtn->setToolTip("Game Settings");

    exitBtn = new QPushButton(this);
    exitBtn->setCursor(Qt::PointingHandCursor);
    exitBtn->setFixedSize(100, 75);
    exitBtn->setStyleSheet(
        "QPushButton { border-image: url(:/ExitBtn.png); }"
        "QPushButton:pressed { "
        "   margin: 5px 8px 5px 8px; "
        "}");
    exitBtn->setToolTip("Leave the Tower");

    buttonsLayout->addWidget(singlePlayerBtn);
    buttonsLayout->addWidget(multiPlayerBtn);
    buttonsLayout->addWidget(leaderBoardBtn);
    buttonsLayout->addWidget(settingBtn);
    buttonsLayout->addWidget(exitBtn);
    buttonsLayout->setSpacing(0);

    mainLayout->addLayout(buttonsLayout);
    mainLayout->addStretch();

    connect(singlePlayerBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });

    connect(multiPlayerBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });

    connect(leaderBoardBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });

    connect(settingBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });

    connect(exitBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });

    connect(singlePlayerBtn, &QPushButton::clicked, this, &mainpage::singlePlayerClicked);
    connect(settingBtn, &QPushButton::clicked, this, &mainpage::settingClicked);

    connect(exitBtn, &QPushButton::clicked, this, [](){

        QTimer::singleShot(250, []() {
            QApplication::quit();
        });
    }
            );
}

void mainpage::singlePlayerClicked()
{
    emit startGame();
}

void mainpage::settingClicked()
{
    emit settingsRequested();
}

