#ifndef PAUSEVIEWER_H
#define PAUSEVIEWER_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>

class PauseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PauseDialog(QWidget* parent = nullptr);

signals:
    void resumeRequested();
    void settingsRequested();
    void saveAndQuitRequested();
    void giveUpRequested();

private slots:
    void onResumeClicked();
    void onSettingsClicked();
    void onSaveAndQuitClicked();
    void onGiveUpClicked();

private:
    QLabel* titleLabel;
    QPushButton* resumeBtn;
    QPushButton* settingsBtn;
    QPushButton* saveQuitBtn;
    QPushButton* giveUpBtn;

    void setupUI();
};


#endif // PAUSEVIEWER_H
