#include "app.h"
#include <QApplication>

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QTimeLapse w;
    w.show();

    return a.exec();
}
