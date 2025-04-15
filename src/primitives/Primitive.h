#pragma once

#include "Material.h"
#include "ray/Ray.h"
#include <limits>
#include <optional>

class Primitive;

typedef struct {
    glm::vec3 normal, color = { 0, 0, 0 };
    float dist           = std::numeric_limits<float>::max ();
    bool isInside        = false;
    Primitive* primitive = nullptr;
} Intersection;

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
