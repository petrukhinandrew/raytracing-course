#pragma once

#include "glm.hpp"

enum LightSourceKind
{
    Directional,
    Positional
};

class LightSource
{
public:
    LightSource() {}
    LightSourceKind kind;
    glm::vec3 position = {0.f, 0.f, 0.f};
    glm::vec3 direction = {0.f, 0.f, 0.f};
    glm::vec3 intensity = {0.f, 0.f, 0.f};
    glm::vec3 attenuation = {0.f, 0.f, 0.f};
};

class LightSourceCommand
{
public:
    virtual void execute(LightSource *lightSource) {}
    virtual ~LightSourceCommand() = default;
};
