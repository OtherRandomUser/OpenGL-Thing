#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    explicit Camera(
        glm::vec3 pos, glm::vec3 front,
        glm::vec3 up, float sensitivity = 1.0f,
        float yaw = -90.0f, float pitch = 0.0f,
        float speed = 0.05f);

    void turn(float x, float y);

    void move_front();
    void move_back();
    void move_left();
    void move_right();

    glm::mat4 look_at();

    glm::vec3 pos()
    {
        return _pos;
    }

private:
    glm::vec3 _pos;
    glm::vec3 _front;
    glm::vec3 _up;

    float _sensitivity;
    float _yaw;
    float _pitch;
    float _speed;
};
