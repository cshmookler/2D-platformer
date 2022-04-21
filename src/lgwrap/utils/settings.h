#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace lgw {
    // store and manage data in a settings file
    class Settings {
    private:
        // settings file directory
        const char* dir;
        // size of variable arrays
        static const int numStringValues = 4;
        static const int numIntValues = 3;
        static const int numFloatValues = 5;
        // where the loaded variables are stored during program execution
        std::string stringValues[numStringValues] = { "" };
        int intValues[numIntValues] = { 0 };
        float floatValues[numFloatValues] = { 0 };
        // list of variable names (in cooresponding order)
        const std::string stringNames[numStringValues] = {
            "window_title",
            "shader_dir",
            "asset_dir",
            "font_dir"
        };
        const std::string intNames[numIntValues] = {
            "window_aspect_ratio_x",
            "window_aspect_ratio_y",
            "window_scale"
        };
        const std::string floatNames[numFloatValues] = {
            "fps_cap",
            "inv_scale_factor",
            "physics_error_margin",
            "camera_position_x",
            "camera_position_y"
        };
    public:
        /*
        // strings
        window_title = stringValues[0]
        shader_dir = stringValues[1]
        asset_dir = stringValues[2]
        font_dir = stringValues[3]
        // integers
        window_aspect_ratio_x = intValues[0]
        window_aspect_ratio_y = intValues[1]
        window_scale = intValues[2]
        // floats
        fps_cap = floatValues[0]
        inv_scale_factor = intValues[1]
        physics_error_margin = floatValues[2]
        camera_position_x = floatValues[3]
        camera_position_y = floatValues[4]
        */
        // straight from the settings file
        std::string& window_title = stringValues[0];
        std::string& shader_dir = stringValues[1];
        std::string& asset_dir = stringValues[2];
        std::string& font_dir = stringValues[3];
        int& window_aspect_ratio_x = intValues[0];
        int& window_aspect_ratio_y = intValues[1];
        int& window_scale = intValues[2];
        float& fps_cap = floatValues[0];
        float& inv_scale_factor = floatValues[1];
        float& physics_error_margin = floatValues[2];
        float& camera_position_x = floatValues[3];
        float& camera_position_y = floatValues[4];
        // derived from other settings
        int window_width = 0;
        int window_height = 0;
        float window_aspect_ratio_dec = 0;
        float spf_cap = 0;
        float window_virtual_width = 0;
        float window_virtual_height = 0;
        // constructor
        Settings(const char* fileDir);
        // reload derived settings
        void reload_derived(void);
        // load settings from a specified directory
        int load(void);
        // save current settings to a specified directory
        bool save(void);
    };
}