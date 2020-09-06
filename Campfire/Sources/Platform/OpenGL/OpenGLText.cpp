#include "Platform/OpenGL/OpenGLText.h"
#include "Scene/Component.h"
#include "Renderer/Renderer2D.h"

// ===================================
// Font
// ===================================
OpenGLFont::OpenGLFont(const std::string& fontPath)
    : path(fontPath)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        LOG_ERROR("OpenGLFont::Unable to init library");
        return;
    }

    FT_Face face;
    // Check that font is valid
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        LOG_ERROR("OpenGLFont::Failed to load font: {0}", fontPath);
        return;
    }

    // Set font's width and height parameters.
    // Setting the width to 0, allows face to dynamically calculate
    // width based on the given height
    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; ++c)
    {
        // Load character glype
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOG_ERROR("OpenGLFont::Failed to load glype {0}", c);
            continue;
        }

        // Generate texture for glyph
        uint32_t textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

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

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SharedPtr<Texture2D> texture = Texture2D::Create(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        texture->SetData(textureID);
        // store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        characters.emplace(c, character);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

// ===================================
// Text
// ===================================
OpenGLText::OpenGLText(const std::string& newText)
{
    text = newText;
}

void OpenGLText::Draw()
{
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 euler = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    auto characters = font->GetCharacterMap();
    for (auto c = text.begin(); c != text.end(); c++)
    {
        glm::mat4 model = glm::mat4(1.0f);

        Character ch = characters[*c];

        float xpos = pos.x + ch.bearing.x * scale.x;
        float ypos = pos.y + ch.bearing.y * scale.y;

        float w = ch.size.x * scale.x;
        float h = ch.size.y * scale.y;

        model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 1.0f));

        pos.x += (ch.advance >> 6) * scale.x;

        Renderer2D::SubmitQuad(model, ch.texture, color);
    }
}
