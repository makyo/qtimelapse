#include <iostream>
#include <string.h>
#include <cstdlib>
#include <stdio.h>

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
}

void QTLCamera::setWorkingDirectory(const char *wd) {
    strcpy(workingDirectory, wd);
}

Error QTLCamera::detectCamera() {
    // Close connection to camera
    gp_camera_exit(params->camera, params->context);

    // Set up camera paramaters
    return initCamera();
}

Error QTLCamera::initCamera() {
    Error result;
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
    }
    return result;
}

/**
 * @brief CameraHandler::findWidgetByName
 * @param p
 * @param name
 * @param child
 * @param rootConfig
 * @return
 */
int QTLCamera::findWidgetByName(GPhotoParams *p, const char *name, CameraWidget **child,
                     CameraWidget **rootConfig) {
    int rc;
    rc = gp_camera_get_config(p->camera, rootConfig, p->context);
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
int QTLCamera::setConfigAction(GPhotoParams *p, const char *name, const char *value) {
    CameraWidget *rootConfig,*child;
    int rc;
    const char *label;
    CameraWidgetType type;

    rc = findWidgetByName(p, name, &child, &rootConfig);
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
        rc = gp_camera_set_config(p->camera, rootConfig, p->context);
        //if (ret != GP_OK)
            //gp_context_error(p->context, _("Failed to set new configuration value %s for configuration entry %s."), value, name);
    }
    gp_widget_free(rootConfig);
    return rc;
}

/**
 * @brief CameraHandler::captureImage
 */
void QTLCamera::captureImage() { /*
    // Set camera paramaters if changed
    if (params_changed) {
        cout << "Setting camera paramaters:" << endl;
        for (unsigned int v = 0; v < choice_label_vector.size(); v++) {
            cout << "Paramater:\t" << choice_label_vector[v].mb_str() << endl;
            cout << "Value:\t\t" << combobox_vector[v]->GetValue().mb_str() << endl;

            int set = set_config_action(&gp_params, choice_label_vector[v].mb_str(),
                                        combobox_vector[v]->GetValue().mb_str());
            if (set == GP_OK) {
                //cout << "OK" <<  endl;
            } else {
                cout << "Error setting paramater:\t" << set << endl;
            }
        }
        params_changed = false;
        cout << endl;
    }

    // Now take picture
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

        result = gp_camera_capture(gp_params.camera, GP_CAPTURE_IMAGE, &path, gp_params.context);

        if (result != GP_OK) {
            cout << "Could not capture image:\t" << result << endl;
            cout << "Check the camera and re-initialise." << endl;
            wxLogError(wxT("Could not capture an image. Please check the camera and re-initialise."));
            start_capture = false;
            gp_camera_exit(gp_params.camera, gp_params.context);
            button2->Disable();
            button3->Disable();
            button4->Disable();
        } else {
            total_frames++;
            cout << "New file is in location " << path.folder << "/" << path.name << " on the camera." << endl;
            wxString status = (wxString::FromAscii("New file is in location "));
            status.Append(wxString::FromAscii(path.folder));
            status.Append(wxString::FromAscii("/"));
            status.Append(wxString::FromAscii(path.name));
            status.Append(wxString::FromAscii(" on the camera."));
            this->SetStatusText(status);

            camera_files.push_back(path.name);
            camera_folders.push_back(path.folder);
        }
    }

    // Now get and delete images
    if (camera_files.size()) {

        for (unsigned int f = 0; f < camera_files.size(); f++) {

            string local_path = string(working_direcrory->GetPath().mb_str());
            local_path += "/";
            local_path += camera_files[f];
            gp_file_new(&camera_file);

            // Retrieve image if checkbox is ticked
            if (tb1->IsChecked()) {

                result = gp_camera_file_get(gp_params.camera, camera_folders[f].c_str(), camera_files[f].c_str(), GP_FILE_TYPE_NORMAL, camera_file, gp_params.context);

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
                    if (tb2->IsChecked()) {

                        result = gp_camera_file_delete(gp_params.camera, camera_folders[f].c_str(), camera_files[f].c_str(), gp_params.context);
                        if (result != GP_OK) {
                            cout << "Problem deleting file from camera." << endl;
                            wxString status = (wxString::FromAscii("Problem deleting file from camera."));
                            this->SetStatusText(status);
                        } else {
                            cout << "File " << camera_files[f] << " deleted from camera." << endl;
                            gp_file_unref(camera_file);
                        }
                    }
                    cout << endl;
                }
            }
        }
    }

    // Display the last image in the GUI
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
void QTLCamera::_captureImage() {}

/**
 * @brief CameraHandler::_deleteImage
 */
void QTLCamera::_deleteImage() {}

/**
 * @brief CameraHandler::_updateParams
 */
void QTLCamera::_updateParams() {}
