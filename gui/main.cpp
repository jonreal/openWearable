#include <QApplication>

#include "rotation_display.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RotationDisplay display;
    display.show();
    return app.exec();
}
