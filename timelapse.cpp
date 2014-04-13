#include "timelapse.h"

/**
 * @brief TimeLapse::TimeLapse
 */
TimeLapse::TimeLapse() {
}

/**
 * @brief TimeLapse::setParams
 * @param p
 */
void TimeLapse::setParams(TimeLapseParams *p) {
    setInterval(p->interval);
    setFramesPerInterval(p->framesPerInterval);
    setMaxRuntime(p->maxRuntime);
    setMaxFrames(p->maxFrames);
}

/**
 * @brief TimeLapse::setInterval
 * @param interval
 */
void TimeLapse::setInterval(int interval) {
    params.interval = interval;
}

/**
 * @brief TimeLapse::setFramesPerInterval
 * @param framesPerInterval
 */
void TimeLapse::setFramesPerInterval(int framesPerInterval) {
    params.framesPerInterval = framesPerInterval;
}

/**
 * @brief TimeLapse::setMaxRuntime
 * @param runtime
 */
void TimeLapse::setMaxRuntime(int runtime) {
    params.maxRuntime = runtime;
}

/**
 * @brief TimeLapse::setMaxFrames
 * @param frames
 */
void TimeLapse::setMaxFrames(int frames) {
    params.maxFrames = frames;
}

/**
 * @brief TimeLapse::setRetrieveImages
 * @param retrieve
 */
void TimeLapse::setRetrieveImages(bool ret) {
    params.retrieveImages = ret;
}

/**
 * @brief TimeLapse::setDeleteImages
 * @param del
 */
void TimeLapse::setDeleteImages(bool del) {
    params.deleteImages = del;
}

/**
 * @brief TimeLapse::getParams
 * @return
 */
TimeLapseParams *TimeLapse::getParams() {
    return &params;
}

/**
 * @brief TimeLapse::start
 * @return
 */
int TimeLapse::start() {
    return -1; // Not implemented
}

/**
 * @brief TimeLapse::stop
 * @return
 */
int TimeLapse::stop() {
    return -1; // Not implemented
}

/**
 * @brief TimeLapse::preview
 * @return
 */
int TimeLapse::preview() {
    camera->captureImage(false, true);
}
