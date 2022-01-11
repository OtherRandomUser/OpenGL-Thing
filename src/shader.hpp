#pragma once

#include <string>

#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram(
        const std::string& vert_shader_filename,
        const std::string& frag_shader_filename);

    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram(ShaderProgram&& other);

    ShaderProgram& operator = (const ShaderProgram& other) = delete;
    ShaderProgram& operator = (ShaderProgram&& other);

    ~ShaderProgram();

    unsigned int id() const
    {
        return _id;
    }

    void use() const
    {
        glUseProgram(_id);
    }

private:
    unsigned int _id;
};
