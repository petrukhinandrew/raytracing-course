#pragma once

#include "Primitive.h"
#include "glm.hpp"

class SetPositionCommand : public PrimitiveCommand
{
public:
    SetPositionCommand(const glm::vec3 &pos) : position(pos) {}

    void execute(Primitive *primitive) override
    {
        primitive->position = position;
    }

private:
    glm::vec3 position;
};

class SetColorCommand : public PrimitiveCommand
{
public:
    SetColorCommand(const glm::vec3 &color) : color(color) {}

    void execute(Primitive *primitive) override
    {
        primitive->color = color;
    }

private:
    glm::vec3 color;
};

class SetRotationCommand : public PrimitiveCommand
{
public:
    SetRotationCommand(const glm::vec4 &rotation) : rotation(rotation) {}

    void execute(Primitive *primitive) override
    {
        primitive->rotation = glm::quat(rotation);
        primitive->inv_rotation = glm::conjugate(primitive->rotation);
    }

private:
    glm::vec4 rotation;
};
