#pragma once

#include "Primitive.h"

class SetPositionCommand : public PrimitiveCommand {
    public:
    SetPositionCommand (const glm::vec3& pos) : position (pos) {
    }

    void execute (Primitive* primitive) override {
        primitive->position = position;
    }

    private:
    glm::vec3 position;
};

class SetColorCommand : public PrimitiveCommand {
    public:
    SetColorCommand (const glm::vec3& c) : color (c) {
    }

    void execute (Primitive* primitive) override {
        primitive->color = color;
    }

    private:
    glm::vec3 color;
};

class SetDielectricCommand : public PrimitiveCommand {
    public:
    SetDielectricCommand () {
    }

    void execute (Primitive* primitive) override {
        primitive->material = Material::Dielectric;
    }
};

class SetMetallicCommand : public PrimitiveCommand {
    public:
    SetMetallicCommand () {
    }

    void execute (Primitive* primitive) override {
        primitive->material = Material::Metallic;
    }
};

class SetEmissionCommand : public PrimitiveCommand {
    public:
    SetEmissionCommand (const glm::vec3& c) : color (c) {
    }
    void execute (Primitive* primitive) override {
        primitive->emission = color;
    }

    private:
    glm::vec3 color;
};

class SetIORCommand : public PrimitiveCommand {
    public:
    SetIORCommand (float r) : ior (r) {
    }
    void execute (Primitive* primitive) {
        primitive->ior = ior;
    }

    private:
    float ior;
};

class SetRotationCommand : public PrimitiveCommand {
    public:
    SetRotationCommand (const glm::vec4& rotation) : rotation (rotation) {
    }

    void execute (Primitive* primitive) override {
        primitive->rotation.x = rotation.x;
        primitive->rotation.y = rotation.y;
        primitive->rotation.z = rotation.z;
        primitive->rotation.w = rotation.w;

        primitive->rotation     = glm::normalize (primitive->rotation);
        primitive->inv_rotation = glm::conjugate (primitive->rotation);
    }

    private:
    glm::vec4 rotation;
};
