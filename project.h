#ifndef PROJECT_H
#define PROJECT_H

#include <QDebug>

#include <fstream>
#include <vector>
#include <libconfig.h++>

#include "camera.h"
#include "timelapse.h"

class QTLProject {
public:
    QTLProject();
    QTLProject(string);
    void initialize();
    void setFilename(string);
    string * getFilename();
    void save(TimeLapse *);
    void load(TimeLapse *);

private:
    string filename;
    libconfig::Config config;
};

#endif // PROJECT_H
