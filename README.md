# 2D-platformer
Written in C++ using OpenGL, GLAD, and GLFW. Developed in Visual Studio 2022.
## Features:
### Current:
 - Object creation and rendering
 - Lifelike object motion
 - Basic object collision
 - FPS displayed in the window title
### Coming soon:
 - Sweep-based continuos collision detection (CCD)
 - Object collision optimization (trim redundent side lengths from collision map)
 - Playable levels
 - Display FPS in the debug menu
 - Settings file
 - Option to change camera position relative to the virtual world
## Build from source:
1. Clone this repository and open its solution (2D platformer.sln) in Microsoft Visual Studio.
2. Build the project using your preferred configuration (Recommended: Release \- x64).
3. The executable will be at \*project directory\*/bin/\*configuration\*/2D platformer.exe. To run the executable outside of Visual Studio, copy all DLLs in the project directory to the same location as the executable. Without these the program will crash on startup.
4. Run the executable.
Please report any errors you encounter!
