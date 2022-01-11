#include <texture.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <spdlog/spdlog.h>
#include <stb_image.h>

Texture::Texture(const std::string& filename)
{
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    auto data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (data == nullptr)
    {
        spdlog::error("failed to load texture \"{}\"", filename);
        throw std::logic_error("failed to load texture");
    }

    auto tp = nrChannels == 3 ? GL_RGB : GL_RGBA;

    glTexImage2D(
        GL_TEXTURE_2D, 0, tp, width, height,
        0, tp, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

void Texture::bind(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, _id);
}
