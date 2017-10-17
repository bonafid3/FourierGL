#include <QApplication>
#include "dialog.h"
#include <QSurfaceFormat>
#include <QFont>

//extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

int main(int argc, char *argv[])
{
    QSurfaceFormat format;

    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 1);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    Dialog w;
    w.setFont(QFont("Monaco", 8));
    w.setWindowFlags(Qt::Window);
    w.show();

    return a.exec();
}
