#pragma once

#include <iostream> // for debug
#include <fstream> // for reading/writing files
#include <sstream> // for handling file contents
#include <string> // for when const char* won't work

class Settings {
public:

    // straight from the settings file
    std::string window_title;
    int window_aspect_ratio_x;
    int window_aspect_ratio_y;
    int window_scale;
    float fps_cap;
    int inv_scale_factor;
    float physics_error_margin;
    // derived from other settings
    int window_width;
    int window_height;
    float window_aspect_ratio_dec;
    float spf_cap;
    float window_virtual_width;
    float window_virtual_height;

    // constructor
    Settings(const char* fileDir) : dir(fileDir) {}
    // load settings from a specified directory
    bool load()
    {
        std::ifstream settingsFile(dir);
        if (!settingsFile.is_open())
        {
            return -1; // settings file was not found
        }
        bool namesFound[7] = {false};
        std::string line;
        while (std::getline(settingsFile, line))
        {
            for (int i = 0; i < namesListLength; i++)
            {
                if (!namesFound[i] && !line.find(namesList[i]))
                {
                    namesFound[i] = true;
                    std::istringstream value(line.substr(line.find("=") + 2));
                    switch (i)
                    {
                    case 0:
                        window_title = value.str();
                        break;
                    case 1:
                        window_aspect_ratio_x = std::stoi(value.str());
                        break;
                    case 2:
                        window_aspect_ratio_y = std::stoi(value.str());
                        break;
                    case 3:
                        window_scale = std::stoi(value.str());
                        break;
                    case 4:
                        fps_cap = std::stof(value.str());
                        break;
                    case 5:
                        inv_scale_factor = std::stoi(value.str());
                        break;
                    case 6:
                        physics_error_margin = std::stof(value.str());
                        break;
                    }
                    break;
                }
            }
        }
        for (int i = 0; i < namesListLength; i++)
        {
            if (namesFound[i] == false)
            {
                return -i - 2; // missing variable in settings file
            }
        }
        window_width = window_aspect_ratio_x * window_scale;
        window_height = window_aspect_ratio_y * window_scale;
        window_aspect_ratio_dec = (float)window_aspect_ratio_x / (float)window_aspect_ratio_y;
        spf_cap = 1.0f / fps_cap;
        window_virtual_width = window_aspect_ratio_dec * (float)inv_scale_factor;
        window_virtual_height = (float)inv_scale_factor;
        settingsFile.close();
        /*
        std::cout << "window_title: " << window_title << std::cout;
        std::cout << "window_aspect_ratio_x: " << window_aspect_ratio_x << std::cout;
        std::cout << "window_aspect_ratio_y: " << window_aspect_ratio_y << std::cout;
        std::cout << "window_scale: " << window_scale << std::cout;
        std::cout << "fps_cap: " << fps_cap << std::cout;
        std::cout << "inv_scale_factor: " << inv_scale_factor << std::cout;
        std::cout << "physics_error_margin: " << physics_error_margin << std::cout;
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
        for (int i = 0; i < namesListLength; i++)
        {
            std::stringstream line;
            line << namesList[i] << " = ";
            switch (i)
            {
            case 0:
                line << window_title;
                break;
            case 1:
                line << window_aspect_ratio_x;
                break;
            case 2:
                line << window_aspect_ratio_y;
                break;
            case 3:
                line << window_scale;
                break;
            case 4:
                line << fps_cap;
                break;
            case 5:
                line << inv_scale_factor;
                break;
            case 6:
                line << physics_error_margin;
                break;
            }
            line << std::endl;
            settingsFile.write(line.str().c_str(), namesListLength);
        }
        settingsFile.close();
        return 1;
    }

private:
    // settings file directory
    std::string dir;
    // list of variable names (in cooresponding order)
    const int namesListLength = 7;
    const std::string namesList[7] = { "window_title",
                                       "window_aspect_ratio_x",
                                       "window_aspect_ratio_y",
                                       "window_scale",
                                       "fps_cap",
                                       "inv_scale_factor",
                                       "physics_error_margin" };
};