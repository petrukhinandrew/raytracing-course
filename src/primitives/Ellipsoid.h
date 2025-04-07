#pragma once

#include "Primitive.h"
#include <optional>
#include <cmath>

class Ellipsoid : public virtual Primitive
{
public:
    Ellipsoid() : radius({0, 0, 0}) {}
    Ellipsoid(glm::vec3 r) : radius(r) {}

    std::optional<float> isIntersectedBy(Ray r) override
    {
        Ray ray = r.copyWith(position, inv_rotation);

        glm::vec3 o_div_radius = ray.start_position / radius;
        glm::vec3 direction_div_radius = ray.direction / radius;

        float a = glm::dot(direction_div_radius, direction_div_radius);
        float b = 2.0f * glm::dot(o_div_radius, direction_div_radius);
        float c = glm::dot(o_div_radius, o_div_radius) - 1.0f;

        float discriminant = b * b - 4.f * a * c;

        if (discriminant < 0.0f)
        {
            return std::nullopt; // No intersection
        }

        discriminant = std::sqrt(discriminant);
        float t1 = (-b - discriminant) / (2.f * a);
        float t2 = (-b + discriminant) / (2.f * a);

        // If both intersection points are negative, the intersection is behind the ray
        if (t2 < 0)
        {
            return std::nullopt; // No valid intersection
        }

        // Return the closest intersection point that is in front of the ray
        if (t1 < 0)
        {
            t1 = t2; // If t1 is negative, use t2
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
        dynamic_cast<Ellipsoid *>(primitive) -> radius = radius;
    }

private:
    glm::vec3 radius;
};