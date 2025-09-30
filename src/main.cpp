#include <iostream>

#include <QApplication>
#include <Window.h>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    Window w;

    // window settings
    w.setWindowIcon(QIcon(":icon.png"));
    w.setWindowIconText("Tanca DataAcquisition");
    w.setWindowTitle("Tanca DataAcquisition");
    w.showMinimized();
    w.setFixedSize(600, 800);

    w.show();

    return app.exec();

}
