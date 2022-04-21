#include "settings.h"

// Settings: constructor
lgw::Settings::Settings(const char* fileDir) : dir(fileDir) {}
// Settings: reload derived settings
void lgw::Settings::reload_derived(void)
{
    window_width = window_aspect_ratio_x * window_scale;
    window_height = window_aspect_ratio_y * window_scale;
    window_aspect_ratio_dec = (float)window_aspect_ratio_x / (float)window_aspect_ratio_y;
    spf_cap = 1.0f / fps_cap;
    window_virtual_width = window_aspect_ratio_dec * inv_scale_factor;
    window_virtual_height = inv_scale_factor;
}
// Settings: load settings from a specified directory
int lgw::Settings::load(void)
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
    reload_derived();
    settingsFile.close();

    // debug
    /*
    std::cout << "window_title: " << window_title << std::endl;
    std::cout << "shader_dir: " << shader_dir << std::endl;
    std::cout << "asset_dir: " << asset_dir << std::endl;
    std::cout << "font_dir: " << font_dir << std::endl;
    std::cout << "window_aspect_ratio_x: " << window_aspect_ratio_x << std::endl;
    std::cout << "window_aspect_ratio_y: " << window_aspect_ratio_y << std::endl;
    std::cout << "window_scale: " << window_scale << std::endl;
    std::cout << "fps_cap: " << fps_cap << std::endl;
    std::cout << "inv_scale_factor: " << inv_scale_factor << std::endl;
    std::cout << "physics_error_margin: " << physics_error_margin << std::endl;
    std::cout << "camera_position_x: " << camera_position_x << std::endl;
    std::cout << "camera_position_y: " << camera_position_y << std::endl;
    std::cout << "window_width: " << window_width << std::endl;
    std::cout << "window_height: " << window_height << std::endl;
    std::cout << "window_aspect_ratio_dec: " << window_aspect_ratio_dec << std::endl;
    std::cout << "spf_cap: " << spf_cap << std::endl;
    std::cout << "window_virtual_width: " << window_virtual_width << std::endl;
    std::cout << "window_virtual_height: " << window_virtual_height << std::endl;
    */
    return 1;
}
// Settings: save current settings to a specified directory
bool lgw::Settings::save(void)
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