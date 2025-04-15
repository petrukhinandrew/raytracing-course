#pragma once

#include "gtc/quaternion.hpp"

class Ray {
    public:
    Ray (glm::vec3 position, glm::vec3 d)
    : start_position (position), direction (glm::normalize (d)) {
    }

    Ray copyWith (const glm::vec3& delta, const glm::quat& rotation) const {
        auto newRay = Ray (rotation * (start_position - delta), rotation * direction);
        newRay.depth = depth;
        return newRay;
    }

    glm::vec3 start_position;
    glm::vec3 direction;
    int depth = 1;
};