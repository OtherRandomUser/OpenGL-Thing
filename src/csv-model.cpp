#include <csv-model.hpp>

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

CsvModel::CsvModel(
    const std::string& filename,
    std::shared_ptr<ShaderProgram> shader,
    std::shared_ptr<Texture> texture)
{
    _vertex_count = 0;
    _vertices = nullptr;
    _vao = 0;
    _vbo = 0;

    _shader = std::move(shader);
    _texture = std::move(texture);

    parse_csv(filename);
    init_buffers();
}

CsvModel::CsvModel(CsvModel&& other)
{
    _vertex_count = other._vertex_count;
    _vertices = other._vertices;
    _vao = other._vao;
    _vbo = other._vbo;

    _shader = std::move(other._shader);
    _texture = std::move(other._texture);

    other._vertex_count = 0;
    other._vertices = nullptr;
    other._vao = 0;
    other._vbo = 0;
}

CsvModel& CsvModel::operator = (CsvModel&& other)
{
    _vertex_count = other._vertex_count;
    _vertices = other._vertices;
    _vao = other._vao;
    _vbo = other._vbo;

    _shader = std::move(other._shader);
    _texture = std::move(other._texture);

    other._vertex_count = 0;
    other._vertices = nullptr;
    other._vao = 0;
    other._vbo = 0;

    return *this;
}

void CsvModel::parse_csv(const std::string& filename)
{
    std::ifstream source_file(filename);

    if (!source_file.is_open())
    {
        spdlog::error("could not load csv model from \"{}\"", filename);
        throw std::invalid_argument("could not load csv model");
    }

    std::string line;

    while (std::getline(source_file, line))
#ifdef GENERATE_NORMALS
        _vertex_count += true_floats_per_line;
#else
        _vertex_count += floats_per_line;
#endif // GENERATE_NORMALS

    _vertices = new float[_vertex_count];

    spdlog::info("vertex count {}", _vertex_count);

    source_file.clear();
    source_file.seekg(0);

    int it = 0;

    while (std::getline(source_file, line))
    {
        int start_of_line = it;
        std::string::size_type begin = 0;

        while (true)
        {
            auto end = line.find(';', begin);

            if (end == std::string::npos || it == _vertex_count)
                break;

            auto substr = line.substr(begin, end - begin);
            _vertices[it] = std::stof(substr);

            it++;
            begin = end + 1;
        }

#ifdef GENERATE_NORMALS
        it += true_floats_per_line - file_floats_per_line;
#endif // GENERATE_NORMALS
    }

#ifdef GENERATE_NORMALS
    it = 0;

    while (it < _vertex_count)
    {
        glm::vec3 vertices[3];

        for (int i = 0; i < 3; i++)
        {
            int offset = it + i * true_floats_per_line;
            vertices[i].x = _vertices[offset];
            vertices[i].y = _vertices[offset + 1];
            vertices[i].z = _vertices[offset + 2];
            spdlog::info("vertex {}, {}, {}", vertices[i].x, vertices[i].y, vertices[i].z);
        }

        auto normal = glm::cross(
            vertices[0] - vertices[1],
            vertices[1] - vertices[2]
        );

        spdlog::info("normal {}, {}, {}", normal.x, normal.y, normal.z);

        for (int i = 0; i < 3; i++)
        {
            int offset = it + i * true_floats_per_line + file_floats_per_line;
            _vertices[offset] = normal.x;
            _vertices[offset + 1] = normal.y;
            _vertices[offset + 2] = normal.z;
        }

        it += 3 * true_floats_per_line;
    }

    // it = 0;

    // while (it < _vertex_count)
    // {
    //     spdlog::info("vert: {}", _vertices[it]);
    //     it++;
    //     if (it % true_floats_per_line == 0)
    //         spdlog::info("--------------");
    // }
#endif // GENERATE_NORMALS
}

void CsvModel::init_buffers()
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertex_count * sizeof(float), _vertices, GL_STATIC_DRAW);

#ifdef GENERATE_NORMALS
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        true_floats_per_line * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        true_floats_per_line * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        true_floats_per_line * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
        true_floats_per_line * sizeof(float), (void*) (8 * sizeof(float)));
    glEnableVertexAttribArray(3);
#else
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        floats_per_line * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        floats_per_line * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        floats_per_line * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
        floats_per_line * sizeof(float), (void*) (8 * sizeof(float)));
    glEnableVertexAttribArray(3);
#endif // GENERATE_NORMALS
}

CsvModel::~CsvModel()
{
    if (_vertices != nullptr)
        delete[] _vertices;
}

void CsvModel::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    if (_vertex_count == 0 || _vertices == nullptr)
        throw std::runtime_error("tried to render a moved csv model");

    auto shader_id = _shader->id();

    _texture->bind(0);

    auto model_loc = glGetUniformLocation(shader_id, "model");
    auto view_loc = glGetUniformLocation(shader_id, "view");
    auto projection_loc = glGetUniformLocation(shader_id, "projection");

    auto texture_loc = glGetUniformLocation(shader_id, "main_texture");

    glUniform1i(texture_loc, 0);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, _vertex_count);
}

void CsvModel::render_sun(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    if (_vertex_count == 0 || _vertices == nullptr)
        throw std::runtime_error("tried to render a moved csv model");

    auto shader_id = _shader->id();

    auto model_loc = glGetUniformLocation(shader_id, "model");
    auto view_loc = glGetUniformLocation(shader_id, "view");
    auto projection_loc = glGetUniformLocation(shader_id, "projection");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, _vertex_count);
}
