#include <iostream>
#include <string.h>
#include <cstdlib>
#include <stdio.h>
#include <vector>

#include "camera.h"

/**
 * @brief CameraHandler::CameraHandler
 */
QTLCamera::QTLCamera() {
    params = new GPhotoParams();
    memset(params, 0, sizeof(GPhotoParams));
    params->folder = strdup("/");
    cout << "Attempting to initialize camera..." << endl;
    params->context = gp_context_new();
    params->abilitiesList = NULL;
    params->widgetList = new vector<QTLWidget>();
}

void QTLCamera::setWorkingDirectory(const char *wd) {
    strcpy(workingDirectory, wd);
}

QTLError QTLCamera::detectCamera() {
    // Close connection to camera
    gp_camera_exit(params->camera, params->context);
    params->widgetList->clear();

    // Set up camera paramaters
    return initCamera();
}

QTLError QTLCamera::initCamera() {
    QTLError result;
    result.rc = GP_OK;
    result.errorText = "Camera initialized.";

    // Set aside memory for camera
    result.rc = gp_camera_new(&params->camera);
    if (result.rc != GP_OK) {
        result.errorText = gp_result_as_string(result.rc);
        return result;
    }

    // Initialise camera
    cout << "Detecting Camera." << endl << endl;
    result.rc = gp_camera_init(params->camera, params->context);

    if (result.rc != GP_OK) {
        result.errorText = gp_result_as_string(result.rc);

        if (result.rc == -105) {
            result.errorText = "Failed to initialise camera. Please check the camera " \
                    "is turned on, then re-initialise or restart the program to enable" \
                    " camera paramaters.";
        } else if (result.rc == -60 || result.rc == -53) {
            result.errorText = "Failed to initialise camera. Please check the camera " \
                    "is unmounted and that no other applications are using it, then " \
                    "re-initialise or restart the program to enable camera paramaters.";
        }
    } else {
        cout << "Camera detected" << endl;
        cout << "Detecting widgets"<< endl;
        CameraWidget *rootConfig;
        result.rc = gp_camera_get_config(params->camera, &rootConfig, params->context);
        if (result.rc == GP_OK) {
            char prefix[] = "";
            _getWidgets(params->widgetList, rootConfig, prefix);
            gp_widget_free(rootConfig);
        }
        cout << "Widgets detected" << endl;
    }

    return result;
}

/**
 * @brief QTLCamera::getWidgets
 * @return
 */
vector<QTLWidget> *QTLCamera::getWidgets() {
    return params->widgetList;
}

/**
 * @brief QTLCamera::_getWidgets
 * @param widgetList
 * @param widget
 * @param prefix
 */
