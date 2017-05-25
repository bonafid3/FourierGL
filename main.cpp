#include <QApplication>
#include "dialog.h"

#include <QFont>

//extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.setFont(QFont("Monaco", 8));
    w.setWindowFlags(Qt::Window);
    w.show();

    return a.exec();
}
