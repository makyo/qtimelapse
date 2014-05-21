#include "project.h"

using namespace libconfig;

/**
 * @brief QTLProject::QTLProject
 */
QTLProject::QTLProject() {
    filename = string("");
    initialize();
}

/**
 * @brief QTLProject::QTLProject
 * @param filename
 */
QTLProject::QTLProject(string newFilename) {
    filename = newFilename;
    config.readFile(filename.c_str());
    // TODO check for qtlVersion setting
    initialize();
}

void QTLProject::initialize() {
    Setting &root = config.getRoot();

    if (!root.exists("qtlVersion")) {
        root.add("qtlVersion", Setting::TypeString) = "0.0.1";
    }
    if (!root.exists("workingDirectory")) {
        root.add("workingDirectory", Setting::TypeString) = "";
    }

    if (!root.exists("timelapse")) {
        Setting &timelapse = root.add("timelapse", Setting::TypeGroup);
        timelapse.add("interval", Setting::TypeInt) = 10;
        timelapse.add("framesPerInterval", Setting::TypeInt) = 1;
        timelapse.add("maxFrames", Setting::TypeInt) = 10;
        timelapse.add("maxRuntime", Setting::TypeInt) = 0;
        timelapse.add("retrieveImages", Setting::TypeBoolean) = true;
        timelapse.add("deleteImages", Setting::TypeBoolean) = true;
    }

    if (!root.exists("camera")) {
        Setting &camera = root.add("camera", Setting::TypeGroup);
        camera.add("widgets", Setting::TypeList);
    }
}

/**
 * @brief QTLProject::setfilename
 * @param newfilename
 */
void QTLProject::setFilename(string newFilename) {
    filename = newFilename;
}

/**
 * @brief QTLProject::getfilename
 * @return
 */
string * QTLProject::getFilename() {
    return &filename;
}

/**
 * @brief QTLProject::save
 * @param workingDirectory
 * @param timeLapse
 * @param camera
 */
void QTLProject::save(TimeLapse *timeLapse) {
    TimeLapseParams *params = timeLapse->getParams();

    config.lookup("workingDirectory") = timeLapse->camera->getWorkingDirectory();

    config.lookup("timelapse.interval") = params->interval;
    config.lookup("timelapse.framesPerInterval") = params->framesPerInterval;
    config.lookup("timelapse.maxFrames") = params->maxFrames;
    config.lookup("timelapse.maxRuntime") = params->maxRuntime;
    config.lookup("timelapse.retrieveImages") = params->retrieveImages;
    config.lookup("timelapse.deleteImages") = params->deleteImages;

    // TODO - save camera widgets to camera group

    config.writeFile(filename.c_str());
}

/**
 * @brief QTLProject::load
 * @param timeLapse
 */
void QTLProject::load(TimeLapse *timeLapse) {
    TimeLapseParams *params = new TimeLapseParams();

    config.lookupValue("timelapse.interval", params->interval);
    config.lookupValue("timelapse.framesPerInterval", params->framesPerInterval);
    config.lookupValue("timelapse.maxFrames", params->maxFrames);
    config.lookupValue("timelapse.maxRuntime", params->maxRuntime);
    config.lookupValue("timelapse.retrieveImages", params->retrieveImages);
    config.lookupValue("timelapse.deleteImages", params->deleteImages);

    timeLapse->setParams(params);
    // TODO - load camera widgets from camera group
}
