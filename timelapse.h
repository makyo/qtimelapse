#ifndef TIMELAPSE_H
#define TIMELAPSE_H

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

/**
 * @brief The TimeLapse class
 */
class TimeLapse {
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

    int start();
    int stop();
    string preview();

    QTLCamera *camera;

private:
    TimeLapseParams params;
};

#endif // TIMELAPSE_H