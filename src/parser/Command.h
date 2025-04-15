#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <scene/Scene.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Command {
    public:
    virtual void execute (Scene& s) {
    }
    virtual ~Command () = default;
};

class DimensionsCommand : public Command {
    public:
    DimensionsCommand (float w, float h) : width (w), height (h) {
    }

    void execute (Scene& s) override {
        s.setDimensions (width, height);
    }

    private:
    int width, height;
};

class BgColorCommand : public Command {
    public:
    BgColorCommand (const glm::vec3 c) : color (c) {
    }

    void execute (Scene& s) override {
        s.setBgColor (color);
    }

    private:
    glm::vec3 color;
};

class RayDepthCommand : public Command {
    public:
    RayDepthCommand (int d) : depth (d) {
    }

    void execute (Scene& s) override {
        s.setRayDepth (depth);
    }

    private:
    int depth;
};

class SetSamplesCommand : public Command {
    public:
    SetSamplesCommand (int s) : samples (s) {
    }

    void execute (Scene& s) override {
        s.samples = samples;
    }

    private:
    int samples;
};


class AmbientLightCommand : public Command {
    public:
    AmbientLightCommand (const glm::vec3& c) : color (c) {
    }

    void execute (Scene& s) override {
        s.ambientLight = color;
    }

    private:
    glm::vec3 color;
};

class CameraPositionCommand : public Command {
    public:
    CameraPositionCommand (const glm::vec3& pos) : position (pos) {
    }

    void execute (Scene& s) override {
        s.initializeCamera (position);
    }

    private:
    glm::vec3 position;
};

class CameraRightCommand : public Command {
    public:
    CameraRightCommand (const glm::vec3& d) : dir (d) {
    }

    void execute (Scene& s) override {
        s.camera.setRight (dir);
    }

    private:
    glm::vec3 dir;
};

class CameraUpCommand : public Command {
    public:
    CameraUpCommand (const glm::vec3& d) : dir (d) {
    }

    void execute (Scene& s) override {
        s.camera.setUp (dir);
    }

    private:
    glm::vec3 dir;
};

class CameraForwardCommand : public Command {
    public:
    CameraForwardCommand (const glm::vec3& d) : dir (d) {
    }

    void execute (Scene& s) override {
        s.camera.setForward (dir);
    }

    private:
    glm::vec3 dir;
};

class CameraFovXCommand : public Command {
    public:
    CameraFovXCommand (float fov) : fovX (fov) {
    }

    void execute (Scene& s) override {
        s.camera.setFovX (fovX);
    }

    private:
    float fovX;
};

class CommandFactory {
    using commandMapping_t =
    std::unordered_map<std::string, std::function<Command*(const std::vector<float>&)>>;

    public:
    static Command*
    createCommand (const std::string& command, const std::vector<float>& args) {
        auto it = commandMap.find (command);
        if (it != commandMap.end ()) {
            return it->second (args);
        }
        // std::cout << "cannot parse " << command << std::endl;
        return nullptr;
    }

    private:
    static commandMapping_t command_init () {
        commandMapping_t mapping = commandMapping_t ();
        mapping["DIMENSIONS"]    = [] (const std::vector<float>& args) {
            return new DimensionsCommand (args[0], args[1]);
        };
        mapping["SAMPLES"] = [] (const std::vector<float>& args) {
            return new SetSamplesCommand (args[0]);
        };
        mapping["RAY_DEPTH"] = [] (const std::vector<float>& args) {
            return new RayDepthCommand (static_cast<int> (args[0]));
        };
        mapping["AMBIENT_LIGHT"] = [] (const std::vector<float>& args) {
            return new AmbientLightCommand (glm::vec3 (args[0], args[1], args[2]));
        };
        mapping["BG_COLOR"] = [] (const std::vector<float>& args) {
            return new BgColorCommand (glm::vec3 (args[0], args[1], args[2]));
        };
        mapping["CAMERA_POSITION"] = [] (const std::vector<float>& args) {
            return new CameraPositionCommand (glm::vec3 (args[0], args[1], args[2]));
        };
        mapping["CAMERA_RIGHT"] = [] (const std::vector<float>& args) {
            return new CameraRightCommand (glm::vec3 (args[0], args[1], args[2]));
        };
        mapping["CAMERA_UP"] = [] (const std::vector<float>& args) {
            return new CameraUpCommand (glm::vec3 (args[0], args[1], args[2]));
        };
        mapping["CAMERA_FORWARD"] = [] (const std::vector<float>& args) {
            return new CameraForwardCommand (glm::vec3 (args[0], args[1], args[2]));
        };
        mapping["CAMERA_FOV_X"] = [] (const std::vector<float>& args) {
            return new CameraFovXCommand (args[0]);
        };
        return mapping;
    }

    static commandMapping_t commandMap;
};

CommandFactory::commandMapping_t CommandFactory::commandMap =
CommandFactory::command_init ();

struct DataType {
    std::unique_ptr<Command> command;
};