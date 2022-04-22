#pragma once

#include <iostream> // for debug
#include <string> // for when const char* won't work

// public (external) libraries
#include <glad/glad.h> // loader for OpenGL
#include <ft2build.h> // font loader
#include FT_FREETYPE_H

// OpenGL math
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/type_ptr.hpp>

// private libraries
#include "../physics/object.h"
#include "shader.h"

namespace lgw {
    // output freetype errors
    FT_Error ftwrapHandleError(FT_Error error);

    class FontLibrary {
    private:
        // library ID
        FT_Library id_ = 0;
    public:
        // library ID reference
        const FT_Library& id = id_;
        // destructor
        inline ~FontLibrary(void)
        {
            FT_Done_FreeType(id_);
        }
        // init FreeType library
        inline FT_Error init(void)
        {
            return ftwrapHandleError(FT_Init_FreeType(&id_));
        }
    };

    // font object
    class Font {
    private:
        FontLibrary& lib;
        const char* dir;
        int charSetLength;
        struct CharSet {
            FT_UInt textureID; // ID handle of the glyph texture
            glm::vec2 size; // size of glyph
            glm::vec2 bearing; // offset from baseline to left/top of glyph
            FT_Pos advance; // offset to advance to next glyph
        } *charSet;
    public:
        // constructor
        Font(FontLibrary& library, const char* fontDir, int numChars);
        // destructor
        inline ~Font(void)
        {
            delete[] charSet;
        }
        // load font
        int load(void);
        // initialize rendering
        void startRender(GLuint& VAO, GLuint& VBO, Shader* shader, int windowWidth, int windowHeight);
        // render a string of text
        void render(Shader* shader, std::string text, float xPos, float yPos, float scale, glm::vec3 color);
        // (optional) unbinds the VAO, VBO, and the texture
        void stopRender(void);
    };
}