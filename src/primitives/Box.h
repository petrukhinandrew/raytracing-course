#pragma once

#include "Primitive.h"
#include <glm.hpp>

class Box : public Primitive
{
public:
    Box() : sizes({0, 0, 0}) {}
    Box(const glm::vec3 s) : sizes(s) {}
    std::optional<float> isIntersectedBy(Ray r) override
    {
        Ray ray = r.copyWith(position, inv_rotation);

        glm::vec3 t1 = (-sizes - ray.start_position) / ray.direction;
        glm::vec3 t2 = (sizes - ray.start_position) / ray.direction;

        if (t1.x > t2.x)
        {
            std::swap(t1.x, t2.x);
        }
        if (t1.y > t2.y)
        {
            std::swap(t1.y, t2.y);
        }
        if (t1.z > t2.z)
        {
            std::swap(t1.z, t2.z);
        }

        float t1_max = std::max(std::max(t1.x, t1.y), t1.z);
        float t2_min = std::min(std::min(t2.x, t2.y), t2.z);

        if (t1_max > t2_min || t2_min < 0)
        {
            return std::nullopt;
        }

        if (t1_max < 0)
        {
            t1_max = t2_min;
        }

        return std::make_optional(t1_max);
    }

    glm::vec3 sizes;
};

class BoxCommand : public PrimitiveCommand
{
public:
    BoxCommand(const glm::vec3 &s) : sizes(s) {}
    void execute(Primitive* primitive) override
    {
        static_cast<Box *>(primitive) -> sizes = sizes;
    }

private:
    glm::vec3 sizes;
};