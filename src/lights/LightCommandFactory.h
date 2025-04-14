#pragma once

#include "LightSource.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

class LightPositionCommand : public LightSourceCommand
{
public:
    LightPositionCommand(glm::vec3 pos) : position(pos) {}

    void execute(LightSource *lightSource) override
    {
        lightSource->position = position;
        lightSource->kind = LightSourceKind::Positional;
    }

private:
    glm::vec3 position;
};

class LightDirectionCommand : public LightSourceCommand
{
public:
    LightDirectionCommand(glm::vec3 dir) : direction(dir) {}

    void execute(LightSource *lightSource) override
    {
        lightSource->direction = direction;
        lightSource->kind = LightSourceKind::Directional;
    }

private:
    glm::vec3 direction;
};

class LightAttenuationCommand : public LightSourceCommand
{
public:
    LightAttenuationCommand(glm::vec3 a) : attenuation(a) {}

    void execute(LightSource *lightSource) override
    {
        lightSource->attenuation = attenuation;
        lightSource->kind = LightSourceKind::Positional;
    }

private:
    glm::vec3 attenuation;

};

class LightIntensityCommand : public LightSourceCommand
{
public:
    LightIntensityCommand(glm::vec3 c) : color(c) {}

    void execute(LightSource *lightSource) override
    {
        lightSource->intensity = color;
    }

private:
    glm::vec3 color;

};

class LightCommandFactory
{
public:
    using commandMapping_t = std::unordered_map<std::string, std::function<LightSourceCommand*(const std::vector<float>)>>;

    static LightSourceCommand *createCommand(const std::string &command, const std::vector<float> &args)
    {
        auto it = commandMap.find(command);
        if (it != commandMap.end())
        {
            return it->second(args);
        }
        return nullptr;
    }

private:
    static commandMapping_t command_init()
    {
        commandMapping_t mapping;
        mapping["LIGHT_POSITION"] = [](const std::vector<float> &args)
        {
            return new LightPositionCommand(glm::vec3(args[0], args[1], args[2]));
        };
        mapping["LIGHT_DIRECTION"] = [](const std::vector<float> &args) 
        {
            return new LightDirectionCommand(glm::vec3(args[0], args[1], args[2]));
        };
        mapping["LIGHT_INTENSITY"] = [](const std::vector<float> &args)
        {
            return new LightIntensityCommand(glm::vec3(args[0], args[1], args[2]));
        };
        mapping["LIGHT_ATTENUATION"] = [](const std::vector<float> &args)
        {
            return new LightAttenuationCommand(glm::vec3(args[0], args[1], args[2]));
        };
        return mapping;
    }

    static commandMapping_t commandMap;
};

LightCommandFactory::commandMapping_t LightCommandFactory::commandMap = LightCommandFactory::command_init();