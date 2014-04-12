#ifndef QTIMELAPSE_H
#define QTIMELAPSE_H

#include <QFileDialog>
#include <QMainWindow>

#include "camera.h"
#include "timelapse.h"

namespace Ui {
class QTimeLapse;
}

class QTimeLapse : public QMainWindow {
    Q_OBJECT

public:
    explicit QTimeLapse(QWidget *parent = 0);
    ~QTimeLapse();

    void handleCameraDetect(int);

private slots:
    void on_actionExit_triggered();
    void on_actionDetect_Camera_triggered();

    void on_btn_captureImage_clicked();
    void on_btn_start_clicked();
    void on_btn_stop_clicked();
    void on_btn_chooseLocation_clicked();
    void on_input_interval_textChanged(const QString &);
    void on_input_framesPerInterval_textChanged(const QString &);
    void on_input_maxRuntime_textChanged(const QString &);
    void on_input_maxFrames_textChanged(const QString &);
    void on_chk_retrieveImages_toggled(bool checked);
    void on_chk_deleteImages_toggled(bool checked);

private:
    Ui::QTimeLapse *ui;
    QFileDialog *fileDialog_workingDirectory;
    TimeLapse *timeLapse;
};

#endif // QTIMELAPSE_H
