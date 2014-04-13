#ifndef QTIMELAPSE_H
#define QTIMELAPSE_H

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QLabel>
#include <QLayoutItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QPixmap>

#include <stdio.h>
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

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

    void handleCameraDetect(QTLError);

private slots:
    void displayCameraSettings();

    void on_actionExit_triggered();
    void on_actionDetect_Camera_triggered();

    void on_btn_captureImage_clicked();
    void on_btn_start_clicked();
    void on_btn_stop_clicked();
    void on_btn_chooseLocation_clicked();

    void setFieldsEnabled(bool);
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
    QGraphicsScene *previewScene;
};

#endif // QTIMELAPSE_H
