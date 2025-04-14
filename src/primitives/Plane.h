#pragma once

#include "Primitive.h"

class Plane : public virtual Primitive
{
public:
    Plane() : normal({0, 0, 0}) {}
    Plane(glm::vec3 n) : normal(n) {}

    std::optional<Intersection> intersectWith(const Ray& r) const override
    {
        Ray ray = r.copyWith(position, rotation);

        float t = -dot(ray.start_position, normal) / dot(ray.direction, normal);

        if (t < 0.0)
        {
            return std::nullopt;
        }

        Intersection i;
        i.color = color;
        i.dist = t;
        i.normal = rotation * normal;

        if (dot(ray.direction, normal) > 0)
        {
            i.isInside = true;
            i.normal = -i.normal;
        }

        return std::make_optional(i);
    }

    glm::vec3 normal;
};

class PlaneCommand : public PrimitiveCommand
{
public:
    PlaneCommand(const glm::vec3 &n) : normal(n) {}
    void execute(Primitive *primitive) override
    {
        dynamic_cast<Plane *>(primitive)->normal = normal;
    }

private:
    glm::vec3 normal;
};