#include "app.h"
#include "ui_qtimelapse.h"

/**
 * @brief QTimeLapse::QTimeLapse
 * @param parent
 */
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

    // The current project YAML.
    project = new QTLProject();

    // Project file dialog.
    fileDialog_project = new QFileDialog(0,
                                         tr("QTimeLapse Project..."),
                                         QString(),
                                         "QTimeLapse project files (*.qtl)");
    fileDialog_project->setDefaultSuffix(QString("qtl"));

    // Set the working directory to the current directory.
    char cwd[FILENAME_MAX];
    GetCurrentDir(cwd, sizeof(cwd));
    cwd[sizeof(cwd) - 1] = '\0';
    timeLapse->camera->setWorkingDirectory(cwd);

    // Attempt to initialize the camera.
    handleCameraDetect(timeLapse->camera->initCamera());

    // Connections
    connect(timeLapse, &TimeLapse::passImageToApp, this,
            &QTimeLapse::receiveImageFromCapture);
    connect(timeLapse, &TimeLapse::updateParamsInApp, this,
            &QTimeLapse::updateParamsFromTimeLapse);

    // Add the preview views to the vector.
    capturePreviewViews.push_back(ui->preview1);
    capturePreviewViews.push_back(ui->preview2);
    capturePreviewViews.push_back(ui->preview3);
    capturePreviewViews.push_back(ui->preview4);
    capturePreviewViews.push_back(ui->preview5);
    capturePreviewViews.push_back(ui->preview6);
    capturePreviewViews.push_back(ui->preview7);
    capturePreviewViews.push_back(ui->preview8);
    capturePreviewViews.push_back(ui->preview9);
    capturePreviewViews.push_back(ui->preview10);
    capturePreviewViews.push_back(ui->preview11);
    capturePreviewViews.push_back(ui->preview12);
    capturePreviewViews.push_back(ui->preview13);
    capturePreviewViews.push_back(ui->preview14);
    capturePreviewViews.push_back(ui->preview15);
    capturePreviewViews.push_back(ui->preview16);
    capturePreviewViews.push_back(ui->preview17);
    capturePreviewViews.push_back(ui->preview18);
    capturePreviewViews.push_back(ui->preview19);
    capturePreviewViews.push_back(ui->preview20);
    capturePreviewViews.push_back(ui->preview21);
    capturePreviewViews.push_back(ui->preview22);
    capturePreviewViews.push_back(ui->preview23);
    capturePreviewViews.push_back(ui->preview24);
    capturePreviewViews.push_back(ui->preview25);
    capturePreviewViews.push_back(ui->preview26);
    capturePreviewViews.push_back(ui->preview27);
    capturePreviewViews.push_back(ui->preview28);
    capturePreviewViews.push_back(ui->preview29);
    capturePreviewViews.push_back(ui->preview30);
    capturePreviewViews.push_back(ui->preview31);
    capturePreviewViews.push_back(ui->preview32);
    capturePreviewViews.push_back(ui->preview33);
    capturePreviewViews.push_back(ui->preview34);
    capturePreviewViews.push_back(ui->preview35);
    capturePreviewViews.push_back(ui->preview36);
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
                             tr( "Error Detecting Camera"),
                             tr(e.errorText));
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
            comboBox->addItem(widget.choices.at(j), QVariant(widget.name));
        }
        QLabel *label = new QLabel(widget.title);
        settingsForm->addRow(label, comboBox);
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setCameraSetting(int)));
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

/**
 * @brief QTimeLapse::setCameraSetting
 * @param item
 */
void QTimeLapse::setCameraSetting(int item) {
    QComboBox *setting = (QComboBox *)sender();
    QString settingName = setting->itemData(item).toString();
    QString settingValue = setting->itemText(item);
    qDebug() << "Setting parameter" << settingName << "to" << settingValue;
    timeLapse->camera->setConfigAction(settingName.toStdString().c_str(), settingValue.toStdString().c_str());
}

/****************************************************************************************
 *
 * Menu actions
 *
 *     File menu
 *
 ***************************************************************************************/

/**
 * @brief QTimeLapse::on_actionNew_triggered
 */
