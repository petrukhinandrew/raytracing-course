#pragma once

#include "Primitive.h"
#include <optional>
#include <cmath>

class Ellipsoid : public virtual Primitive
{
public:
    Ellipsoid() : radius({0, 0, 0}) {}
    Ellipsoid(glm::vec3 r) : radius(r) {}

    std::optional<Intersection> intersectWith(const Ray &r) const override
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
            return std::nullopt;
        }

        discriminant = std::sqrt(discriminant);
        float t1 = (-b - discriminant) / (2.f * a);
        float t2 = (-b + discriminant) / (2.f * a);

        if (t2 < 0)
        {
            return std::nullopt;
        }

        Intersection i;
        i.color = color;

        if (t1 < 0)
        {
            t1 = t2;
            i.isInside = true;
        }
        i.dist = t1;
        i.normal = glm::normalize((ray.start_position + i.dist * ray.direction) / (radius * radius));
        i.normal = rotation * (i.isInside ? -i.normal : i.normal);
        return std::make_optional(i);
    }

    glm::vec3 radius;
};

class EllipsoidCommand : public PrimitiveCommand
{
public:
    EllipsoidCommand(const glm::vec3 &r) : radius(r) {}
    void execute(Primitive *primitive) override
    {
        dynamic_cast<Ellipsoid *>(primitive) -> radius = radius;
    }

private:
    glm::vec3 radius;
};