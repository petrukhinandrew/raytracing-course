#pragma once

#include "glm.hpp"
#include <random>

enum LightSourceKind { Directional, Positional };

class LightSource {
    public:
    LightSource () {
    }
    LightSourceKind kind;
    glm::vec3 position    = { 0.f, 0.f, 0.f };
    glm::vec3 direction   = { 0.f, 0.f, 0.f };
    glm::vec3 intensity   = { 0.f, 0.f, 0.f };
    glm::vec3 attenuation = { 0.f, 0.f, 0.f };

    // For direct light sampling
    glm::vec3 samplePoint(float random) const {
        if (kind == LightSourceKind::Directional) {
            return position;
        }
        // For positional lights, we'll treat them as point lights for now
        return position;
    }

    float getArea() const {
        if (kind == LightSourceKind::Directional) {
            return 1.0f; // Directional lights have infinite area
        }
        // For positional lights, we'll treat them as point lights for now
        return 1.0f;
    }

    glm::vec3 getIntensity() const {
        return intensity;
    }
};

class LightSourceCommand {
    public:
    virtual void execute (LightSource* lightSource) const {
    }
    virtual ~LightSourceCommand () = default;
};