void QTimeLapse::on_actionNew_triggered() {
    project = new QTLProject();
    project->load(timeLapse);
}

/**
 * @brief QTimeLapse::on_actionOpen_triggered
 */
void QTimeLapse::on_actionOpen_triggered() {
    QString filename;
    if (fileDialog_project->exec()) {
        filename = fileDialog_project->selectedFiles().at(0);
        project = new QTLProject(filename.toStdString());
        project->load(timeLapse);
    }
}

/**
 * @brief QTimeLapse::on_actionSave_triggered
 */
void QTimeLapse::on_actionSave_triggered() {
    if (!project->getFilename()->empty()) {
        project->save(timeLapse);
    } else {
        on_actionSave_as_triggered();
    }
}

/**
 * @brief QTimeLapse::on_actionSave_as_triggered
 */
void QTimeLapse::on_actionSave_as_triggered() {
    QString filename;
    if (fileDialog_project->exec()) {
        filename = fileDialog_project->selectedFiles().at(0);
        project->setFilename(filename.toStdString());
        project->save(timeLapse);
    }
}

/**
 * @brief QTimeLapse::on_actionExit_triggered
 */
void QTimeLapse::on_actionExit_triggered() {
    destroy();
}

/****************************************************************************************
 *
 * Menu actions
 *
 *     Camera menu
 *
 ***************************************************************************************/

/**
 * @brief QTimeLapse::on_actionDetect_Camera_triggered
 */
void QTimeLapse::on_actionDetect_Camera_triggered() {
    handleCameraDetect(timeLapse->camera->detectCamera());
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
    setPreview(QString(timeLapse->preview().c_str()));
}

/**
 * @brief QTimeLapse::on_btn_start_clicked
 */
void QTimeLapse::on_btn_start_clicked() {
    timeLapse->start();
    ui->btn_start->setEnabled(false);
    ui->btn_captureImage->setEnabled(false);
    ui->btn_chooseLocation->setEnabled(false);
    setFieldsEnabled(false);
    ui->btn_stop->setEnabled(true);
}

/**
 * @brief QTimeLapse::setPreview
 */
void QTimeLapse::setPreview(const QString &previewFile) {
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
 * @brief QTimeLapse::receiveImageFromCapture
 * @param path
 */
void QTimeLapse::receiveImageFromCapture(const QString &path) {
    setPreview(path);
    ui->statusBar->showMessage("Captured image: " + path);
    capturePreviews.push_back(new QPixmap(path));
    if (capturePreviews.size() > 36) {
        capturePreviews.erase(capturePreviews.begin());
    }
    for (unsigned int i = 0; i < capturePreviews.size(); i++) {
        if (capturePreviewViews.at(i)->scene()) {
            delete capturePreviewViews.at(i)->scene();
        }
        QGraphicsScene *preview = new QGraphicsScene();
        preview->addPixmap(capturePreviews.at(i)->scaled(
                               capturePreviewViews.at(i)->size(), Qt::KeepAspectRatio));
        capturePreviewViews.at(i)->setScene(preview);
    }
}

/**
 * @brief QTimeLapse::on_btn_stop_clicked
 */
void QTimeLapse::on_btn_stop_clicked() {
    timeLapse->stop();
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
    QString filename;
    if (fileDialog_workingDirectory->exec()) {
        filename = fileDialog_workingDirectory->selectedFiles().at(0);
        timeLapse->camera->setWorkingDirectory(filename.toStdString().c_str());
        ui->statusBar->showMessage("Working directory set to: " + filename);
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

/**
 * @brief QTimeLapse::updateParamsFromTimeLapse
 */
void QTimeLapse::updateParamsFromTimeLapse() {
    TimeLapseParams *params = timeLapse->getParams();
    ui->input_interval->setText(QString("%1").arg(params->interval));
    ui->input_framesPerInterval->setText(QString("%1").arg(params->framesPerInterval));
    ui->input_maxFrames->setText(QString("%1").arg(params->maxFrames));
    ui->input_maxRuntime->setText(QString("%1").arg(params->maxRuntime));
    ui->chk_retrieveImages->setChecked(params->retrieveImages);
    ui->chk_deleteImages->setChecked(params->deleteImages);
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
