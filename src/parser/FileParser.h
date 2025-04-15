#pragma once

#include "Command.h"
#include "lights/LightCommandFactory.h"
#include "lights/LightSource.h"
#include "primitives/Ellipsoid.h"
#include "primitives/Plane.h"
#include "primitives/Primitive.h"
#include "primitives/PrimitiveCommandFactory.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class CommandParser {
    public:
    virtual void parse (const std::string& filename) = 0;
    virtual ~CommandParser ()                        = default;
};

class TextFileParser : public CommandParser {
    public:
    Scene& scene;

    TextFileParser (Scene& s) : scene (s) {
    }

    void parse (const std::string& filename) override {
        std::ifstream file (filename);
        if (!file.is_open ()) {
            throw std::runtime_error ("Could not open file: " + filename);
        }

        std::string line;
        while (std::getline (file, line)) {
            std::istringstream iss (line);
            std::string command;
            if (!(iss >> command)) {
                continue;
            }

            std::vector<float> args;
            float arg;
            while (iss >> arg) {
                args.push_back (arg);
            }

            if (command == "NEW_PRIMITIVE")
                continue;

            if (command == "NEW_LIGHT") {
                scene.addLightSource ();
                continue;
            }

            if (command == "ELLIPSOID") {
                scene.addEllipsoid ();
            }
            if (command == "PLANE") {
                scene.addPlane ();
            }
            if (command == "BOX") {
                scene.addBox ();
            }

            auto lightCommand = LightCommandFactory::createCommand (command, args);
            if (lightCommand != nullptr) {
                lightCommand->execute (scene.currentLight ());
                continue;
            }

            auto primitiveCommand = PrimitiveCommandFactory::createCommand (command, args);
            if (primitiveCommand != nullptr) {
                primitiveCommand->execute (scene.currentPrimitive ());
                continue;
            } else {
                auto cmd = CommandFactory::createCommand (command, args);
                if (cmd != nullptr)
                    cmd->execute (scene);
            }
        }
        scene.updateCameraFovY ();
        file.close ();
    }
};
