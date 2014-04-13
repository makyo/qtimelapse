#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <gphoto2/gphoto2.h>
#include <string>
#include <vector>

#include <QDebug>

using namespace std;

/**
 * @brief QTLError
 */
typedef struct _QTLError QTLError;
struct _QTLError {
    int rc;
    const char *errorText;
};

/**
 * @brief QTLWidget
 */
typedef struct _QTLWidget QTLWidget;
struct _QTLWidget {
    CameraWidget *widget;
    const char *title;
    const char *defaultChoice;
    const char *choiceLabel;
    vector<const char *> choices;
};

/**
 * @brief GPhotoParams
 */
typedef struct _GPhotoParams GPhotoParams;
struct _GPhotoParams {
    Camera *camera;
    GPContext *context;
    char *folder;
    char *filename;
    CameraAbilitiesList *abilitiesList;
    GPPortInfoList *portInfoList;
    int debugFuncId;
    vector<QTLWidget> *widgetList;
};

/**
 * @brief The Camera class
 */
class QTLCamera {
public:
    QTLCamera();

    void setWorkingDirectory(const char *);
    QTLError detectCamera();
    QTLError initCamera();
    int findWidgetByName(const char *, CameraWidget **, CameraWidget **);
    int setConfigAction(const char *, const char *);
    string captureImage(bool, bool);
    vector<QTLWidget> *getWidgets();

private:
    void _updateParams();
    string _captureImage(string, bool, bool);
    QTLError _deleteImage(CameraFilePath *, CameraFile *);
    void _getWidgets(vector<QTLWidget> *, CameraWidget *, char *);

    char workingDirectory[FILENAME_MAX];
    GPhotoParams *params;
    bool paramsChanged;
};

enum CAMERA_STATUS {
    CAMERA_OK,
    CAMERA_NOT_FOUND,
    CAMERA_NOT_INITIALIZED
};

#endif // CAMERAHANDLER_H
