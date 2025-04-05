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
        Ray ray = Ray(start_position - delta, direction);

        assert(ray.start_position.x + delta.x - start_position.x < 0.00001 && "x");
        assert(ray.start_position.y + delta.y - start_position.y < 0.00001 && "y");
        assert(ray.start_position.z + delta.z - start_position.z < 0.00001 && "z");
    
        rotate(rotation, ray.start_position);
        rotate(rotation, ray.direction);
        return ray;
    }

    glm::vec3 start_position;
    glm::vec3 direction;

private:
    void rotate(glm::quat q, glm::vec3 &v)
    {
        glm::vec3 t = 2.f * glm::cross({q[1], q[2], q[3]}, v);
        v = v + q[0] * t + cross({q[1], q[2], q[3]}, t);
    }
};