void QTLCamera::_getWidgets(vector<QTLWidget> *widgetList, CameraWidget *widget,
                           char *prefix) {
    int rc, n;
    char *newprefix;
    const char *label, *name, *uselabel;
    CameraWidgetType type;
    CameraWidget *rootConfig, *child;
    QTLWidget qtlWidget;

    gp_widget_get_label(widget, &label);
    gp_widget_get_name(widget, &name);
    gp_widget_get_type(widget, &type);

    if (strlen(name)) {
        uselabel = name;
    } else {
        uselabel = label;
    }

    n = gp_widget_count_children(widget);

    newprefix = new char[strlen(prefix) + 1 + strlen(uselabel) + 1];

    if (!newprefix) {
        return;
    }

    sprintf(newprefix, "%s/%s", prefix, uselabel);

    //XXX Was this supposed to be a conditional for the whole section?
    // Assuming yes due to indenting.
    cout << "  Detected widget: " << uselabel << endl;
    if ((type != GP_WIDGET_WINDOW) && (type != GP_WIDGET_SECTION)) {
        rc = findWidgetByName(uselabel, &child, &rootConfig);
        rc = gp_widget_get_type(child, &type);
        rc = gp_widget_get_label(child, &label);

        if (type == GP_WIDGET_RADIO) {
            int count, i;
            char *current;

            rc = gp_widget_get_value(child, &current);
            if (rc == GP_OK) {
                count = gp_widget_count_choices(child);
                if (type == GP_WIDGET_MENU) {
                } else {

                    for (i=0; i<count; i++) {
                        const char *choice;
                        rc = gp_widget_get_choice(child, i, &choice);
                        qtlWidget.choices.push_back(choice);
                        cout << "    Detected choice: " << choice << endl;
                    }

                    qtlWidget.title = label;
                    qtlWidget.defaultChoice = current;
                    qtlWidget.choiceLabel = uselabel;

                    //XXX do we need this?
                    /*
                    wxString mystring(uselabel, wxConvUTF8);
                    choice_label_vector.push_back(mystring);
                    */

                    params->widgetList->push_back(qtlWidget);
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        CameraWidget *child;
        rc = gp_widget_get_child(widget, i, &child);
        if (rc != GP_OK) {
            continue;
        }
        _getWidgets(widgetList, child, newprefix);
    }
    free(newprefix);
}

/**
 * @brief CameraHandler::findWidgetByName
 * @param p
 * @param name
 * @param child
 * @param rootConfig
 * @return
 */
int QTLCamera::findWidgetByName(const char *name, CameraWidget **child,
                     CameraWidget **rootConfig) {
    int rc;
    rc = gp_camera_get_config(params->camera, rootConfig, params->context);
    if (rc != GP_OK) {
        return rc;
    }

    rc = gp_widget_get_child_by_name(*rootConfig, name, child);
    if (rc != GP_OK) {
        rc = gp_widget_get_child_by_label(*rootConfig, name, child);
    }
    if (rc != GP_OK) {
        char *part, *s, *newname;

        newname = strdup(name);
        if (!newname) {
            return GP_ERROR_NO_MEMORY;
        }

        *child = *rootConfig;
        part = newname;
        while (part[0] == '/') {
            part++;
        }
        while (1) {
            CameraWidget *tmp;

            s = strchr(part,'/');
            if (s) {
                *s = '\0';
            }
            rc = gp_widget_get_child_by_name(*child, part, &tmp);
            if (rc != GP_OK) {
                rc = gp_widget_get_child_by_label(*child, part, &tmp);
            }
            if (rc != GP_OK) {
                break;
            }
            *child = tmp;
            if (!s) {
                // end of path
                break;
            }
            part = s + 1;
            while (part[0] == '/') {
                part++;
            }
        }
        if (s) {
            // If we have stuff left over, we failed.
            //gp_context_error (p->context, _("%s not found in configuration tree."), newname);
            free(newname);
            gp_widget_free(*rootConfig);
            return GP_ERROR;
        }
        free (newname);
    }
    return GP_OK;
}

/**
 * @brief CameraHandler::setConfigAction
 * @param p
 * @param name
 * @param value
 * @return
 */
int QTLCamera::setConfigAction(const char *name, const char *value) {
    CameraWidget *rootConfig,*child;
    int rc;
    const char *label;
    CameraWidgetType type;

    rc = findWidgetByName(name, &child, &rootConfig);
    if (rc != GP_OK) {
        return rc;
    }

    rc = gp_widget_get_type (child, &type);
    if (rc != GP_OK) {
        gp_widget_free(rootConfig);
        return rc;
    }
    rc = gp_widget_get_label(child, &label);
    if (rc != GP_OK) {
        gp_widget_free(rootConfig);
        return rc;
    }

    switch (type) {
    case GP_WIDGET_TOGGLE: {
    }
    case GP_WIDGET_TEXT: {      /* char *       */
        rc = gp_widget_set_value(child, value);
        if (rc != GP_OK)
            //gp_context_error(p->context, _("Failed to set the value of text widget %s to %s."), name, value);
        break;
    }
    case GP_WIDGET_RANGE: { /* float        */
        float f, t, b, s;

        rc = gp_widget_get_range(child, &b, &t, &s);
        if (rc != GP_OK)
            break;
        if (!sscanf(value, "%f", &f)) {
            //gp_context_error(p->context, _("The passed value %s is not a floating point value."), value);
            rc = GP_ERROR_BAD_PARAMETERS;
            break;
        }
        if ((f < b) || (f > t)) {
            //gp_context_error(p->context, _("The passed value %f is not within the expected range %f - %f."), f, b, t);
            rc = GP_ERROR_BAD_PARAMETERS;
            break;
        }
        rc = gp_widget_set_value(child, &f);
        if (rc != GP_OK)
            //gp_context_error(p->context, _("Failed to set the value of range widget %s to %f."), name, f);
        break;
    }
    case GP_WIDGET_DATE:  {     /* int          */
        int t = -1;
        if (t == -1) {
            if (!sscanf(value, "%d", &t)) {
                //gp_context_error(p->context, _("The passed value %s is neither a valid time nor an integer."), value);
                rc = GP_ERROR_BAD_PARAMETERS;
                break;
            }
        }
        rc = gp_widget_set_value(child, &t);
        if (rc != GP_OK)
            //gp_context_error(p->context, _("Failed to set new time of date/time widget %s to %s."), name, value);
        break;
    }
    case GP_WIDGET_MENU:
    case GP_WIDGET_RADIO: { /* char *       */
        int cnt, i;

        cnt = gp_widget_count_choices(child);
        if (cnt < GP_OK) {
            rc = cnt;
            break;
        }
        rc = GP_ERROR_BAD_PARAMETERS;
        for (i=0; i<cnt; i++) {
            const char *choice;

            rc = gp_widget_get_choice(child, i, &choice);
            if (rc != GP_OK)
                continue;
            if (!strcmp(choice, value)) {
                rc = gp_widget_set_value(child, value);
                break;
            }
        }
        if (i != cnt)
            break;

        if (sscanf(value, "%d", &i)) {
            if ((i >= 0) && (i < cnt)) {
                const char *choice;

                rc = gp_widget_get_choice(child, i, &choice);
                if (rc == GP_OK)
                    rc = gp_widget_set_value(child, choice);
                break;
            }
        }
        //gp_context_error(p->context, _("Choice %s not found within list of choices."), value);
        break;
    }

    /* ignore: */
    case GP_WIDGET_WINDOW:
    case GP_WIDGET_SECTION:
    case GP_WIDGET_BUTTON:
        //gp_context_error(p->context, _("The %s widget is not configurable."), name);
        rc = GP_ERROR_BAD_PARAMETERS;
        break;
    }
    if (rc == GP_OK) {
        rc = gp_camera_set_config(params->camera, rootConfig, params->context);
        //if (ret != GP_OK)
            //gp_context_error(p->context, _("Failed to set new configuration value %s for configuration entry %s."), value, name);
    }
    gp_widget_free(rootConfig);
    return rc;
}

/**
 * @brief CameraHandler::captureImage
 */
void QTLCamera::captureImage(bool retrieveImages, bool deleteImages) {
    // Set camera paramaters if changed
    if (paramsChanged) {
        _updateParams();
    }

    // Now take picture
    _captureImage(retrieveImages);

    // Display the last image in the GUI
    /*
    if (result == GP_OK && tb1->IsChecked()) {

        wxString name(path.name, wxConvUTF8);
        wxBitmap bitmap;
        wxImage image = bitmap.ConvertToImage();

        if (!image.LoadFile(name, wxBITMAP_TYPE_ANY, -1)) {
            cout << "Couldn't load image." << endl;
            } else {
            image.Rescale(resize_x, resize_y);
                retrieved_image = wxBitmap(image);
            small_images.push_back(wxBitmap(image.Rescale(90,60)));
            if(small_images.size() > 48) {
                small_images.erase(small_images.begin());
            }
        }
    }*/
}

/**
 * @brief CameraHandler::_captureImage
 */
void QTLCamera::_captureImage(bool retrieveImage) {/*
    cout << "Capturing Image..." << endl;
    CameraFile * camera_file;
    CameraFilePath path;
    vector<string> camera_files;
    vector<string> camera_folders;

    int result = 0;
    long frames_per_int;
    if (!frames_per_interval->GetValue().ToLong(&frames_per_int)) {
        cout << "Error getting frames/interval" << endl;
    }

    for (int f = 0; f < frames_per_int; f++) {

        result = gp_camera_capture(params.camera, GP_CAPTURE_IMAGE, &path, params.context);

        if (result != GP_OK) {
            cout << "Could not capture image:\t" << result << endl;
            cout << "Check the camera and re-initialise." << endl;
            cout << "Could not capture an image. Please check the camera and re-initialise.";
            start_capture = false;
            gp_camera_exit(params.camera, params.context);
        } else {
            total_frames++;
            cout << "New file is in location " << path.folder << "/" << path.name << " on the camera." << endl;
            camera_files.push_back(path.name);
            camera_folders.push_back(path.folder);
        }
    }

    if (camera_files.size()) {

        for (unsigned int f = 0; f < camera_files.size(); f++) {

            string local_path = string(working_direcrory->GetPath().mb_str());
            local_path += "/";
            local_path += camera_files[f];
            gp_file_new(&camera_file);

            // Retrieve image if checkbox is ticked
            if (retrieveImages) {

                result = gp_camera_file_get(params.camera, camera_folders[f].c_str(), camera_files[f].c_str(), GP_FILE_TYPE_NORMAL, camera_file, params.context);

                if (result != GP_OK) {
                    cout << "Could not retieve image:\t" << result << endl;
                } else {
                    //result = gp_file_save(camera_file, path.name);
                    result = gp_file_save(camera_file, local_path.c_str());

                    if (result != GP_OK) {
                        cout << "Couldn't write file " << local_path << " - check path/permissions/disk space." << endl;
                        wxString status = (wxString::FromAscii("Couldn't write file "));
                        status.Append(wxString::FromAscii(local_path.c_str()));
                        status.Append(wxString::FromAscii(" - check path/permissions/disk space."));
                        this->SetStatusText(status);

                    } else {
                        if (start_capture) {
                            cout << "Written file " << total_frames << ": " << local_path << endl;
                            wxString status = (wxString::FromAscii("Written file "));
                            status.Append(wxString::Format(_T("%d"), total_frames));
                            status.Append(wxString::FromAscii(": "));
                            status.Append(wxString::FromAscii(local_path.c_str()));
                            this->SetStatusText(status);
                        } else {
                            cout << "Written file " << local_path << endl;
                            wxString status = (wxString::FromAscii("Written file "));
                            status.Append(wxString::FromAscii(local_path.c_str()));
                            this->SetStatusText(status);
                        }
                    }

                    // Delete image from camera if checkbox is ticked
                    if (deleteImages) {
                        _deleteImage();
                    }
                    cout << endl;
                }
            }
        }
    }
*/}

/**
 * @brief CameraHandler::_deleteImage
 */
QTLError QTLCamera::_deleteImage() {/*
    Error result;
    result.rc = gp_camera_file_delete(params.camera, camera_folders[f].c_str(), camera_files[f].c_str(), params.context);
    if (result.rc != GP_OK) {
        result.errorText = gp_result_as_string(result.rc);
        cout << "Problem deleting file from camera." << endl;
        cout << "Problem deleting file from camera." << result.errorText << endl;
    } else {
        cout << "File " << camera_files[f] << " deleted from camera." << endl;
        gp_file_unref(camera_file);
    }}
*/}

/**
 * @brief CameraHandler::_updateParams
 */
void QTLCamera::_updateParams() {/*
    cout << "Setting camera paramaters:" << endl;
    for (unsigned int v = 0; v < choice_label_vector.size(); v++) {
        cout << "Paramater:\t" << choice_label_vector[v].mb_str() << endl;
        cout << "Value:\t\t" << combobox_vector[v]->GetValue().mb_str() << endl;

        int set = set_config_action(&params, choice_label_vector[v].mb_str(),
                                    combobox_vector[v]->GetValue().mb_str());
        if (set == GP_OK) {
            //cout << "OK" <<  endl;
        } else {
            cout << "Error setting paramater:\t" << set << endl;
        }
    }
    paramsChanged = false;
    cout << endl;
*/}
