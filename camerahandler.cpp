#include <iostream>
#include <string.h>
#include <cstdlib>
#include <stdio.h>

#include "camerahandler.h"

using namespace std;

void GPhotoParamsInit(GPhotoParams *p) {
    if (!p) {
        return;
    }

    memset(p, 0, sizeof(GPhotoParams));
    p->folder = strdup("/");
    gp_camera_new(&p->camera);
    p->context = gp_context_new();
    p->abilitiesList = NULL;
}

int findWidgetByName(GPhotoParams *p, const char *name, CameraWidget **child,
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
                rc = gp_widget_get_child_by_label (*child, part, &tmp);
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

int setConfigAction(GPhotoParams *p, const char *name, const char *value) {
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
        rc = gp_widget_set_value (child, value);
        if (rc != GP_OK)
            //gp_context_error (p->context, _("Failed to set the value of text widget %s to %s."), name, value);
        break;
    }
    case GP_WIDGET_RANGE: { /* float        */
        float   f,t,b,s;

        rc = gp_widget_get_range (child, &b, &t, &s);
        if (rc != GP_OK)
            break;
        if (!sscanf (value, "%f", &f)) {
            //gp_context_error (p->context, _("The passed value %s is not a floating point value."), value);
            rc = GP_ERROR_BAD_PARAMETERS;
            break;
        }
        if ((f < b) || (f > t)) {
            //gp_context_error (p->context, _("The passed value %f is not within the expected range %f - %f."), f, b, t);
            rc = GP_ERROR_BAD_PARAMETERS;
            break;
        }
        rc = gp_widget_set_value (child, &f);
        if (rc != GP_OK)
            //gp_context_error (p->context, _("Failed to set the value of range widget %s to %f."), name, f);
        break;
    }
    case GP_WIDGET_DATE:  {     /* int          */
        int t = -1;
        if (t == -1) {
            if (!sscanf (value, "%d", &t)) {
                //gp_context_error (p->context, _("The passed value %s is neither a valid time nor an integer."), value);
                rc = GP_ERROR_BAD_PARAMETERS;
                break;
            }
        }
        rc = gp_widget_set_value (child, &t);
        if (rc != GP_OK)
            //gp_context_error (p->context, _("Failed to set new time of date/time widget %s to %s."), name, value);
        break;
    }
    case GP_WIDGET_MENU:
    case GP_WIDGET_RADIO: { /* char *       */
        int cnt, i;

        cnt = gp_widget_count_choices (child);
        if (cnt < GP_OK) {
            rc = cnt;
            break;
        }
        rc = GP_ERROR_BAD_PARAMETERS;
        for ( i=0; i<cnt; i++) {
            const char *choice;

            rc = gp_widget_get_choice (child, i, &choice);
            if (rc != GP_OK)
                continue;
            if (!strcmp (choice, value)) {
                rc = gp_widget_set_value (child, value);
                break;
            }
        }
        if (i != cnt)
            break;

        if (sscanf (value, "%d", &i)) {
            if ((i>= 0) && (i < cnt)) {
                const char *choice;

                rc = gp_widget_get_choice (child, i, &choice);
                if (rc == GP_OK)
                    rc = gp_widget_set_value (child, choice);
                break;
            }
        }
        //gp_context_error (p->context, _("Choice %s not found within list of choices."), value);
        break;
    }

    /* ignore: */
    case GP_WIDGET_WINDOW:
    case GP_WIDGET_SECTION:
    case GP_WIDGET_BUTTON:
        //gp_context_error (p->context, _("The %s widget is not configurable."), name);
        rc = GP_ERROR_BAD_PARAMETERS;
        break;
    }
    if (rc == GP_OK) {
        rc = gp_camera_set_config (p->camera, rootConfig, p->context);
        //if (ret != GP_OK)
            //gp_context_error (p->context, _("Failed to set new configuration value %s for configuration entry %s."), value, name);
    }
    gp_widget_free (rootConfig);
    return (rc);
}

CameraHandler::CameraHandler() {
}

void CameraHandler::captureImage() {
}
