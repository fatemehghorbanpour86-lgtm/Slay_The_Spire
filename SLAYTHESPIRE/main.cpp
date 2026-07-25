#include "mainwindow.h"
#include "audiomanager.h"
#include <QApplication>
#include <QPixmap>
#include <QCursor>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AudioManager::instance().initialize();

    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);

    QApplication::setOverrideCursor(customCursor);

    MainWindow w;
    w.show();
    return QApplication::exec();

    // از اناهیتا به فاطمه صدامو داری؟
    //ااااااارررررهههههههههههههه
    //بلاخرهههههههههه
}
