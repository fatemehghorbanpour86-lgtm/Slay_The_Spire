#include "pauseviewer.h"
#include "audiomanager.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QDir>
#include <QCoreApplication>

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
    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);
    this->setCursor(customCursor);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 40, 60, 40);
    mainLayout->setSpacing(20);

    titleLabel = new QLabel("Paused", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "color: #facc15; font-size: 30px; font-weight: bold; background: transparent;");
    mainLayout->addWidget(titleLabel);
    mainLayout->addStretch(1);

    auto makeButton = [&](const QString& text) {
        QPushButton* btn = new QPushButton(text, this);
        btn->setFixedSize(220, 55);
        btn->setCursor(buttonHoverCursor);
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
    QDialog dialog(this);
    dialog.setWindowTitle("Give Up");
    dialog.setObjectName("GiveUp");
    dialog.setStyleSheet(
        "#GiveUp { border-image: url(:/ChangeUsernameViewer.png); }");
    dialog.setFixedSize(600, 400);
    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);
    dialog.setCursor(customCursor);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

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
    cancelBtn->setCursor(buttonHoverCursor);
    cancelBtn->move(0,250);

    QPushButton* confirmBtn = new QPushButton(&dialog);
    confirmBtn->setFixedSize(150, 100);
    confirmBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/YesBtn.png); }"
                              "QPushButton:pressed { margin: 5px 5px 5px 5px; }");
    confirmBtn->setCursor(buttonHoverCursor);
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