#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <scene/Scene.h>

class Command {
public:
    virtual void execute(Scene& s) { }
    virtual ~Command() = default;
};

class DimensionsCommand : public Command {
public:
    DimensionsCommand(float w, float h) : width(w), height(h) {}
    
    void execute(Scene& s) override {
        s.setDimensions(width, height);
    }
private:
    int width, height;
};

class BgColorCommand : public Command {
public:
    BgColorCommand(float r, float g, float b) : red(r), green(g), blue(b) {}

    void execute(Scene& s) override {
        s.setBgColor(glm::vec3{ red, green, blue });
    }
private:
    float red, green, blue;
};

class RayDepthCommand: public Command {
public: 
    RayDepthCommand(float d): depth(d) { }

    void execute(Scene& s) override {
        
    }
private:
    int depth;
};

class CameraPositionCommand : public Command {
public:
    CameraPositionCommand(glm::vec3 pos) : position(pos) {}

    void execute(Scene& s) override {
        s.initializeCamera(position);
    }
private:
    glm::vec3 position;
};

class CameraRightCommand : public Command {
public:
    CameraRightCommand(glm::vec3 d) : dir(d) {}

    void execute(Scene& s) override {
        s.camera.setRight(dir);
    }
private:
    glm::vec3 dir;
};

class CameraUpCommand : public Command {
public:
    CameraUpCommand(glm::vec3 d): dir(d) {}

    void execute(Scene& s) override {
        s.camera.setUp(dir);
    }
private:
    glm::vec3 dir;
};

class CameraForwardCommand : public Command {
public:
    CameraForwardCommand(glm::vec3 d) : dir(d) {}

    void execute(Scene& s) override {
        s.camera.setForward(dir);
    }
private:
    glm::vec3 dir;
};

class CameraFovXCommand : public Command {
public:
    CameraFovXCommand(float fov) : fovX(fov) {}

    void execute(Scene& s) override {
        s.camera.setFovX(fovX);
    }
private:
    float fovX;
};

class CommandFactory {
    using commandMapping_t = std::unordered_map<std::string, std::function<std::unique_ptr<Command>(const std::vector<float>&)>>;
public:
    static std::unique_ptr<Command> createCommand(const std::string& command, const std::vector<float>& args) {
        auto it = commandMap.find(command);
        if (it != commandMap.end()) {
            return it->second(args);
        }
        std::cout << "cannot parse " << command << std::endl;
        return nullptr; // or throw an exception for unknown command
    }

private:
    static commandMapping_t command_init() {
        commandMapping_t mapping = commandMapping_t();
        mapping["DIMENSIONS"] = [](const std::vector<float>& args) {
            return std::make_unique<DimensionsCommand>(args[0], args[1]);
        };
        mapping["RAY_DEPTH"] = [](const std::vector<float>& args) {
            return std::make_unique<RayDepthCommand>(args[0]);
        };
        mapping["BG_COLOR"] = [](const std::vector<float>& args) {
            return std::make_unique<BgColorCommand>(args[0], args[1], args[2]);
        };
        mapping["CAMERA_POSITION"] = [](const std::vector<float>& args) {
            return std::make_unique<CameraPositionCommand>(glm::vec3(args[0], args[1], args[2]));
        };
        mapping["CAMERA_RIGHT"] = [](const std::vector<float>& args) {
            return std::make_unique<CameraRightCommand>(glm::vec3(args[0], args[1], args[2]));
        };
        mapping["CAMERA_UP"] = [](const std::vector<float>& args) {
            return std::make_unique<CameraUpCommand>(glm::vec3(args[0], args[1], args[2]));
        };
        mapping["CAMERA_FORWARD"] = [](const std::vector<float>& args) {
            return std::make_unique<CameraForwardCommand>(glm::vec3(args[0], args[1], args[2]));
        };
        mapping["CAMERA_FOV_X"] = [](const std::vector<float>& args) {
            return std::make_unique<CameraFovXCommand>(args[0]);
        };
        return mapping;
    }

    static commandMapping_t commandMap;
};

CommandFactory::commandMapping_t CommandFactory::commandMap = CommandFactory::command_init();

struct DataType {
    std::unique_ptr<Command> command;
};