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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);

    toggleSoundBtn = new QPushButton(this);
    toggleSoundBtn->setCursor(Qt::PointingHandCursor);
    refreshSoundButtonText();
    connect(toggleSoundBtn, &QPushButton::clicked, this, &SettingsDialog::onToggleSoundClicked);

    changeUsernameBtn = new QPushButton("Change Username", this);
    changeUsernameBtn->setCursor(Qt::PointingHandCursor);
    connect(changeUsernameBtn, &QPushButton::clicked, this, &SettingsDialog::onChangeUsernameClicked);

    closeBtn = new QPushButton("Close", this);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    mainLayout->addWidget(toggleSoundBtn);
    mainLayout->addWidget(changeUsernameBtn);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(closeBtn);
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

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLabel* label = new QLabel("Enter New Username", &dialog);
    layout->addWidget(label);

    QLineEdit* usernameEdit = new QLineEdit(&dialog);
    layout->addWidget(usernameEdit);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    QPushButton* confirmBtn = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton* cancelBtn  = buttonBox->button(QDialogButtonBox::Cancel);
    confirmBtn->setText("Confirm");
    cancelBtn->setText("Cancel");
    layout->addWidget(buttonBox);

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