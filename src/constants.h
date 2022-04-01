#pragma once

// global constants
namespace CONST {
    const int WINDOW_ASPECT_RATIO[2] = { 16, 9 };
    const int WINDOW_DEFAULT_SCALE = 100;
    const int WINDOW_DEFAULT_WIDTH = WINDOW_ASPECT_RATIO[0] * WINDOW_DEFAULT_SCALE;
    const int WINDOW_DEFAULT_HEIGHT = WINDOW_ASPECT_RATIO[1] * WINDOW_DEFAULT_SCALE;
    const char* WINDOW_DEFAULT_TITLE = "2D Physics Game";
    const float WINDOW_ASPECT_RATIO_DEC = (float)(WINDOW_ASPECT_RATIO[0] * WINDOW_DEFAULT_SCALE) / (float)(WINDOW_ASPECT_RATIO[1] * WINDOW_DEFAULT_SCALE);
    const float FPS_CAP = 60.0f; // frames per second ceiling, >1000 is essentially unlimited but there's no practical purpose for >60 anyway
    const float SPF_CAP = 1.0f / FPS_CAP; // seconds per frame ceiling
    const int INV_SCALE_FACTOR = 10; // relative to the window height
    const float WINDOW_VIRTUAL_WIDTH = WINDOW_ASPECT_RATIO_DEC * (float)INV_SCALE_FACTOR; // window width in virtual units
    const float WINDOW_VIRTUAL_HEIGHT = (float)CONST::INV_SCALE_FACTOR; // window height in virtual units
    const double PHYSICS_ERROR_MARGIN = 0.0001;
}