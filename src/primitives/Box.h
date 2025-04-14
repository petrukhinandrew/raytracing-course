#pragma once

#include "Primitive.h"
#include <glm.hpp>

class Box : public virtual Primitive
{
public:
    Box() : sizes({0, 0, 0}) {}
    Box(const glm::vec3 s) : sizes(s) {}
    std::optional<Intersection> intersectWith(Ray r) override
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
        
        Intersection i;
        i.color = color;

        if (t1_max < 0)
        {
            t1_max = t2_min;
            i.isInside = true;
        }
        i.dist = t1_max;
        i.normal = (ray.start_position + i.dist * ray.direction) / sizes;

        float max_distance = 0.0;
        int max_index = 0;
        if (std::abs(i.normal.x) >= max_distance) {
            max_distance = std::abs(i.normal.x);
            max_index = 0;
        }
        if (std::abs(i.normal.y) >= max_distance) {
            max_distance = std::abs(i.normal.y);
            max_index = 1;
        }
        if (std::abs(i.normal.z) >= max_distance) {
            max_distance = std::abs(i.normal.z);
            max_index = 2;
        }

        i.normal.x = (0 == max_index) ? (i.normal.x > 0.0 ? 1.0 : -1.0) : 0.0;
        i.normal.y = (1 == max_index) ? (i.normal.y > 0.0 ? 1.0 : -1.0) : 0.0;
        i.normal.z = (2 == max_index) ? (i.normal.z > 0.0 ? 1.0 : -1.0) : 0.0;
        i.normal = glm::normalize(i.normal);

        i.normal = rotation * (i.isInside ? -i.normal : i.normal);


        return std::make_optional(i);
    }

    glm::vec3 sizes;
};

class BoxCommand : public PrimitiveCommand
{
public:
    BoxCommand(const glm::vec3 &s) : sizes(s) {}
    void execute(Primitive* primitive) override
    {
        dynamic_cast<Box *>(primitive) -> sizes = sizes;
    }

private:
    glm::vec3 sizes;
};