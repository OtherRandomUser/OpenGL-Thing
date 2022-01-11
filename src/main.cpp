#include <memory>
#include <vector>

#include <fmt/format.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <camera.hpp>
#include <csv-model.hpp>
#include <settings.hpp>
#include <shader.hpp>
#include <texture.hpp>

constexpr int window_width = 800;
constexpr int window_height = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void opengl_message_callback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR)
        spdlog::error("Open GL error: type = {}, severity = {}, message = {}",
        type, severity, message);
    else
        spdlog::warn("Open GL: type = {}, severity = {}, message = {}",
        type, severity, message);
}

void process_input(GLFWwindow* window, Camera& camera)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.move_front();

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.move_back();

    if(glfwGetKey(window, GLFW_KEY_A)== GLFW_PRESS)
        camera.move_left();

    if (glfwGetKey(window, GLFW_KEY_D)== GLFW_PRESS)
        camera.move_right();

    if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        camera.turn(0.0f,1.0f);

    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.turn(0.0f,-1.0f);

    if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        camera.turn(-1.0f, 0.0f);

    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        camera.turn(1.0f, 0.0f);
}

int main()
{
    auto settings = load_settings("settings.json");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(
        window_width,
        window_height,
        "Trabalho 3",
        nullptr,
        nullptr);

    if (window == nullptr)
    {
        spdlog::error("Failed to create window");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK)
    {
        spdlog::error("Failed to initialize GLEW!");
        return EXIT_FAILURE;
    }

    spdlog::info("Successfully loaded GLEW");
    spdlog::info("OpenGL version: {}", glGetString(GL_VERSION));

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_message_callback, 0);

    glEnable(GL_DEPTH_TEST);

    auto vertex_shader_filename = fmt::format(
        "{}/shaders/{}",
        settings.root_folder,
        settings.vertex_shader);

    auto fragment_shader_filename = fmt::format(
        "{}/shaders/{}",
        settings.root_folder,
        settings.fragment_shader);

    auto shader = std::make_shared<ShaderProgram>(
        vertex_shader_filename,
        fragment_shader_filename);

    std::vector<CsvModel> scene;

    for (auto& object: settings.objects)
    {
        auto texture_filename = fmt::format(
            "{}/res/{}",
            settings.root_folder,
            object.texture);

        auto texture = std::make_shared<Texture>(texture_filename);

        auto model_filename = fmt::format(
            "{}/res/{}",
            settings.root_folder,
            object.model);

        scene.emplace_back(
            model_filename,
            shader,
            texture);

        spdlog::info("loaded object {}", model_filename);
    }

    auto sun_vert_shader_filename = fmt::format(
        "{}/shaders/{}",
        settings.root_folder,
        settings.sun.vertex_shader);

    spdlog::info("vertex {}", vertex_shader_filename);

    auto sun_frag_shader_filename = fmt::format(
        "{}/shaders/{}",
        settings.root_folder,
        settings.sun.fragment_shader);

    auto sun_shader = std::make_shared<ShaderProgram>(
        sun_vert_shader_filename,
        sun_frag_shader_filename);

    auto sun_model_filename = fmt::format(
        "{}/res/{}",
        settings.root_folder,
        settings.sun.model);

    CsvModel sun_model(
        sun_model_filename,
        sun_shader,
        nullptr);

    Camera camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 light_pos(0.0f, -0.5f, 3.0f);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window, camera);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto angle = fmodf((float) glfwGetTime(), 3.5f);
        auto light_rot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(-0.5f, 0.0f, 0.0f));

        shader->use();

        auto model = glm::mat4(1.0f);
        auto view = glm::mat4(1.0f);
        auto projection = glm::mat4(1.0f);

        constexpr bool spin = false;

        if (spin)
        {
            model = glm::rotate(model,
                (float) glfwGetTime(), glm::vec3(0.0f, -0.5f, 0.0f));

            view = glm::translate(view, glm::vec3(0.0f, -0.7f, -3.0f));
        }
        else
        {
            view = camera.look_at();
        }

        projection = glm::perspective(glm::radians(45.0f),
            (float) window_width / (float) window_height, 0.1f, 100.0f);

        auto shader_id = shader->id();
        auto light_pos_loc = glGetUniformLocation(shader_id, "lightPos");
        auto light_rot_loc = glGetUniformLocation(shader_id, "lightRot");
        auto light_color_loc = glGetUniformLocation(shader_id, "lightColor");
        auto view_pos_loc = glGetUniformLocation(shader_id, "viewPos");
        glUniform3fv(light_pos_loc, 1, glm::value_ptr(light_pos));
        glUniformMatrix4fv(light_rot_loc, 1, GL_FALSE, glm::value_ptr(light_rot));
        glUniform3f(light_color_loc, 1.0f, 1.0f, 0.58f);
        glUniform3fv(view_pos_loc, 1, glm::value_ptr(camera.pos()));

        for (auto& object: scene)
            object.render(model, view, projection);

        sun_shader->use();
        model = light_rot;
        model = glm::translate(model, light_pos);
        model = glm::scale(model, glm::vec3(0.2f));
        sun_model.render_sun(model, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
