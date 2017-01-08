#ifndef CONSTANTS
#define CONSTANTS

#include <QString>
#include <QObject>

const int SCENE_TIMER_INTERVAL = 16;  //msec
const int SCENE_DEFAULT_WIDTH = 800;
const int SCENE_DEFAULT_HEIGHT = 600;
const float MOUSE_SENSITIVITY = 200.0;
const int SLIDER_SENSITIVITY = 10;
const int VERTICES_IN_POLYGON = 3;
const int CONTROL_PANEL_WIDTH = 400;
const int ELEMENT_LIST_WIDTH = 200;
const int EDIT_WINDOW_WIDTH = 500;
const int EDIT_WINDOW_HEIGHT = 520;
const double MAX_LINK_LENGTH = 50.0;
const int TIP_COORD_PRECISION = 3;
const int CHOOSE_PATH_BUTTON_WIDTH = 30;
const float MIN_SCENE_ZOOM = 0.05;
const float MAX_SCENE_ZOOM = 2.0;
const float SCROLL_SENSITIVITY = 0.1;
const int IK_ANIMATION_STEPS = 120;
const int TOKEN_MATERIAL_ID = 6;   //silver token

// temporary range for tip coordinates in IK module
const int IK_RANGE = 30;

const QString APPLICATION_NAME = "Visualisation-of-the-Manipulator";
const QString COMPANY_NAME = "Wrocław-University-of-Technology";
const QString DEFAULT_WORKSPACE_PATH = "./manip";

#endif // CONSTANTS

