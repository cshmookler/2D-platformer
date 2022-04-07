#pragma once

#include <iostream> // for debug
#include <fstream> // for reading/writing files
#include <sstream> // for handling file contents
#include <string> // for when const char* won't do the job

class Settings {
private:
    // settings file directory
    const char* dir = "data/settings.txt";
    // size of variable arrays
    static const int numStringValues = 1;
    static const int numIntValues = 3;
    static const int numFloatValues = 5;
    // where the loaded variables are stored during program execution
    std::string stringValues[numStringValues] = { "" };
    int intValues[numIntValues] = { 0 };
    float floatValues[numFloatValues] = { 0 };
    // list of variable names (in cooresponding order)
    const std::string stringNames[numStringValues] = {
        "window_title"
    };
    const std::string intNames[numIntValues] = {
        "window_aspect_ratio_x",
        "window_aspect_ratio_y",
        "window_scale"
    };
    const std::string floatNames[numFloatValues] = {
        "inv_scale_factor",
        "fps_cap",
        "physics_error_margin",
        "camera_position_x",
        "camera_position_y"
    };
public:
    /*
    // strings
    window_title = stringValues[0]
    // integers
    window_aspect_ratio_x = intValues[0]
    window_aspect_ratio_y = intValues[1]
    window_scale = intValues[2]
    // floats
    inv_scale_factor = intValues[0]
    fps_cap = floatValues[1]
    physics_error_margin = floatValues[2]
    camera_position_x = floatValues[3]
    camera_position_y = floatValues[4]
    */
    // straight from the settings file
    std::string& window_title = stringValues[0];
    int& window_aspect_ratio_x = intValues[0];
    int& window_aspect_ratio_y = intValues[1];
    int& window_scale = intValues[2];
    float& inv_scale_factor = floatValues[0];
    float& fps_cap = floatValues[1];
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
    Settings(const char* fileDir) : dir(fileDir) {}
    // load settings from a specified directory
    int load()
    {
        std::ifstream settingsFile(dir);
        if (!settingsFile.is_open())
        {
            return -1; // settings file was not found
        }
        bool stringNamesFound[numStringValues] = { false };
        bool intNamesFound[numIntValues] = { false };
        bool floatNamesFound[numFloatValues] = { false };
        bool found = false;
        std::string line;
        while (std::getline(settingsFile, line))
        {
            for (int i = 0; i < numStringValues; i++)
            {
                if (!stringNamesFound[i] && !line.find(stringNames[i]))
                {
                    std::istringstream value(line.substr(line.find("=") + 2));
                    stringValues[i] = value.str();
                    stringNamesFound[i] = true;
                    found = true;
                    break;
                }
            }
            if (found)
            {
                found = false;
                continue;
            }
            for (int i = 0; i < numIntValues; i++)
            {
                if (!intNamesFound[i] && !line.find(intNames[i]))
                {
                    std::istringstream value(line.substr(line.find("=") + 2));
                    intValues[i] = std::stoi(value.str());
                    intNamesFound[i] = true;
                    found = true;
                    break;
                }
            }
            if (found)
            {
                found = false;
                continue;
            }
            for (int i = 0; i < numFloatValues; i++)
            {
                if (!floatNamesFound[i] && !line.find(floatNames[i]))
                {
                    std::istringstream value(line.substr(line.find("=") + 2));
                    floatValues[i] = std::stof(value.str());
                    floatNamesFound[i] = true;
                    break;
                }
            }
        }
        // check for missing settings
        for (int i = 0; i < numStringValues; i++)
        {
            if (stringNamesFound[i] == false)
            {
                return -i - 2; // missing string in settings file
            }
        }
        for (int i = 0; i < numIntValues; i++)
        {
            if (intNamesFound[i] == false)
            {
                return -i - 2 - numStringValues; // missing integer in settings file
            }
        }
        for (int i = 0; i < numFloatValues; i++)
        {
            if (floatNamesFound[i] == false)
            {
                return -i - 2 - numStringValues - numIntValues; // missing float in settings file
            }
        }
        // derive other settings
        window_width = window_aspect_ratio_x * window_scale;
        window_height = window_aspect_ratio_y * window_scale;
        window_aspect_ratio_dec = (float)window_aspect_ratio_x / (float)window_aspect_ratio_y;
        spf_cap = 1.0f / fps_cap;
        window_virtual_width = window_aspect_ratio_dec * inv_scale_factor;
        window_virtual_height = inv_scale_factor;
        settingsFile.close();
        /*
        // debug
        std::cout << "window_title: " << window_title << std::cout;
        std::cout << "window_aspect_ratio_x: " << window_aspect_ratio_x << std::cout;
        std::cout << "window_aspect_ratio_y: " << window_aspect_ratio_y << std::cout;
        std::cout << "window_scale: " << window_scale << std::cout;
        std::cout << "fps_cap: " << fps_cap << std::cout;
        std::cout << "inv_scale_factor: " << inv_scale_factor << std::cout;
        std::cout << "physics_error_margin: " << physics_error_margin << std::cout;
        std::cout << "camera_position_x: " << camera_position_x << std::cout;
        std::cout << "camera_position_y: " << camera_position_y << std::cout;
        std::cout << "window_width: " << window_width << std::cout;
        std::cout << "window_height: " << window_height << std::cout;
        std::cout << "window_aspect_ratio_dec: " << window_aspect_ratio_dec << std::cout;
        std::cout << "spf_cap: " << spf_cap << std::cout;
        std::cout << "window_virtual_width: " << window_virtual_width << std::cout;
        std::cout << "window_virtual_height: " << window_virtual_height << std::cout;
        */
        return 1;
    }
    // save current settings to a specified directory
    bool save()
    {
        std::ofstream settingsFile(dir);
        if (!settingsFile.is_open())
        {
            return 0;
        }
        for (int i = 0; i < numStringValues; i++)
        {
            settingsFile << stringNames[i] << " = " << stringValues[i] << std::endl;
        }
        for (int i = 0; i < numIntValues; i++)
        {
            settingsFile << intNames[i] << " = " << intValues[i] << std::endl;
        }
        for (int i = 0; i < numFloatValues; i++)
        {
            settingsFile << floatNames[i] << " = " << floatValues[i] << std::endl;
        }
        settingsFile.close();
        return 1;
    }
};