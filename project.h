#ifndef PROJECT_H
#define PROJECT_H

#include <fstream>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "camera.h"
#include "timelapse.h"

class QTLProject {
public:
    QTLProject();
    QTLProject(const string *);

    void setFilename(string *);
    void save(TimeLapse *, QTLCamera *);

private:
    string *filename;
    YAML::Node config;
};

#endif // PROJECT_H
