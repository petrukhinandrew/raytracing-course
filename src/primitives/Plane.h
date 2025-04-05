#pragma once

#include "Primitive.h"

class Plane : public Primitive
{
public:
    Plane() : normal({0, 0, 0}) {}
    Plane(glm::vec3 n) : normal(n) {}

    std::optional<float> isIntersectedBy(Ray r) override
    {
        Ray ray = r.copyWith(position, rotation);

        float t = -dot(ray.start_position, normal) / dot(ray.direction, normal);

        if (t < 0.0)
        {
            return std::nullopt;
        }

        return std::make_optional(t);
    }

    glm::vec3 normal;
};

class PlaneCommand : public PrimitiveCommand
{
public:
    PlaneCommand(glm::vec3 n) : normal(n) {}
    void execute(Primitive *primitive) override
    {
        static_cast<Plane *>(primitive)->normal = normal;
    }

private:
    glm::vec3 normal;
};