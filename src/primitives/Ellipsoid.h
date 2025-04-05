#pragma once

#include "Primitive.h"

class Ellipsoid : public Primitive
{
public:
    Ellipsoid() : radius({0, 0, 0}) {}
    Ellipsoid(glm::vec3 r) : radius(r) {}

    std::optional<float> isIntersectedBy(Ray r) override
    {
        Ray ray = r.copyWith(position, inv_rotation);

        glm::vec3 o_div_radius = ray.start_position / radius;
        glm::vec3 direction_div_radius = ray.direction / radius;

        float a = dot(direction_div_radius, direction_div_radius);
        float b = dot(o_div_radius, direction_div_radius);
        float c = dot(o_div_radius, o_div_radius);

        float discriminant = b * b - a * (c - 1.0);

        if (discriminant < 0.0)
        {
            return std::nullopt;
        }

        discriminant = std::sqrt(discriminant);
        float t1 = (-b - discriminant) / a;
        float t2 = (-b + discriminant) / a;

        if (t2 < 0)
        {
            return std::nullopt;
        }

        if (t1 < 0)
        {
            t1 = t2;
        }
        return std::make_optional(t1);
    }

    glm::vec3 radius;
};

class EllipsoidCommand : public PrimitiveCommand
{
public:
    EllipsoidCommand(glm::vec3 r) : radius(r) {}
    void execute(Primitive *primitive) override
    {
        static_cast<Ellipsoid *>(primitive) -> radius = radius;
    }

private:
    glm::vec3 radius;
};