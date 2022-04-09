#pragma once

#include <iostream> // for debug
#include <fstream> // for reading/writing files
#include <sstream> // for handling file contents
#include <string> // for when const char* won't do the job

namespace files
{
    // store and manage data in a settings file
    class Settings {
    private:
        // settings file directory
        const char* dir;
        // size of variable arrays
        static const int numStringValues = 3;
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
            "asset_dir"
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
        float t = 0.0f;
        // constructor
        Settings(const char* fileDir) : dir(fileDir) {}
        // reload derived settings
        void reload_derived()
        {
            window_width = window_aspect_ratio_x * window_scale;
            window_height = window_aspect_ratio_y * window_scale;
            window_aspect_ratio_dec = (float)window_aspect_ratio_x / (float)window_aspect_ratio_y;
            spf_cap = 1.0f / fps_cap;
            window_virtual_width = window_aspect_ratio_dec * inv_scale_factor;
            window_virtual_height = inv_scale_factor;
        }
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
            reload_derived();
            settingsFile.close();

            // debug
            /*
            std::cout << "window_title: " << window_title << std::endl;
            std::cout << "shader_dir: " << shader_dir << std::endl;
            std::cout << "asset_dir: " << asset_dir << std::endl;
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

    // load the contents of a file
    std::string loadFile(std::string dir)
    {
        // open file
        std::ifstream file(dir);
        if (!file.is_open())
            return std::string(); // file not found
        // copy file contents to std::stringstream
        std::stringstream buffer;
        buffer << file.rdbuf();
        // close file
        file.close();
        return buffer.str(); // return file contents as a string
    }
}