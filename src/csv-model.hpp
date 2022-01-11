#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <shader.hpp>
#include <texture.hpp>

// #define GENERATE_NORMALS

class CsvModel
{
public:
    CsvModel(
        const std::string& filename,
        std::shared_ptr<ShaderProgram> shader,
        std::shared_ptr<Texture> texture);

    CsvModel(const CsvModel& other) = delete;
    CsvModel(CsvModel&& other);

    CsvModel& operator = (const CsvModel& other) = delete;
    CsvModel& operator = (CsvModel&& other);

    ~CsvModel();

    void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void render_sun(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

private:
#ifdef GENERATE_NORMALS
    static constexpr int file_floats_per_line = 8;
    static constexpr int true_floats_per_line = 11;
#else
    static constexpr int floats_per_line = 11;
#endif // GENERATE_NORMALS

    void parse_csv(const std::string& filename);
    void init_buffers();

    int _vertex_count;
    float *_vertices;

    unsigned int _vao;
    unsigned int _vbo;

    std::shared_ptr<ShaderProgram> _shader;
    std::shared_ptr<Texture> _texture;
};
