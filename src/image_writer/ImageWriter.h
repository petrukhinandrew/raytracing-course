#pragma once

#include "scene/Scene.h"
#include <string>

class ImageWriter {
    public:
    virtual void write (const std::string& filename, Scene& scene) = 0;
    virtual ~ImageWriter ()                                        = default;
};