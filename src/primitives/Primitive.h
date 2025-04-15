#pragma once

#include "Material.h"
#include "intersection/Intersection.h"
#include "ray/Ray.h"
#include <optional>

class Primitive {
    public:
    Primitive () {
    }
    glm::vec3 position     = { 0.0, 0.0, 0.0 };
    glm::vec3 color        = { 0.0, 0.0, 0.0 };
    glm::quat rotation     = glm::quat (1.0, 0.0, 0.0, 0.0);
    glm::quat inv_rotation = glm::quat (1.0, 0.0, 0.0, 0.0);
    float ior              = 1;
    Material material      = Material::Diffuse;
    glm::vec3 emission     = { 0.0, 0.0, 0.0 };

    virtual std::optional<Intersection> intersectWith (const Ray& r) const = 0;
    virtual ~Primitive () = default;
};

class PrimitiveCommand {
    public:
    virtual void execute (Primitive* primitive) {
    }
    virtual ~PrimitiveCommand () = default;
};
