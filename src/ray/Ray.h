#pragma once

#include <cassert>
#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "gtc/matrix_transform.hpp"

class Ray
{
public:
    Ray(glm::vec3 position, glm::vec3 d) : start_position(position), direction(glm::normalize(d)) {}

    Ray copyWith(glm::vec3 delta, glm::quat rotation)
    {
        return Ray(rotation * (start_position - delta), rotation * direction);
    }

    glm::vec3 start_position;
    glm::vec3 direction;
};