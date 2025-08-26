#pragma once

#include "camera/Camera.h"
#include "lights/LightSource.h"
#include "primitives/Box.h"
#include "primitives/Ellipsoid.h"
#include "primitives/Plane.h"
#include "primitives/Primitive.h"
#include <vector>

class Scene {
    public:
    using Pixel     = glm::uvec3;
    using PixelData = std::vector<std::vector<Pixel>>;

    virtual void setDimensions (int width, int height) = 0;
    virtual void setBgColor (glm::vec3 color)          = 0;
    virtual int getWidth () const                      = 0;
    virtual int getHeight () const                     = 0;
    virtual void setRayDepth (int depth)               = 0;
    virtual int getRayDepth () const                   = 0;

    virtual Pixel getPixel (int x, int y) = 0;
    virtual void render ()                = 0;

    int samples = 1;

    Camera camera;
    virtual void updateCameraFovY () = 0;

    std::vector<LightSource*> lightSources;
    LightSource* currentLight () const {
        return lightSources.back ();
    }

    std::vector<Primitive*> primitives;
    Primitive* currentPrimitive () const {
        return primitives.back ();
    }

    glm::vec3 ambientLight = { 0.f, 0.f, 0.f };

    void initializeCamera (const glm::vec3& position) {
        camera.setPosition (position);
    }

    void addLightSource () {
        lightSources.push_back (new LightSource ());
    }

    void addEllipsoid () {
        primitives.push_back (new Ellipsoid ());
    }

    void addBox () {
        primitives.push_back (new Box ());
    }
    void addPlane () {
        primitives.push_back (new Plane ());
    }
    virtual ~Scene () = default;
};