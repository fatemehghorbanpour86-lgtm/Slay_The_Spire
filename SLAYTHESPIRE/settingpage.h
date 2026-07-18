#ifndef SETTINGPAGE_H
#define SETTINGPAGE_H

#include <QDialog>
#include <QPushButton>
#include <QString>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(const QString& currentUsername, QWidget* parent = nullptr);

signals:
    void usernameChanged(const QString& newUsername);

private slots:
    void onToggleSoundClicked();
    void onChangeUsernameClicked();

private:
    QString username;

    QPushButton* toggleSoundBtn;
    QPushButton* changeUsernameBtn;
    QPushButton* closeBtn;

    void setupUI();
    void refreshSoundButtonText();
};

#endif // SETTINGPAGE_H
