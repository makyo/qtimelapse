#include "qtimelapse.h"
#include "ui_qtimelapse.h"

QTimeLapse::QTimeLapse(QWidget *parent) : QMainWindow(parent), ui(new Ui::QTimeLapse) {
    ui->setupUi(this);

    // Working directory file dialog.
    fileDialog_workingDirectory = new QFileDialog();
    fileDialog_workingDirectory->setFileMode(QFileDialog::DirectoryOnly);

    // The time-lapse itself.
    timeLapse = new TimeLapse();
    TimeLapseParams p;
    p.interval = 10;
    p.framesPerInterval = 1;
    p.maxRuntime = 0;
    p.maxFrames = 10;
    timeLapse->setParams(&p);

    // Handler for the user's camera.
    cameraHandler = new CameraHandler();
    //TODO attempt to detect camera
}

QTimeLapse::~QTimeLapse() {
    delete ui;
}

/****************************************************************************************
 *
 * Menu actions
 *
 ***************************************************************************************/

/**
 * @brief QTimeLapse::on_actionDetect_Camera_triggered
 */
void QTimeLapse::on_actionDetect_Camera_triggered() {
    // detect camera
}

/**
 * @brief QTimeLapse::on_actionExit_triggered
 */
void QTimeLapse::on_actionExit_triggered() {
    this->destroy();
}

/****************************************************************************************
 *
 * Time Lapse Settings tab
 *
 *     Buttons
 *
 ***************************************************************************************/

/**
 * @brief QTimeLapse::on_btn_captureImage_clicked
 */
void QTimeLapse::on_btn_captureImage_clicked() {
    //TODO try to capture a preview image
}

/**
 * @brief QTimeLapse::on_btn_start_clicked
 */
void QTimeLapse::on_btn_start_clicked() {
    //TODO try to start capturing
        //TODO disable start button
        //TODO disable capture button
        //TODO disable fields
        //TODO enable stop button
    //TODO else warn
}

/**
 * @brief QTimeLapse::on_btn_stop_clicked
 */
void QTimeLapse::on_btn_stop_clicked() {
    //TODO stop capturing
    //TODO enable start button
    //TODO enable capture button
    //TODO enable fields
    //TODO disable stop button
}

/**
 * @brief QTimeLapse::on_btn_chooseLocation_clicked
 */
void QTimeLapse::on_btn_chooseLocation_clicked() {
    QString fileName;
     if (fileDialog_workingDirectory->exec()) {
         fileName = fileDialog_workingDirectory->selectedFiles().at(0);
         cameraHandler->setWorkingDirectory(fileName.toStdString().c_str());
         this->setStatusTip("Working directory set to: " + fileName);
     }
}

/**
 * @brief QTimeLapse::on_input_interval_textChanged
 * @param interval
 */
void QTimeLapse::on_input_interval_textChanged(const QString &interval) {
    timeLapse->setInterval(interval.toInt());
}

/**
 * @brief QTimeLapse::on_input_framesPerInterval_textChanged
 * @param framesPerInterval
 */
void QTimeLapse::on_input_framesPerInterval_textChanged(
        const QString &framesPerInterval) {
    timeLapse->setFramesPerInterval(framesPerInterval.toInt());

}

/**
 * @brief QTimeLapse::on_input_maxRuntime_textChanged
 * @param runtime
 */
void QTimeLapse::on_input_maxRuntime_textChanged(const QString &runtime) {
    timeLapse->setMaxRuntime(runtime.toInt());
}

/**
 * @brief QTimeLapse::on_input_maxFrames_textChanged
 * @param frames
 */
void QTimeLapse::on_input_maxFrames_textChanged(const QString &frames) {
    timeLapse->setMaxFrames(frames.toInt());
}
