# 2D-platformer
Written in C++ using OpenGL, GLAD, and GLFW. Developed in Visual Studio 2022.
# Features
 - [X] Object creation and rendering
 - [X] Lifelike object motion
 - [X] Basic object collision
 - [X] ~~FPS displayed in the window title~~
       *FPS displayed in a debug menu* (recently added)
 - [X] Settings file
 - [X] Option to change camera position relative to the virtual world
 - [ ] Sweep-based continuos collision detection (CCD)
 - [ ] Standardize object management to minimize redundency in code
 - [ ] Realistic friction (or any friction at all)
 - [ ] Optimize object collision (trim redundent side lengths from collision map)
 - [ ] Zoom in and out with the mouse wheel
 - [ ] Playable levels
# Build this project from source
1. Clone this repository and open its solution (2D platformer.sln) in Microsoft Visual Studio.
2. Build the project (Ctrl+Shift+B) using your preferred configuration (Recommended: Release \- x64).
3. An executable will be generated at \*project directory\*/bin/\*configuration\*/2D platformer.exe. To run the executable outside of Visual Studio, copy the data folder and all DLLs in the project directory to the same location as the executable. Without these the program will crash on startup.
4. Please report any errors you encounter.
# Release v0.1.0
Coming soon...
