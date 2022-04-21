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
#include <ft2build.h> // font loader
#include FT_FREETYPE_H

// OpenGL math
#include <glm/vec3.hpp> // glm::vec3

// local headers
#include "lgwrap/lgwrap.h"
#include "const.h"

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
// glfw: whenever a key is pressed this callback function executes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// glfw: whenever the mouse is moved this callback function executes
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
// glfw: whenever the mouse is clicked this callback function executes
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main(int argc, char** argv)
{
    // initialize settings
    lgw::Settings settings(CONST::SETTINGS_DIR);
    int error = settings.load();
    if (error == -1)
    {
        std::cout << "Settings file not found" << std::endl;
        return -1;
    }
    else if (error < -1)
    {
        std::cout << "Setting #" << -error - 2 << " not found in settings file" << std::endl;
        return -1;
    }
    
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // basic cross-platform compatibility
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    // glfw: window creation
    GLFWwindow* window = glfwCreateWindow(settings.window_width, settings.window_height, settings.window_title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "glfw: failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowAspectRatio(window, 16, 9);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "glad: init failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // basic shader program
    std::string vertexShaderDir = settings.shader_dir + "basic.vertex.glsl";
    std::string fragmentShaderDir = settings.shader_dir + "basic.fragment.glsl";
    lgw::VertexShader* vertexShader = new lgw::VertexShader(GL_VERTEX_SHADER, vertexShaderDir.c_str());
    vertexShader->compile();
    lgw::FragmentShader* fragmentShader = new lgw::FragmentShader(GL_FRAGMENT_SHADER, fragmentShaderDir.c_str());
    fragmentShader->compile();
    lgw::Shader* basicShader = new lgw::Shader(*vertexShader, *fragmentShader);
    if (basicShader->link())
        return -1;
    delete vertexShader;
    delete fragmentShader;
    
    // texture shader program
    vertexShaderDir = settings.shader_dir + "texture.vertex.glsl";
    fragmentShaderDir = settings.shader_dir + "texture.fragment.glsl";
    lgw::VertexShader* vertexShaderr = new lgw::VertexShader(GL_VERTEX_SHADER, vertexShaderDir.c_str());
    vertexShaderr->compile();
    lgw::FragmentShader* fragmentShaderr = new lgw::FragmentShader(GL_FRAGMENT_SHADER, fragmentShaderDir.c_str());
    fragmentShaderr->compile();
    lgw::Shader* textureShader = new lgw::Shader(*vertexShaderr, *fragmentShaderr);
    if (textureShader->link())
        return -1;
    delete vertexShaderr;
    delete fragmentShaderr;
    
    // gl: set up vertex data, buffers, and configure vertex attributes
    unsigned int rectIndices[6] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    GLuint VAO_basic, VBO_basic, EBO_basic;
    glGenVertexArrays(1, &VAO_basic);
    glBindVertexArray(VAO_basic);

    glGenBuffers(1, &VBO_basic);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_basic);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &EBO_basic);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_basic);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_basic);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_basic);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_basic);
    
    GLuint VAO_texture, VBO_texture;
    glGenVertexArrays(1, &VAO_texture);
    glBindVertexArray(VAO_texture);
    glGenBuffers(1, &VBO_texture);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, NULL, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    lgw::FontLibrary ftLibrary;
    ftLibrary.init();
    lgw::Font activeFont(ftLibrary, settings.font_dir.c_str(), 128);
    activeFont.load();

    //FT_F26Dot6 fontPoint = 100;
    //FT_Set_Char_Size(typeFace, 0, fontPoint * 64, settings.window_width, settings.window_height);
    /*FT_UInt glyph_index = FT_Get_Char_Index(typeFace, (int)'A');
    if (ftHandleError(FT_Load_Char(typeFace, glyph_index, FT_LOAD_RENDER))) // FT_LOAD_RENDER
    {
        FT_Done_FreeType(freetype);
        glfwTerminate();
        return -1;
    }*/

    // toggles
    // current draw mode
    lgw::Toggle wireframe(false, true);
    // display fps
    lgw::Toggle showFPS(false, true);

    // intersect point for CCD
    lgw::Point intersect;
    
    // player object
    lgw::Point playerInitPos = { (settings.window_virtual_width / 2.0f) - 0.5f, 0.0f };
    lgw::Object player(settings.window_aspect_ratio_dec, settings.inv_scale_factor, lgw::Point(playerInitPos.x, playerInitPos.y), lgw::Point(playerInitPos.x + 1.0f, playerInitPos.y + 1.0f));
    // additional variables
    float playerColor[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    lgw::Vector playerMoved = { 0.0f, 0.0f };
    float gravity = -9.8f;
    bool canJump = false;

    // green box
    lgw::Barrier2D box(settings.window_aspect_ratio_dec, settings.inv_scale_factor, lgw::Point(0.0f, 0.0f), lgw::Point(2.0f, 2.0f));
    box.setVertices(settings.camera_position_x, settings.camera_position_y);
    // additional variables
    float objectColor[4] = { 0.0f, 1.0f, 0.5f, 1.0f };

    // barriers
    lgw::Barrier1D lowerBound(settings.window_aspect_ratio_dec, settings.inv_scale_factor, lgw::Point(0.0f, 0.0f), lgw::Point(settings.window_virtual_width, 0.0f));
    lgw::Barrier1D upperBound(settings.window_aspect_ratio_dec, settings.inv_scale_factor, lgw::Point(0.0f, settings.window_virtual_height), lgw::Point(settings.window_virtual_width, settings.window_virtual_height));
    lgw::Barrier1D leftBound(settings.window_aspect_ratio_dec, settings.inv_scale_factor, lgw::Point(0.0f, 0.0f), lgw::Point(0.0f, settings.window_virtual_height));
    lgw::Barrier1D rightBound(settings.window_aspect_ratio_dec, settings.inv_scale_factor, lgw::Point(settings.window_virtual_width, 0.0f), lgw::Point(settings.window_virtual_width, settings.window_virtual_height));
    // additional variables
    float barrierColor[4] = { 0.75f, 0.0f, 0.0f, 1.0f };

    // initial window width and height
    float init_virtual_width = settings.window_virtual_width;
    float init_virtual_height = settings.window_virtual_height;

    // fps counter text
    std::string fpsText = "FPS: 0 / 0";

    // stores the amount of time elapsed since the beginning of the previous frame
    double timeElapsed;
    
    // timers
    // timer that is reset on every frame
    lgw::Stopwatch frameStopwatch;
    // timer that is used to update the fps counter once every second
    lgw::Stopwatch fpsStopwatch;
    // counter to count the number of frames
    lgw::Counter fpsCounter;
    
    // glfw: program loop
    while (!glfwWindowShouldClose(window))
    {
        // reset frame stopwatch and record the amount of time elapsed since the beginning of the previous frame
        timeElapsed = frameStopwatch.get();
        frameStopwatch.reset();

        // glfw: input
        // close the window if the escape key is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        // toggle wireframe mode when the space key is pressed down
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (wireframe.con)
            {
                wireframe.con = false;
                wireframe.toggle();
                if (wireframe.val)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        {
            wireframe.con = true;
        }
        // toggle fps
        if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
        {
            if (showFPS.con)
            {
                showFPS.con = false;
                showFPS.toggle();
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE)
        {
            showFPS.con = true;
        }

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
            playerMoved.y = 7.0f / (float)timeElapsed;
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            playerMoved.y = -0.2f / (float)timeElapsed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            playerMoved.x = 0.2f / (float)timeElapsed;
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            playerMoved.x = -0.2f / (float)timeElapsed;
        
        // move the player
        player.calcTimeStep((float)timeElapsed, playerMoved.x, playerMoved.y + gravity);
        playerMoved.x = 0.0f;
        playerMoved.y = 0.0f;

        lgw::Point playerLeft(player.p1.x, player.p2.y);
        lgw::Point playerRight(player.p2.x, player.p1.y);
        // detect and resolve collisions
        voidMinorPosDiff(player.p1, playerLeft, settings.physics_error_margin);

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

        // gl: clear window
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render text
        activeFont.startRender(VAO_texture, VBO_texture, *textureShader, settings.window_width, settings.window_height);
        if (showFPS.val)
            activeFont.render(*textureShader, fpsText.c_str(), 10.0f, 850.0f, 1.0f, glm::vec3(0.8, 0.8f, 0.8f));
        activeFont.stopRender();
        
        // gl: bind basic VAO, VBO, and EBO
        glBindVertexArray(VAO_basic);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_basic);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_basic);
        basicShader->use();
        
        // gl: render barriers
        lowerBound.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lowerBound.vertices), lowerBound.vertices);
        glUniform4f(basicShader->uniLoc("color"), barrierColor[0], barrierColor[1], barrierColor[2], barrierColor[3]);
        glDrawArrays(GL_LINES, 0, 2);

        upperBound.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(upperBound.vertices), upperBound.vertices);
        glDrawArrays(GL_LINES, 0, 2);

        leftBound.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(leftBound.vertices), leftBound.vertices);
        glDrawArrays(GL_LINES, 0, 2);

        rightBound.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rightBound.vertices), rightBound.vertices);
        glDrawArrays(GL_LINES, 0, 2);
        
        // gl: render the player
        player.setVertices(settings.camera_position_x, settings.camera_position_y);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(player.vertices), player.vertices);
        glUniform4f(basicShader->uniLoc("color"), playerColor[0], playerColor[1], playerColor[2], playerColor[3]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
        // gl: render the green box
        box.setVertices(settings.camera_position_x, settings.camera_position_y); // box position is static; no need to update vertices on every frame
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(box.vertices), box.vertices);
        glUniform4f(basicShader->uniLoc("color"), objectColor[0], objectColor[1], objectColor[2], objectColor[3]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        textureShader->use();
        
        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // update fps counter every second
        if (fpsStopwatch.get() >= 1.0)
        {
            std::stringstream frameCount;
            frameCount << "FPS: " << fpsCounter.get() << " / " << settings.fps_cap;
            fpsText = frameCount.str();
            fpsCounter.set(0);
            fpsStopwatch.reset();
        }
        else
        {
            fpsCounter.inc();
        }

        // wait until the next frame
        while (frameStopwatch.get() < settings.spf_cap)
        {
            // do nothing
        }
    }
    
    // gl: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO_basic);
    glDeleteBuffers(1, &VBO_basic);
    glDeleteBuffers(1, &EBO_basic);
    glDeleteVertexArrays(1, &VAO_texture);
    glDeleteBuffers(1, &VBO_texture);
    delete basicShader;
    delete textureShader;
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// function definitions ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever a key is pressed this callback function executes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case GLFW_KEY_W: {
        break;
    }
    case GLFW_KEY_A: {
        break;
    }
    case GLFW_KEY_S: {
        break;
    }
    case GLFW_KEY_D: {
        break;
    }
    default: {
        // do nothing
        return;
    }
    }
}

// glfw: whenever the mouse is moved this callback function executes
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

}

// glfw: whenever the mouse is clicked this callback function executes
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

}