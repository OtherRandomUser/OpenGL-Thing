#include <camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(
    glm::vec3 pos, glm::vec3 front, glm::vec3 up,
    float sensitivity, float yaw, float pitch, float speed)
{
    _pos = pos;
    _front = front;
    _up = up;

    _sensitivity = sensitivity;
    _yaw = yaw;
    _pitch = pitch;
    _speed = speed;

    turn(0.0f, 0.0f);
}

void Camera::turn(float x, float y)
{
    _yaw += x * _sensitivity;
    _pitch += y * _sensitivity;

    if (_pitch > 89.0f)
        _pitch = 89.0f;

    if (_pitch < -89.0f)
        _pitch = -89.0f;

    glm::vec3 direction;

    direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    direction.y = sin(glm::radians(_pitch));
    direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(direction);
}

void Camera::move_front()
{
    _pos += _speed * _front;
}

void Camera::move_back()
{
    _pos -= _speed * _front;
}

void Camera::move_left()
{
    _pos -= glm::normalize(glm::cross(_front, _up)) * _speed;
}

void Camera::move_right()
{
    _pos += glm::normalize(glm::cross(_front, _up)) * _speed;
}

glm::mat4 Camera::look_at()
{
    return glm::lookAt(_pos, _pos + _front, _up);
}
