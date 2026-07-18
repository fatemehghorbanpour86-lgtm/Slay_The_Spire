#include "settingpage.h"
#include "audiomanager.h"
#include "savemanager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>

SettingsDialog::SettingsDialog(const QString& currentUsername, QWidget* parent)
    : QDialog(parent), username(currentUsername)
{
    setWindowTitle("Settings");
    setupUI();
}

void SettingsDialog::setupUI()
{
    setObjectName("SettingPage");
    setStyleSheet(
        "#SettingPage { border-image: url(:/Reward/SelectionViewer.png); }");
    setFixedSize(500, 450);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(140, 30, 140, 30);
    mainLayout->setSpacing(20);

    QLabel* title = new QLabel("Setting", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #facc15; font-size: 30px; font-weight: bold; background: transparent;");
    mainLayout->addWidget(title);
    mainLayout->addStretch(1);


    toggleSoundBtn = new QPushButton(this);
    toggleSoundBtn->setCursor(Qt::PointingHandCursor);
    toggleSoundBtn->setStyleSheet(
        "QPushButton { border-image: url(:/SettingPushButton.png); font-size: 15px;"
        "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );
    toggleSoundBtn->setFixedSize(220, 60);
    connect(toggleSoundBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    refreshSoundButtonText();
    connect(toggleSoundBtn, &QPushButton::clicked, this, &SettingsDialog::onToggleSoundClicked);


    changeUsernameBtn = new QPushButton("Change Username", this);
    changeUsernameBtn->setCursor(Qt::PointingHandCursor);
    changeUsernameBtn->setStyleSheet(
        "QPushButton { border-image: url(:/SettingPushButton.png); font-size: 15px;"
        "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );
    changeUsernameBtn->setFixedSize(220, 60);
    connect(changeUsernameBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    connect(changeUsernameBtn, &QPushButton::clicked, this, &SettingsDialog::onChangeUsernameClicked);

    closeBtn = new QPushButton(this);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { border-image: url(:/RestSite/BackBtn.png); border: none; background: transparent; }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );
    closeBtn->move(0,300);
    closeBtn->setFixedSize(140, 80);
    connect(closeBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    mainLayout->addWidget(toggleSoundBtn);
    mainLayout->addWidget(changeUsernameBtn);
    mainLayout->addStretch(4);
}

void SettingsDialog::refreshSoundButtonText()
{
    bool muted = AudioManager::instance().isMuted();
    toggleSoundBtn->setText(muted ? "Unmute Sound" : "Mute Sound");
}

void SettingsDialog::onToggleSoundClicked()
{
    AudioManager& audio = AudioManager::instance();
    audio.setMuted(!audio.isMuted());
    refreshSoundButtonText();
}

void SettingsDialog::onChangeUsernameClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Change Username");
    dialog.setObjectName("ChangeUsername");
    dialog.setStyleSheet(
        "#ChangeUsername { border-image: url(:/ChangeUsernameViewer.png); }");
    dialog.setFixedSize(450, 400);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 40, 20, 140);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* label = new QLabel("Enter New Username", &dialog);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: #facc15 ; font-size: 28px; font-weight: bold; background: transparent;");
    layout->addWidget(label);

    layout->addSpacing(25);

    QLineEdit* usernameEdit = new QLineEdit(&dialog);
    usernameEdit->setPlaceholderText("New Username");
    usernameEdit->setFixedSize(250, 40);
    usernameEdit->setStyleSheet("QLineEdit { background-color: rgba(31, 41, 55, 0.8);"
                                 "color: white;"
                                 "border: 1px solid #5a5a54;"
                                 "padding: 5px;"
                                 "font-weight: bold;"
                                 "}");
    layout->addWidget(usernameEdit, 0, Qt::AlignCenter);

    QPushButton* cancelBtn = new QPushButton(&dialog);
    cancelBtn->setFixedSize(150, 100);
    cancelBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/NoBtn.png); }"
                             "QPushButton:pressed { margin: 5px 5px 5px 5px; }");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->move(0,270);

    QPushButton* confirmBtn = new QPushButton(&dialog);
    confirmBtn->setFixedSize(150, 100);
    confirmBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/YesBtn.png); }"
                              "QPushButton:pressed { margin: 5px 5px 5px 5px; }");
    confirmBtn->setCursor(Qt::PointingHandCursor);
    confirmBtn->move(300,270);


    connect(cancelBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });

    connect(confirmBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });


    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    connect(confirmBtn, &QPushButton::clicked, &dialog, [&]()
            {
                QString newUsername = usernameEdit->text().trimmed();

                if (newUsername.isEmpty())
                {
                    QMessageBox::warning(&dialog, "Change Username", "Username cannot be empty.");
                    return; // Dialog stays open
                }

                if (newUsername == username)
                {
                    QMessageBox::information(&dialog, "Change Username",
                                             "This is already your current username.");
                    return; // Dialog stays open
                }

                if (!SaveManager::changeUsername(username, newUsername))
                {
                    QMessageBox::warning(&dialog, "Change Username", "Username already exists.");
                    return; // Dialog stays open
                }

                username = newUsername;
                emit usernameChanged(newUsername);

                dialog.accept();
            });

    dialog.exec();
}