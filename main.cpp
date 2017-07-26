#include "wndmain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WndMain w;

    QFile l_css(":/qdarkstyle/style.qss");
    if (l_css.open(QFile::ReadOnly))
        a.setStyleSheet(QString::fromUtf8(l_css.readAll()));

    w.show();
    return a.exec();
}
