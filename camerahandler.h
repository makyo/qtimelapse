#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <gphoto2/gphoto2.h>

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

/*
 * CameraHandler
 *
 * Handle interaction with the camera
 */
class CameraHandler {
    public:
        CameraHandler();
        void GPhotoParamsInit(GPhotoParams *g);
        int findWidgetByName(GPhotoParams *p, const char *, CameraWidget **, CameraWidget **);
        int setConfigAction(GPhotoParams *p,const char *name, const char *value);
        void captureImage();
};

#endif // CAMERAHANDLER_H
