#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Plane.h"
#include "Ellipsoid.h"
#include "Box.h"
#include "Properties.h"

class PrimitiveCommandFactory
{
public:
    static std::unique_ptr<PrimitiveCommand> createCommand(const std::string &commandType, const std::vector<float> &args)
    {
        if (commandType == "PLANE")
        {
            auto plane = std::make_unique<PlaneCommand>(glm::vec3{args[0], args[1], args[2]});
            return plane;
        }
        else if (commandType == "ELLIPSOID")
        {
            auto ellipsoid = std::make_unique<EllipsoidCommand>(glm::vec3{args[0], args[1], args[2]});
            return ellipsoid;
        }
        else if (commandType == "BOX") 
        {
            auto boxid = std::make_unique<BoxCommand>(glm::vec3{args[0], args[1], args[2]});
            return boxid;
        }
        else if (commandType == "POSITION" && args.size() == 3)
        {
            return std::make_unique<SetPositionCommand>(glm::vec3{args[0], args[1], args[2]});
        }
        else if (commandType == "COLOR" && args.size() == 3)
        {
            return std::make_unique<SetColorCommand>(glm::vec3{args[0], args[1], args[2]});
        }
        else if (commandType == "ROTATION" && args.size() == 4)
        {
            return std::make_unique<SetRotationCommand>(glm::vec4{args[0], args[1], args[2], args[3]});
        }
        return nullptr;
    }
};