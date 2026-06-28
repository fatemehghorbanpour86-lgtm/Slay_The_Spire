#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QStyleOption>
#include <QPainter>

class loginpage : public QWidget
{
    Q_OBJECT
public:
    explicit loginpage(QWidget *parent = nullptr);

signals:
    void loginSuccess();
    void registerRequsted();

private:
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QPushButton *loginBtn;
    QPushButton *registerBtn;

protected:
    void paintEvent(QPaintEvent *) override{
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // LOGINPAGE_H
