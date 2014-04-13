#ifndef TIMELAPSE_H
#define TIMELAPSE_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTimer>

#include "camera.h"

/**
 * @brief TimeLapseParams
 */
typedef struct _TimeLapseParams TimeLapseParams;
struct _TimeLapseParams {
    int interval;
    int framesPerInterval;
    int maxRuntime;
    int maxFrames;

    bool retrieveImages;
    bool deleteImages;
};

class TimeLapseWorker : public QObject {
    Q_OBJECT

    bool halt = false;

public slots:
    void captureImages(QTLCamera *, unsigned long, int, bool, bool);
    void haltCapture();

signals:
    void resultReady(const QString &);
};

/**
 * @brief The TimeLapse class
 */
class TimeLapse : public QObject {
    Q_OBJECT
    QThread workerThread;

public:
    TimeLapse();
    void setParams(TimeLapseParams *);
    void setInterval(int);
    void setFramesPerInterval(int);
    void setMaxRuntime(int);
    void setMaxFrames(int);
    void setRetrieveImages(bool);
    void setDeleteImages(bool);
    TimeLapseParams *getParams();

    int initCamera();
    int detectCamera();

    void start();
    void stop();
    string preview();

    QTLCamera *camera;

public slots:
    void handleCapture(const QString &);

signals:
    void startCapture(QTLCamera *, unsigned long, int, bool, bool);

private:
    TimeLapseParams params;
    TimeLapseWorker *worker;
};

#endif // TIMELAPSE_H
