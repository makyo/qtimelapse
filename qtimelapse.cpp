#include "qtimelapse.h"
#include "ui_qtimelapse.h"

QTimeLapse::QTimeLapse(QWidget *parent) : QMainWindow(parent), ui(new Ui::QTimeLapse) {
    ui->setupUi(this);
}

QTimeLapse::~QTimeLapse() {
    delete ui;
}
