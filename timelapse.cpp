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
 */
void TimeLapse::start() {
    // Build a threaded worker.
    TimeLapseWorker *worker = new TimeLapseWorker;
    worker->moveToThread(&workerThread);

    // Connect signals from worker to thread and self.
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &QObject::destroyed, this, &TimeLapse::stopCapture);
    connect(this, &TimeLapse::startCapture, worker, &TimeLapseWorker::captureImages);
    connect(worker, &TimeLapseWorker::resultReady, this, &TimeLapse::handleCapture);
    workerThread.start();

    // Begin the capture process.
    unsigned long timeBetweenFrames =
            (params.interval * 1000) / params.framesPerInterval;
    startCapture(camera, timeBetweenFrames, params.maxFrames, params.retrieveImages,
            params.deleteImages);

    // Cancel the capture process after the max runtime.
    if (params.maxRuntime > 0) {
        QTimer::singleShot(params.maxRuntime * 60, this, SLOT(stopCapture()));
    }
}

/**
 * @brief TimeLapse::stop
 */
void TimeLapse::stop() {
    workerThread.quit();
    workerThread.wait();
}

/**
 * @brief stopCapture
 */
void TimeLapse::stopCapture() {
    stop();
}

/**
 * @brief TimeLapse::preview
 * @return
 */
string TimeLapse::preview() {
    return camera->captureImage(true, true);
}

/**
 * @brief handleCapture
 * @param result
 */
void TimeLapse::handleCapture(const QString &result) {
    qDebug() << "Handling image:" << result;
    return;
}

/**
 * @brief TimeLapseWorker::captureImages
 */
void TimeLapseWorker::captureImages(QTLCamera *camera, unsigned long timeBetweenFrames,
                                    int maxFrames, bool retrieveImages,
                                    bool deleteImages) {
    for (int i = 0; i < maxFrames; i++) {
        qDebug() << "Capturing image " << i << "..." << endl;
        emit resultReady(QString::fromStdString(
                             camera->captureImage(retrieveImages, deleteImages)));
        qDebug() << "Sleeping " << timeBetweenFrames << " milliseconds..." << endl;
        workerThread.msleep(timeBetweenFrames);
    }
}
