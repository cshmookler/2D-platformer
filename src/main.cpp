//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // starts the program without the console

// standard libraries
#include <iostream> // for debug
#include <fstream> // for reading/writing files
#include <sstream> // for changing the window title
#include <string> // for when const char* won't work
#include <cmath> // for advanced math functions

// public (external) libraries
#include <glad/glad.h> // loader for OpenGL
#include <GLFW/glfw3.h> // API for window creation
#include <ft2build.h> // render fonts
#include FT_FREETYPE_H

// private libraries
#include "const.h"
#include "tools.h"
#include "files.h"
#include "ocp.h"

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// program entry point
int main(int argc, char** argv)
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // initialize settings
    files::Settings settings(CONST::SETTINGS_DIR);
    int error = settings.load();
    if (error == -1)
    {
        std::cout << "Settings file not found" << std::endl;
        glfwTerminate();
        return -1;
    }
    else if (error < -1)
    {
        std::cout << "Setting #" << -error - 2 << " not found in settings file" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // glfw: window creation
    GLFWwindow* window = glfwCreateWindow(settings.window_width, settings.window_height, settings.window_title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowAspectRatio(window, 16, 9);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // load vertex shader
    std::string vertexShaderBuffer = files::loadFile(settings.shader_dir + "basic.vertex.glsl");
    const char* vertexShaderSource = vertexShaderBuffer.c_str();
    if (vertexShaderSource == std::string())
    {
        std::cout << "Failed to load vertex shader" << std::endl; // vertex shader file either wasn't found or is empty
        return -1;
    }
    // load fragment shader
    std::string fragmentShaderBuffer = files::loadFile(settings.shader_dir + "basic.fragment.glsl");
    const char* fragmentShaderSource = fragmentShaderBuffer.c_str();
    if (fragmentShaderSource == std::string())
    {
        std::cout << "Failed to load fragment shader" << std::endl; // fragment shader file either wasn't found or is empty
        return -1;
    }

    // compile the vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char* infoLog = new char[1024];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        std::cout << "Vertex shader compilation failed" << std::endl << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }
    // compile the fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        std::cout << "Fragment shader compilation failed" << std::endl << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }
    // create the shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
        std::cout << "Shader program linking failed" << std::endl << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }
    // deallocate vertex shader and fragment shader
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // deallocate 'infolog'
    delete[] infoLog;

    // gl: set up vertex data, buffers, and configure vertex attributes
    unsigned int rectIndices[6] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // use shader program
    glUseProgram(shaderProgram);

    // load fonts
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, "C:/Windows/Fonts/Arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    ocp::Point intersect;
    
    // player object
    ocp::Point playerInitPos = { (settings.window_virtual_width / 2.0f) - 0.5f, 0.0f };
    ocp::Object player(settings.window_aspect_ratio_dec, settings.inv_scale_factor, ocp::Point(playerInitPos.x, playerInitPos.y), ocp::Point(playerInitPos.x + 1.0f, playerInitPos.y + 1.0f));
    // additional variables
    float playerColor[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    ocp::Vector playerMoved = { 0.0f, 0.0f };
    float gravity = -9.8f;
    bool canJump = false;

    // green box
    ocp::Barrier2D box(settings.window_aspect_ratio_dec, settings.inv_scale_factor, ocp::Point(0.0f, 0.0f), ocp::Point(2.0f, 2.0f));
    box.setVertices(settings.camera_position_x, settings.camera_position_y);
    // additional variables
    float objectColor[4] = { 0.0f, 1.0f, 0.5f, 1.0f };

    // barrier
    float barrierColor[4] = { 0.75f, 0.0f, 0.0f, 1.0f };
    ocp::Barrier1D lowerBound(settings.window_aspect_ratio_dec, settings.inv_scale_factor, ocp::Point(0.0f, 0.0f), ocp::Point(settings.window_virtual_width, 0.0f));
    ocp::Barrier1D upperBound(settings.window_aspect_ratio_dec, settings.inv_scale_factor, ocp::Point(0.0f, settings.window_virtual_height), ocp::Point(settings.window_virtual_width, settings.window_virtual_height));
    ocp::Barrier1D leftBound(settings.window_aspect_ratio_dec, settings.inv_scale_factor, ocp::Point(0.0f, 0.0f), ocp::Point(0.0f, settings.window_virtual_height));
    ocp::Barrier1D rightBound(settings.window_aspect_ratio_dec, settings.inv_scale_factor, ocp::Point(settings.window_virtual_width, 0.0f), ocp::Point(settings.window_virtual_width, settings.window_virtual_height));
    box.setVertices(settings.camera_position_x, settings.camera_position_y);

    float init_virtual_width = settings.window_virtual_width;
    float init_virtual_height = settings.window_virtual_height;

    // start timers:
    // timer that is used to update the fps counter once every second
    tools::Stopwatch titleUpdateStopwatch;
    // timer that is reset on every frame
    tools::Stopwatch fpsStopwatch;
    // counter to count the number of frames
    tools::Counter fpsCounter;

    // glfw: program loop
    while (!glfwWindowShouldClose(window))
    {
        // glfw: input
        // close the window if the escape key is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        // enable wireframe mode when the '1' key is pressed
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // disable wireframe mode when the '2' key is pressed
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // zooming in and out
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
            settings.inv_scale_factor -= 0.1f;
        else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
            settings.inv_scale_factor += 0.1f;
        // moving the camera
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            settings.camera_position_y += 0.01f;
        else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            settings.camera_position_y -= 0.01f;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            settings.camera_position_x += 0.01f;
        else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            settings.camera_position_x -= 0.01f;
        // reset camera position and zoom
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        {
            settings.camera_position_x = 0.0f;
            settings.camera_position_y = 0.0f;
            settings.inv_scale_factor = 10.0f;
        }

        settings.reload_derived();

        // move the player to a set location when the 'T' key is pressed
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            player.velocity.x = 0.0f;
            player.velocity.y = 0.0f;
            player.p1 = playerInitPos;
            player.p2 = playerInitPos + 1.0f;
        }

        // player movement
        if (canJump && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            playerMoved.y = 9.0f / (float)fpsStopwatch.get();
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            playerMoved.y = -0.2f / (float)fpsStopwatch.get();
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            playerMoved.x = 0.2f / (float)fpsStopwatch.get();
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            playerMoved.x = -0.2f / (float)fpsStopwatch.get();
        
        // gl: render
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // move the player
        player.calcTimeStep((float)fpsStopwatch.get(), playerMoved.x, playerMoved.y + gravity);
        fpsStopwatch.reset(); // reset fps timer
        playerMoved.x = 0.0f;
        playerMoved.y = 0.0f;

        ocp::Point playerLeft(player.p1.x, player.p2.y);
        ocp::Point playerRight(player.p2.x, player.p1.y);
        // detect and resolve collisions
        voidMinorPosDiff(player.p1, playerLeft, settings.physics_error_margin);
        /*
        if (lineIntersectsLine(boxTop, box.p2, player.p1, playerLeft, intersect))
            std::cout << "blob" << std::endl;
        */
        if (player.p1.x < 0.0f)
        {
            player.p1.x = 0.0f;
            player.p2.x = 1.0f;
            player.velocity.x = 0.0f;
        }
        else if (player.p2.x > init_virtual_width)
        {
            player.p1.x = init_virtual_width - 1.0f;
            player.p2.x = init_virtual_width;
            player.velocity.x = 0.0f;
        }

        if (player.p1.y < 0.0f)
        {
            canJump = true;
            player.p1.y = 0.0f;
            player.p2.y = 1.0f;
            player.velocity.y = 0.0f;
        }
        else
        {
            canJump = false;
            if (player.p2.y > init_virtual_height)
            {
                player.p1.y = init_virtual_height - 1.0f;
                player.p2.y = init_virtual_height;
                player.velocity.y = 0.0f;
            }
        }

        // gl: render barriers
        lowerBound.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lowerBound.vertices), lowerBound.vertices, GL_DYNAMIC_DRAW);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), barrierColor[0], barrierColor[1], barrierColor[2], barrierColor[3]);
        glDrawArrays(GL_LINES, 0, 2);

        upperBound.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferData(GL_ARRAY_BUFFER, sizeof(upperBound.vertices), upperBound.vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, 2);

        leftBound.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferData(GL_ARRAY_BUFFER, sizeof(leftBound.vertices), leftBound.vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, 2);

        rightBound.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rightBound.vertices), rightBound.vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, 2);
        
        // gl: render the player
        player.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferData(GL_ARRAY_BUFFER, sizeof(player.vertices), player.vertices, GL_DYNAMIC_DRAW);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), playerColor[0], playerColor[1], playerColor[2], playerColor[3]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
        // gl: render the green box
        box.setVertices(settings.camera_position_x, settings.camera_position_y); // box position is static; no need to update vertices on every frame
        glBufferData(GL_ARRAY_BUFFER, sizeof(box.vertices), box.vertices, GL_DYNAMIC_DRAW);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), objectColor[0], objectColor[1], objectColor[2], objectColor[3]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // update fps counter every second
        if (titleUpdateStopwatch.get() >= 1.0)
        {
            std::stringstream frameCount;
            frameCount << settings.window_title << " [ FPS: " << fpsCounter.get() << " / " << settings.fps_cap << " ]";
            glfwSetWindowTitle(window, frameCount.str().c_str());
            fpsCounter.set(0);
            titleUpdateStopwatch.reset();
        }
        else
        {
            fpsCounter.inc();
        }

        // wait until the next frame
        while (fpsStopwatch.get() < settings.spf_cap)
        {
            // do nothing
        }
    }
    
    // gl: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}