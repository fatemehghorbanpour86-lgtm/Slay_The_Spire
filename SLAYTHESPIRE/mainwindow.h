#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QProgressBar>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class SplashPage;
class loginpage;
class mainpage;
class BattlePage;


class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void showLoginPAge();
    void showMainMenuPage();
    void showBattlePage();
    void showMapPage();
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;

    SplashPage *splashPage;
    loginpage *loginPage;
    mainpage *mainMenuPage;
    BattlePage *battlePage;
};
#endif // MAINWINDOW_H
