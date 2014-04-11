#ifndef TIMELAPSE_H
#define TIMELAPSE_H

typedef struct _TimeLapseParams TimeLapseParams;
struct _TimeLapseParams {
    int interval;
    int framesPerInterval;
    int maxRuntime;
    int maxFrames;
};

class TimeLapse {
public:
    TimeLapse();
    void setParams(TimeLapseParams *);
    void setInterval(int);
    void setFramesPerInterval(int);
    void setMaxRuntime(int);
    void setMaxFrames(int);
    TimeLapseParams *getParams();
    int start();
    int stop();
    int preview();

private:
    TimeLapseParams params;
};

#endif // TIMELAPSE_H
