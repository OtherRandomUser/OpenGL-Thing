#include <shader.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include <spdlog/spdlog.h>

unsigned int compile_shader(const std::string& filename, GLenum tp)
{
    std::ifstream source_file(filename);

    if (!source_file.is_open())
    {
        std::cout << "ERROR: could not load shader from \""
            << filename << "\"\n";
        return 0;
    }

    std::stringstream buffer;
    buffer << source_file.rdbuf();

    std::string source_code = buffer.str();

    unsigned int id = glCreateShader(tp);
    auto cstr = source_code.c_str();
    glShaderSource(id, 1, &cstr, NULL);
    glCompileShader(id);

    int success;
    constexpr int info_log_size = 1024;
    char info_log[info_log_size];

    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(id, info_log_size, NULL, info_log);
        spdlog::error("compilation of shader \"{}\" failed", filename);
        spdlog::error("GL log \"{}\"", info_log);
        return 0;
    }

    return id;
}

ShaderProgram::ShaderProgram(
    const std::string& vert_shader_filename,
    const std::string& frag_shader_filename)
{
    _id = 0;

    auto vert_shader_id = compile_shader(
        vert_shader_filename,
        GL_VERTEX_SHADER);

    if (vert_shader_id == 0)
    {
        throw std::logic_error("failed to compile vertex shader");
    }

    auto frag_shader_id = compile_shader(
        frag_shader_filename,
        GL_FRAGMENT_SHADER);

    if (frag_shader_id == 0)
    {
        glDeleteShader(vert_shader_id);
        throw std::logic_error("failed to compile vertex shader");
    }

    _id = glCreateProgram();

    glAttachShader(_id, vert_shader_id);
    glAttachShader(_id, frag_shader_id);
    glLinkProgram(_id);

    glDeleteShader(vert_shader_id);
    glDeleteShader(frag_shader_id);

    int success = 0;
    constexpr int info_log_size = 1024;
    char info_log[info_log_size];

    glGetProgramiv(_id, GL_LINK_STATUS, &success);
    if(success == GL_FALSE)
    {
        glGetShaderInfoLog(_id, info_log_size, NULL, info_log);
        spdlog::error("shader program linking failed");
        spdlog::error("GL log \"{}\"", info_log);
        throw std::logic_error("failed to link shader program");
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
{
    _id = other._id;
    other._id = 0;
}

ShaderProgram& ShaderProgram::operator = (ShaderProgram&& other)
{
    _id = other._id;
    other._id = 0;

    return *this;
}

ShaderProgram::~ShaderProgram()
{
    if (_id != 0)
        glDeleteProgram(_id);
}
