#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <gphoto2/gphoto2.h>

using namespace std;

typedef struct _Error Error;
struct _Error {
    int rc;
     const char *errorText;
};

typedef struct _GPhotoParams GPhotoParams;
struct _GPhotoParams {
    Camera *camera;
    GPContext *context;
    char *folder;
    char *filename;
    CameraAbilitiesList *abilitiesList;
    GPPortInfoList *portInfoList;
    int debugFuncId;
};

/**
 * @brief The Camera class
 */
class QTLCamera {
public:
    QTLCamera();

    void setWorkingDirectory(const char *);
    Error detectCamera();
    Error initCamera();
    int findWidgetByName(GPhotoParams *p, const char *, CameraWidget **, CameraWidget **);
    int setConfigAction(GPhotoParams *p,const char *name, const char *value);
    void captureImage();

private:
    void _updateParams();
    void _captureImage();
    void _deleteImage();

    char *workingDirectory;
    GPhotoParams *params;
};

enum CAMERA_STATUS {
    CAMERA_OK,
    CAMERA_NOT_FOUND,
    CAMERA_NOT_INITIALIZED
};

#endif // CAMERAHANDLER_H
