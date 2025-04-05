#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include "scene/Scene.h"
#include "parser/Command.h"

class ImageScene : public Scene
{
public:
    ImageScene() {}

    void render() override
    {
        for (int j = 0; j < height; ++j)
        {
            for (int i = 0; i < width; ++i)
            {
                glm::ivec2 pixel_position = {i, j};
                Ray ray = camera.trace(pixel_position);

                glm::vec3 color = bg_color;
                double distance = 1e9;

                for (const auto &primitive : primitives)
                {
                    auto intersection = primitive->isIntersectedBy(ray);
                    if (intersection.has_value() && intersection.value() < distance)
                    {
                        distance = intersection.value();
                        color = primitive->color;
                    }
                }

                glm::uvec3 new_color = normal_to_rgb(color);
                pixels[pixel_position.y][pixel_position.x] = new_color;
            }
        }
    }
    Pixel getPixel(int x, int y) override
    {
        return pixels[y][x];
    }

    void setDimensions(int width, int height) override
    {
        pixels.resize(height, std::vector<Pixel>(width, {0, 0, 0}));
        this->width = width;
        this->height = height;
        camera.setDimensions(glm::vec2{width, height});
    }

    void setBgColor(Scene::Color_t color) override
    {
        bg_color = std::move(color);
    }

    void updateCameraFovY() override
    {
        camera.updateFovY((float)width / (float)height);
    }

    int getWidth() const override
    {
        return width;
    }

    int getHeight() const override
    {
        return height;
    }

    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            pixels[y][x] = {r, g, b};
        }
    }
    ~ImageScene() override
    {
        pixels.clear();
    }

private:
    int width, height;
    PixelData pixels;
    Scene::Color_t bg_color;

    uint8_t normal_to_rgb(float val)
    {
        return std::round(glm::clamp(val * 255.0, 0.0, 255.0));
    }

    glm::uvec3 normal_to_rgb(glm::vec3 val)
    {
        glm::vec3 result{};
        result.x = normal_to_rgb(val.x);
        result.y = normal_to_rgb(val.y);
        result.z = normal_to_rgb(val.z);
        return result;
    }
};