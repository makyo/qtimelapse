#include "project.h"

/**
 * @brief QTLProject::QTLProject
 */
QTLProject::QTLProject() {
    config = YAML::Load("{QTimeLapse: {workingDirectory: null, timeLapse: {}, camera: {widgets: []}}}");
}

/**
 * @brief QTLProject::QTLProject
 * @param filename
 */
QTLProject::QTLProject(const string *newFilename) {
    filename = filename;
    config = YAML::LoadFile(*filename);
}

/**
 * @brief QTLProject::setFilename
 * @param newFilename
 */
void QTLProject::setFilename(string *newFilename) {
    filename = newFilename;
}

/**
 * @brief QTLProject::save
 * @param workingDirectory
 * @param timeLapse
 * @param camera
 */
void QTLProject::save(TimeLapse *timeLapse, QTLCamera *camera) {
    TimeLapseParams *tlParams = timeLapse->getParams();
    config["QTimeLapse"]["timelapse"]["interval"] = tlParams->interval;
    config["QTimeLapse"]["timelapse"]["framesPerInterval"] = tlParams->framesPerInterval;
    config["QTimeLapse"]["timelapse"]["maxRuntime"] = tlParams->maxRuntime;
    config["QTimeLapse"]["timelapse"]["maxFrames"] = tlParams->maxFrames;
    config["QTimeLapse"]["timelapse"]["retrieveImages"] = tlParams->retrieveImages;
    config["QTimeLapse"]["timelapse"]["deleteImages"] = tlParams->deleteImages;

    config["QTimeLapse"]["workingDirectory"] = camera->getWorkingDirectory();

    vector<QTLWidget> *widgets = camera->getWidgets();
    for (int i = 0; i < widgets->size(); i++) {
        QTLWidget widget = widgets->at(i);
        config["QTimeLapse"]["camera"]["widgets"][widget.name] = widget.title;
    }

    ofstream fout(filename->c_str());
    fout << config;
}
