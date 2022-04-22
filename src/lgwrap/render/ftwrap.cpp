#include "ftwrap.h"

// output freetype errors
FT_Error lgw::ftwrapHandleError(FT_Error error)
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

// font object: constructor
lgw::Font::Font(FontLibrary& library, const char* fontDir, int numChars) : lib(library), dir(fontDir), charSetLength(numChars)
{
    charSet = new CharSet[charSetLength]{ 0 };
}
// load font
int lgw::Font::load(void)
{
    FT_Face face;
    FT_Error error = ftwrapHandleError(FT_New_Face(lib.id, dir, 0, &face));
    if (error)
        return error;

    FT_Set_Pixel_Sizes(face, 0, 30);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
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
void lgw::Font::startRender(GLuint& VAO, GLuint& VBO, Shader* shader, int windowWidth, int windowHeight)
{
    shader->use();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
    glUniformMatrix4fv(shader->uniLoc("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}
// render a string of text
void lgw::Font::render(Shader* shader, std::string text, float xPos, float yPos, float scale, glm::vec3 color)
{
    // set color
    glUniform3f(shader->uniLoc("textColor"), color.x, color.y, color.z);

    // temporary variables
    float x, y, w, h;

    for (int i = 0; i < text.length(); i++)
    {
        // new line
        if (text[i] == 10)
        {
            yPos += (charSet[text[i]].bearing.y / 64) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            continue;
        }

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
void lgw::Font::stopRender(void)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}