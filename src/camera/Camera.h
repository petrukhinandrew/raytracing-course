#pragma once

#include <cassert>

#include "ray/Ray.h"
#include "glm.hpp"

void dump(std::string extra, glm::vec3 v)
{
    std::cout << extra << v.x << " " << v.y << " " << v.z << std::endl;
}

class Camera
{
public:
    Camera() : position(0), right(0), up(0), forward(0), fov(0) {}

    void setDimensions(const glm::ivec2 dims)
    {
        this->dimensions = dims;
    }

    void setPosition(const glm::vec3 &position)
    {
        this->position = position;
    }

    void setRight(const glm::vec3 &right)
    {
        this->right = right;
    }

    void setUp(const glm::vec3 &up)
    {
        this->up = up;
    }

    void setForward(const glm::vec3 &forward)
    {
        this->forward = forward;
    }

    void setFovX(float fov_x)
    {
        this->fov = glm::vec2{(float)std::tan(fov_x / 2.f), 0.f};
    }

    void updateFovY(float aspect_ratio)
    {
        this->fov.y = fov.x / aspect_ratio;
    }

    Ray trace(glm::vec2 pixel)
    {
        glm::vec3 pos{
            (2.f * ((float)(pixel.x) + 0.5f) / (float)(dimensions.x) - 1.0f) * fov.x,
            -(2.f * ((float)(pixel.y) + 0.5f) / (float)(dimensions.y) - 1.0f) * fov.y,
            1};

        glm::vec3 direction = glm::normalize(pos.x * right + pos.y * up + pos.z * forward);

        return Ray{position, direction};
    }

private:
    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;
    glm::vec2 dimensions;

    glm::vec2 fov;
};
