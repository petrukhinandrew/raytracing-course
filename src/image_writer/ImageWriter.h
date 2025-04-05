#pragma once

#include <string>
#include "scene/Scene.h"

class ImageWriter
{
public:
    virtual void write(const std::string &filename, Scene &scene) = 0;
    virtual ~ImageWriter() = default;
};