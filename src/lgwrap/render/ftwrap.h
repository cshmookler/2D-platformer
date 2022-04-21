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
    // unnamed namespace
    namespace {
        // output freetype errors
        FT_Error ftwrapHandleError(FT_Error error)
        {
            if (error != FT_Err_Ok)
            {
                std::cout << "FreeType: " << FT_Error_String(error) << std::endl;
                return error;
            }
            else
            {
                return 0;
            }
        }

        // information of each character
        struct CharSet {
            FT_UInt textureID; // ID handle of the glyph texture
            glm::vec2 size; // size of glyph
            glm::vec2 bearing; // offset from baseline to left/top of glyph
            FT_Pos advance; // offset to advance to next glyph
        };
    }

    class FontLibrary {
    private:
        // library ID
        FT_Library id_ = 0;
    public:
        // library ID reference
        const FT_Library& id = id_;
        // destructor
        ~FontLibrary(void)
        {
            FT_Done_FreeType(id_);
        }
        // init FreeType library
        FT_Error init(void)
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
        CharSet* charSet;
    public:
        // constructor
        Font(FontLibrary& library, const char* fontDir, int numChars) : lib(library), dir(fontDir), charSetLength(numChars)
        {
            charSet = new CharSet[charSetLength]{ 0 };
        }
        // destructor
        ~Font(void)
        {
            delete[] charSet;
        }
        // load font
        int load(void)
        {
            FT_Face face;
            FT_Error error = ftwrapHandleError(FT_New_Face(lib.id, dir, 0, &face));
            if (error)
                return error;

            FT_Set_Pixel_Sizes(face, 0, 48);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

            for (unsigned char i = 0; i < charSetLength; i++)
            {
                // load character glyph
                error = ftwrapHandleError(FT_Load_Char(face, i, FT_LOAD_RENDER));
                if (error)
                    return error;
                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // store character for later use
                charSet[i] = {
                    texture,
                    glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    face->glyph->advance.x
                };
            }
            // destroy 'face' after the textures are created and glyph data is moved to 'charSet'
            FT_Done_Face(face);

            return 0;
        }
        // initialize rendering
        void startRender(GLuint& VAO, GLuint& VBO, Shader shader, int windowWidth, int windowHeight)
        {
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
            glUniformMatrix4fv(shader.uniLoc("projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //shader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            shader.use();
        }
        // render a string of text
        void render(Shader& shader, std::string text, float xPos, float yPos, float scale, glm::vec3 color)
        {
            // set color
            glUniform3f(shader.uniLoc("textColor"), color.x, color.y, color.z);

            // temporary variables
            float x, y, w, h;

            for (int i = 0; i < text.length(); i++)
            {
                // determine x and y position on the window
                x = xPos + charSet[text[i]].bearing.x * scale;
                y = yPos - (charSet[text[i]].size.y - charSet[text[i]].bearing.y) * scale;

                // determine width and height
                w = charSet[text[i]].size.x * scale;
                h = charSet[text[i]].size.y * scale;

                float vertices[6][4] = {
                    { x,     y + h,   0.0f, 0.0f },
                    { x,     y,       0.0f, 1.0f },
                    { x + w, y,       1.0f, 1.0f },

                    { x,     y + h,   0.0f, 0.0f },
                    { x + w, y,       1.0f, 1.0f },
                    { x + w, y + h,   1.0f, 0.0f }
                };

                // render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, charSet[text[i]].textureID);
                // update content of VBO memory
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                // render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                xPos += (charSet[text[i]].advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            }
        }
        // (optional) unbinds the VAO, VBO, and the texture
        void stopRender(void)
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_BLEND);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    };
}