#pragma once

#include "Box.h"
#include "Ellipsoid.h"
#include "Plane.h"
#include "Properties.h"
#include <string>
#include <vector>

class PrimitiveCommandFactory {
    public:
    static PrimitiveCommand* createCommand (const std::string& commandType,
    const std::vector<float>& args) {
        if (commandType == "PLANE") {
            auto plane = new PlaneCommand (glm::vec3{ args[0], args[1], args[2] });
            return plane;
        } else if (commandType == "ELLIPSOID") {
            auto ellipsoid =
            new EllipsoidCommand (glm::vec3{ args[0], args[1], args[2] });
            return ellipsoid;
        } else if (commandType == "BOX") {
            auto boxid = new BoxCommand (glm::vec3{ args[0], args[1], args[2] });
            return boxid;
        } else if (commandType == "POSITION" && args.size () == 3) {
            return new SetPositionCommand (glm::vec3{ args[0], args[1], args[2] });
        } else if (commandType == "COLOR" && args.size () == 3) {
            return new SetColorCommand (glm::vec3{ args[0], args[1], args[2] });
        } else if (commandType == "ROTATION" && args.size () == 4) {
            return new SetRotationCommand (
            glm::vec4{ args[0], args[1], args[2], args[3] });
        } else if (commandType == "DIELECTRIC" && args.size () == 0) {
            return new SetDielectricCommand ();
        } else if (commandType == "METALLIC" && args.size () == 0) {
            return new SetMetallicCommand ();
        } else if (commandType == "EMISSION" && args.size () == 3) {
            return new SetEmissionCommand (glm::vec3{ args[0], args[1], args[2] });
        } else if (commandType == "IOR" && args.size () == 1) {
            return new SetIORCommand (args[0]);
        }
        return nullptr;
    }
};