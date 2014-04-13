#include "app.h"
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
    p.retrieveImages = true;
    p.deleteImages = true;
    timeLapse->setParams(&p);

    timeLapse->camera = new QTLCamera();
    char cwd[FILENAME_MAX];
    GetCurrentDir(cwd, sizeof(cwd));
    cwd[sizeof(cwd) - 1] = '\0';
    timeLapse->camera->setWorkingDirectory(cwd);
    handleCameraDetect(timeLapse->camera->initCamera());
}

/**
 * @brief QTimeLapse::~QTimeLapse
 */
QTimeLapse::~QTimeLapse() {
    delete ui;
}

/**
 * @brief QTimeLapse::handleCameraDetect
 * @param rc
 */
void QTimeLapse::handleCameraDetect(QTLError e) {
    if (e.rc != GP_OK) {
        ui->btn_captureImage->setEnabled(false);
        ui->btn_start->setEnabled(false);
        ui->btn_stop->setEnabled(false);
        setFieldsEnabled(false);
        QMessageBox::warning(this,
                             "Error Detecting Camera",
                             e.errorText);
    } else {
        ui->btn_captureImage->setEnabled(true);
        ui->btn_start->setEnabled(true);
        ui->btn_stop->setEnabled(false);
        setFieldsEnabled(true);
        displayCameraSettings();
    }
}

/**
 * @brief QTimeLapse::displayCameraSettings
 */
void QTimeLapse::displayCameraSettings() {
    vector<QTLWidget> *widgets = timeLapse->camera->getWidgets();
    QFormLayout *settingsForm = new QFormLayout();
    for (unsigned int i = 0; i < widgets->size(); i++) {
        QTLWidget widget = widgets->at(i);
        QComboBox *comboBox = new QComboBox();
        for (unsigned int j = 0; j < widget.choices.size(); j++) {
            comboBox->addItem(widget.choices.at(j));
        }
        QLabel *label = new QLabel(widget.title);
        settingsForm->addRow(label, comboBox);
    }

    // Delete existing layout along with all of its widgets.
    if (ui->grp_cameraSettings->layout() != NULL) {
        QLayoutItem *item;
        while ((item = ui->grp_cameraSettings->layout()->takeAt(0)) != NULL) {
            delete item->widget();
            delete item;
        }
        delete ui->grp_cameraSettings->layout();
    }
    ui->grp_cameraSettings->setLayout(settingsForm);
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
    handleCameraDetect(timeLapse->camera->detectCamera());
}

/**
 * @brief QTimeLapse::on_actionExit_triggered
 */
void QTimeLapse::on_actionExit_triggered() {
    destroy();
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
    QString previewFile = QString(timeLapse->preview().c_str());
    QPixmap *preview = new QPixmap(previewFile);
    if (ui->capturePreview->scene()) {
        delete ui->capturePreview->scene();
    }
    previewScene = new QGraphicsScene();
    previewScene->addPixmap(
                preview->scaled(ui->capturePreview->size(), Qt::KeepAspectRatio));
    previewScene->setSceneRect(preview->rect());
    ui->capturePreview->setScene(previewScene);
}

/**
 * @brief QTimeLapse::on_btn_start_clicked
 */
void QTimeLapse::on_btn_start_clicked() {
    //TODO try to start capturing
    if (true) {
        ui->btn_start->setEnabled(false);
        ui->btn_captureImage->setEnabled(false);
        ui->btn_chooseLocation->setEnabled(false);
        setFieldsEnabled(false);
        ui->btn_stop->setEnabled(true);
    } else {
        //TODO else warn
    }
}

/**
 * @brief QTimeLapse::on_btn_stop_clicked
 */
void QTimeLapse::on_btn_stop_clicked() {
    //TODO stop capturing
    ui->btn_start->setEnabled(true);
    ui->btn_captureImage->setEnabled(true);
    ui->btn_chooseLocation->setEnabled(true);
    setFieldsEnabled(true);

    ui->btn_stop->setEnabled(false);
}

/**
 * @brief QTimeLapse::on_btn_chooseLocation_clicked
 */
void QTimeLapse::on_btn_chooseLocation_clicked() {
    QString fileName;
     if (fileDialog_workingDirectory->exec()) {
         fileName = fileDialog_workingDirectory->selectedFiles().at(0);
         timeLapse->camera->setWorkingDirectory(fileName.toStdString().c_str());
         setStatusTip("Working directory set to: " + fileName);
     }
}

/****************************************************************************************
 *
 * Time Lapse Settings tab
 *
 *     Input fields
 *
 ***************************************************************************************/

/**
 * @brief QTimeLapse::setFieldsEnabled
 * @param enabled
 */
void QTimeLapse::setFieldsEnabled(bool enabled) {
    ui->input_interval->setEnabled(enabled);
    ui->input_framesPerInterval->setEnabled(enabled);
    ui->input_maxRuntime->setEnabled(enabled);
    ui->input_maxFrames->setEnabled(enabled);
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

/****************************************************************************************
 *
 * Camera Settings tab
 *
 *     Global settings
 *
 ***************************************************************************************/

/**
 * @brief QTimeLapse::on_chk_retrieveImages_toggled
 * @param checked
 */
void QTimeLapse::on_chk_retrieveImages_toggled(bool checked) {
    timeLapse->setRetrieveImages(checked);
}

/**
 * @brief QTimeLapse::on_chk_deleteImages_toggled
 * @param checked
 */
void QTimeLapse::on_chk_deleteImages_toggled(bool checked) {
    timeLapse->setDeleteImages(checked);
}
