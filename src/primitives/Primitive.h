#pragma once

#include <memory>
#include <string>
#include <optional>
#include "glm.hpp"
#include "Material.h"
#include "ray/Ray.h"

class Primitive
{
public:
    Primitive() {

    }
    glm::vec3 position = {0.0, 0.0, 0.0};
    glm::vec3 color = {0.0, 0.0, 0.0};
    glm::quat rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
    glm::quat inv_rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
    Material material = Material::Diffuse;

    virtual std::optional<float> isIntersectedBy(Ray r) = 0;
    virtual ~Primitive() = default;
};

class PrimitiveCommand
{
public:
    virtual void execute(Primitive* primitive) {}
    virtual ~PrimitiveCommand() = default;
};
