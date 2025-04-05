#pragma once

#include <vector>
#include <cstdint>
#include "glm.hpp"
#include "camera/Camera.h"
#include "primitives/Primitive.h"
#include "primitives/Box.h"
#include "primitives/Plane.h"
#include "primitives/Ellipsoid.h"

class Scene
{
public:
    using Color_t = glm::vec3;
    using Pixel = glm::uvec3;
    using PixelData = std::vector<std::vector<Pixel>>;

    virtual void setDimensions(int width, int height) = 0;
    virtual void setBgColor(Color_t color) = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    virtual Pixel getPixel(int x, int y) = 0;
    virtual void render() = 0;

    Camera camera;
    virtual void updateCameraFovY() = 0;

    std::vector<Primitive*> primitives;
    Primitive* currentPrimitive;

    void initializeCamera(const glm::vec3 &position)
    {
        camera.setPosition(position);
    }

    void addEllipsoid()
    {
        currentPrimitive = new Ellipsoid();
        primitives.push_back(currentPrimitive);
    }

    void addBox()
    {
        currentPrimitive = new Box();
        primitives.push_back(currentPrimitive);
    }
    void addPlane()
    {
        currentPrimitive = new Plane();
        primitives.push_back(currentPrimitive);
    }
    virtual ~Scene() = default;
};