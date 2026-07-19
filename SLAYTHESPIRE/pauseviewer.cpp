#include "pauseviewer.h"
#include "audiomanager.h"

#include <QVBoxLayout>
#include <QMessageBox>

PauseDialog::PauseDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
}

void PauseDialog::setupUI()
{
    setWindowTitle("Paused");
    setObjectName("PauseDialog");
    setStyleSheet(
        "#PauseDialog { border-image: url(:/Reward/SelectionViewer.png); }"
        );
    setFixedSize(400, 480);
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 40, 60, 40);
    mainLayout->setSpacing(20);

    titleLabel = new QLabel("Paused", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "color: #facc15; font-size: 30px; font-weight: bold; background: transparent;");
    mainLayout->addWidget(titleLabel);
    mainLayout->addStretch(1);

    auto makeButton = [this](const QString& text) {
        QPushButton* btn = new QPushButton(text, this);
        btn->setFixedSize(220, 55);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { border-image: url(:/SettingPushButton.png); font-size: 15px;"
            "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
            );
        connect(btn, &QPushButton::pressed, this, []() {
            AudioManager::instance().play(AudioManager::Sound::ButtonClick);
        });
        return btn;
    };

    resumeBtn   = makeButton("Resume");
    settingsBtn = makeButton("Settings");
    saveQuitBtn = makeButton("Save and Quit");
    giveUpBtn   = makeButton("Give Up");

    mainLayout->addWidget(resumeBtn,   0, Qt::AlignHCenter);
    mainLayout->addWidget(settingsBtn, 0, Qt::AlignHCenter);
    mainLayout->addWidget(saveQuitBtn, 0, Qt::AlignHCenter);
    mainLayout->addWidget(giveUpBtn,   0, Qt::AlignHCenter);

    mainLayout->addStretch(2);

    connect(resumeBtn,   &QPushButton::clicked, this, &PauseDialog::onResumeClicked);
    connect(settingsBtn, &QPushButton::clicked, this, &PauseDialog::onSettingsClicked);
    connect(saveQuitBtn, &QPushButton::clicked, this, &PauseDialog::onSaveAndQuitClicked);
    connect(giveUpBtn,   &QPushButton::clicked, this, &PauseDialog::onGiveUpClicked);
}

void PauseDialog::onResumeClicked()
{
    emit resumeRequested();
    accept();
}

void PauseDialog::onSettingsClicked()
{
    emit settingsRequested();
}

void PauseDialog::onSaveAndQuitClicked()
{
    emit saveAndQuitRequested();
}

void PauseDialog::onGiveUpClicked()
{
    // QMessageBox confirm(this);
    // confirm.setWindowTitle("Give Up");
    // confirm.setText("Are you sure you want to give up?\nAll current progress will be lost.");
    // QPushButton* yesBtn = confirm.addButton("Yes", QMessageBox::YesRole);
    // confirm.addButton("No", QMessageBox::NoRole);
    // confirm.exec();

    // if (confirm.clickedButton() == yesBtn)
    // {
    //     emit giveUpRequested();
    // }


    QDialog dialog(this);
    dialog.setWindowTitle("Give Up");
    dialog.setObjectName("GiveUp");
    dialog.setStyleSheet(
        "#GiveUp { border-image: url(:/ChangeUsernameViewer.png); }");
    dialog.setFixedSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 200);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* label = new QLabel("Are you sure you want to give up?\nAll current progress will be lost.", &dialog);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: #facc15 ; font-size: 28px; font-weight: bold; background: transparent;");
    layout->addWidget(label);

    QPushButton* cancelBtn = new QPushButton(&dialog);
    cancelBtn->setFixedSize(150, 100);
    cancelBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/NoBtn.png); }"
                             "QPushButton:pressed { margin: 5px 5px 5px 5px; }");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->move(0,250);

    QPushButton* confirmBtn = new QPushButton(&dialog);
    confirmBtn->setFixedSize(150, 100);
    confirmBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/YesBtn.png); }"
                              "QPushButton:pressed { margin: 5px 5px 5px 5px; }");
    confirmBtn->setCursor(Qt::PointingHandCursor);
    confirmBtn->move(450,250);


    connect(cancelBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });

    connect(confirmBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });

    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    connect(confirmBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted)
    {
        emit giveUpRequested();
    }
}