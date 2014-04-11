#ifndef QTIMELAPSE_H
#define QTIMELAPSE_H

#include <QFileDialog>
#include <QMainWindow>

#include "camerahandler.h"

namespace Ui {
class QTimeLapse;
}

class QTimeLapse : public QMainWindow {
    Q_OBJECT

public:
    explicit QTimeLapse(QWidget *parent = 0);
    ~QTimeLapse();

private slots:
    void on_actionExit_triggered();
    void on_actionDetect_Camera_triggered();

    void on_btn_captureImage_clicked();
    void on_btn_start_clicked();
    void on_btn_stop_clicked();
    void on_btn_chooseLocation_clicked();

private:
    Ui::QTimeLapse *ui;
    QFileDialog *fileDialog_workingDirectory;
    CameraHandler *cameraHandler;
};

#endif // QTIMELAPSE_H
