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
#include "tools.h"
#include "settings.h"
#include "physics.h"

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
    Settings settings("data/settings.txt");
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

    // vertex shader source
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout(location = 0) in vec3 pos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(pos, 1.0);\n"
        "}\0";
    // fragment shader source
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "uniform vec4 color;\n"
        "out vec4 fragColor;\n"
        "void main()\n"
        "{\n"
        "    fragColor = vec4(color);\n"
        "}\0";

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
    
    // player object
    physics::Point playerInitPos = { (settings.window_virtual_width / 2.0f) - 0.5f, 0.0f };
    physics::Object player(settings.window_aspect_ratio_dec,
                           settings.inv_scale_factor,
                           physics::Point(playerInitPos.x, playerInitPos.y),
                           physics::Point(playerInitPos.x + 1.0f, playerInitPos.y + 1.0f));
    // additional variables
    float playerColor[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    physics::Vector playerMoved = { 0.0f, 0.0f };
    float gravity = -9.8f;
    bool canJump = false;

    // green box
    physics::Barrier2D box(settings.window_aspect_ratio_dec,
                           settings.inv_scale_factor,
                           physics::Point(0.0f, 0.0f),
                           physics::Point(2.0f, 2.0f));
    box.setVertices(settings.camera_position_x, settings.camera_position_y);
    // additional variables
    float barrierColor[4] = { 0.0f, 1.0f, 0.5f, 1.0f };
    bool inBox = false;

    physics::Point intersect;

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

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
            settings.inv_scale_factor -= 0.1;
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
            settings.inv_scale_factor += 0.1;

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
        canJump = false;
        
        // gl: render
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // move the player
        player.calcTimeStep((float)fpsStopwatch.get(), playerMoved.x, playerMoved.y + gravity);
        fpsStopwatch.reset(); // reset fps timer
        playerMoved.x = 0.0f;
        playerMoved.y = 0.0f;

        physics::Point playerLeft(player.p1.x, player.p2.y);
        physics::Point boxTop(box.p1.x, box.p2.y);
        // detect and resolve collisions
        voidFloatError(player.p1, playerLeft, settings.physics_error_margin);

        if (lineIntersectsLine(boxTop, box.p2, player.p1, playerLeft, intersect))
            std::cout << "blob" << std::endl;

        if (physics::objectIntersectsBarrier(player, box))
        {
            if (!inBox)
            {
                std::cout << "You are in the box!" << std::endl;
                inBox = true;
            }
        }
        else if (inBox)
        {
            std::cout << "You are not in the box." << std::endl;
            inBox = false;
        }
            
        if (player.p1.x < 0.0f)
        {
            player.p1.x = 0.0f;
            player.p2.x = 1.0f;
            player.velocity.x = 0.0f;
        }
        else if (player.p2.x > settings.window_virtual_width)
        {
            player.p1.x = settings.window_virtual_width - 1.0f;
            player.p2.x = settings.window_virtual_width;
            player.velocity.x = 0.0f;
        }

        if (player.p1.y < 0.0f)
        {
            canJump = true;
            player.p1.y = 0.0f;
            player.p2.y = 1.0f;
            player.velocity.y = 0.0f;
        }
        else if (player.p2.y > settings.window_virtual_height)
        {
            player.p1.y = settings.window_virtual_height - 1.0f;
            player.p2.y = settings.window_virtual_height;
            player.velocity.y = 0.0f;
        }
        
        // gl: render the player
        player.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferData(GL_ARRAY_BUFFER, sizeof(player.vertices), player.vertices, GL_DYNAMIC_DRAW);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), playerColor[0], playerColor[1], playerColor[2], playerColor[3]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
        // gl: render the green box
        box.setVertices(settings.camera_position_x, settings.camera_position_y); // box position is static; no need to update vertices on every frame
        glBufferData(GL_ARRAY_BUFFER, sizeof(box.vertices), box.vertices, GL_STATIC_DRAW);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), barrierColor[0], barrierColor[1], barrierColor[2], barrierColor[3]);
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