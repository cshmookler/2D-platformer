#pragma once

// global constants
namespace CONST {
    const int WINDOW_DEFAULT_WIDTH = 1600;
    const int WINDOW_DEFAULT_HEIGHT = 900;
    const char* WINDOW_DEFAULT_TITLE = "2D Physics Game";
    const float WINDOW_ASPECT_RATIO = (float)WINDOW_DEFAULT_WIDTH / (float)WINDOW_DEFAULT_HEIGHT;
    const float FPS_CAP = 60.0f; // frames per second ceiling, >1000 is essentially unlimited but there's no practical purpose for >60 anyway
    const float SPF_CAP = 1.0f / FPS_CAP; // seconds per frame ceiling
    const int INV_SCALE_FACTOR = 10; // relative to the window height
    const float WINDOW_WIDTH = CONST::WINDOW_ASPECT_RATIO * (float)CONST::INV_SCALE_FACTOR; // window width in virtual units
    const float WINDOW_HEIGHT = (float)CONST::INV_SCALE_FACTOR; // window height in virtual units
    const float PHYSICS_ERROR_MARGIN = 0.0001;
}