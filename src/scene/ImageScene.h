#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <limits>
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
                ray.depth = rayDepth;

                glm::vec3 color = bg_color;

                if (auto intersection = findIntersection(ray); intersection.has_value())
                {
                    color = intersection.value().color;
                }

                glm::uvec3 new_color = normal_to_rgb(glm::pow(aces_tonemap(color), gamma));
                pixels[pixel_position.y][pixel_position.x] = new_color;
            }
        }
    }
    std::optional<Intersection> findIntersection(Ray ray, float maxDistance = std::numeric_limits<float>::max(), bool ignoreLight = false)
    {
        if (ray.depth <= 0)
        {
            Intersection noIntersection{
                .normal = {1.0f, 0.0f, 0.0f},
                .color = {0.0f, 0.0f, 0.0f},
                .dist = maxDistance,
                .isInside = false,
            };

            return std::make_optional(noIntersection);
        }

        ray.depth -= 1;

        Intersection closestIntersection{
            .color = {0.0f, 0.0f, 0.0f},
            .normal = {0.0f, 0.0f, 0.0f},
            .dist = maxDistance,
            .isInside = false,
        };

        bool hasIntersection = false;

        for (const auto &primitive : primitives)
        {
            std::optional<Intersection> currentIntersection = primitive->intersectWith(ray);
            if (!currentIntersection.has_value())
            {
                continue;
            }

            if (currentIntersection->dist > closestIntersection.dist)
            {
                continue;
            }

            hasIntersection = true;
            closestIntersection = currentIntersection.value();
            closestIntersection.color = {0.0f, 0.0f, 0.0f};

            if (ignoreLight)
            {
                continue;
            }

            glm::vec3 intersectionPosition = ray.start_position + ray.direction * currentIntersection->dist;

            switch (primitive->material)
            {
            case Material::Diffuse:
                closestIntersection.color = primitive->color * ambientLight;
                break;

            case Material::Dielectric:
            {
                float cosThetaIncident = -glm::dot(currentIntersection->normal, ray.direction);
                float refractiveIndex1 = 1.0f;
                float refractiveIndex2 = primitive->ior;

                if (currentIntersection->isInside)
                {
                    std::swap(refractiveIndex1, refractiveIndex2);
                }

                float sinThetaRefracted = refractiveIndex1 / refractiveIndex2 * std::sqrt(1 - cosThetaIncident * cosThetaIncident);
                float reflectionCoefficient;

                if (sinThetaRefracted >= 1.0f)
                {
                    reflectionCoefficient = 1.0f;
                }
                else
                {
                    float r0 = std::pow((refractiveIndex1 - refractiveIndex2) / (refractiveIndex1 + refractiveIndex2), 2.0f);
                    reflectionCoefficient = r0 + (1 - r0) * std::pow(1 - cosThetaIncident, 5.0f);
                }

                glm::vec3 reflectedDirection = glm::normalize(ray.direction - 2.0f * currentIntersection->normal * dot(currentIntersection->normal, ray.direction));
                Ray reflectedRay(intersectionPosition + reflectedDirection * shift, reflectedDirection);
                reflectedRay.depth = ray.depth;

                std::optional<Intersection> reflectedIntersection = findIntersection(reflectedRay, maxDistance);
                glm::vec3 reflectedColor;

                if (!reflectedIntersection.has_value())
                {
                    reflectedColor = reflectionCoefficient * bg_color;
                }
                else
                {
                    reflectedColor = reflectionCoefficient * reflectedIntersection->color;
                }
                closestIntersection.color += reflectedColor;

                if (sinThetaRefracted >= 1.0f)
                {
                    continue;
                }

                float cosThetaRefracted = std::sqrt(1 - sinThetaRefracted * sinThetaRefracted);
                glm::vec3 refractedDirection = glm::normalize(
                    (refractiveIndex1 / refractiveIndex2) * ray.direction +
                    ((refractiveIndex1 / refractiveIndex2) * cosThetaIncident - cosThetaRefracted) * currentIntersection->normal);
                Ray refractedRay(intersectionPosition + refractedDirection * shift, refractedDirection);
                refractedRay.depth = ray.depth;

                std::optional<Intersection> refractedIntersection = findIntersection(refractedRay, maxDistance);
                glm::vec3 refractedColor;

                if (!refractedIntersection.has_value())
                {
                    refractedColor = (1 - reflectionCoefficient) * bg_color;
                }
                else
                {
                    refractedColor = (1 - reflectionCoefficient) * refractedIntersection->color;
                    if (!currentIntersection->isInside)
                    {
                        refractedColor *= primitive->color;
                    }
                }
                closestIntersection.color += refractedColor;
                continue;
            }

            case Material::Metallic:
            {
                glm::vec3 reflectedDirection = glm::normalize(ray.direction - 2.0f * currentIntersection->normal * dot(currentIntersection->normal, ray.direction));
                Ray reflectedRay(intersectionPosition + reflectedDirection * shift, reflectedDirection);
                reflectedRay.depth = ray.depth;

                std::optional<Intersection> metallicReflection = findIntersection(reflectedRay, maxDistance);
                if (!metallicReflection.has_value())
                {
                    closestIntersection.color += primitive->color * bg_color;
                }
                else
                {
                    closestIntersection.color += primitive->color * metallicReflection->color;
                }
                continue;
            }
            }

            for (const auto &lightSource : lightSources)
            {
                glm::vec3 lightDirection;
                float lightDistance = maxDistance;

                if (lightSource->kind == LightSourceKind::Directional)
                {
                    lightDirection = lightSource->direction;
                }
                else
                {
                    auto lightRay = lightSource->position - intersectionPosition;
                    lightDistance = glm::length(lightRay);
                    lightDirection = glm::normalize(lightRay);
                }

                Ray lightRay(intersectionPosition + lightDirection * shift, lightDirection);
                std::optional<Intersection> lightIntersection = findIntersection(lightRay, lightDistance, true);

                if (lightIntersection.has_value())
                {
                    continue;
                }

                glm::vec3 lightColor = lightSource->intensity;
                if (lightSource->kind == LightSourceKind::Positional)
                {
                    lightColor *= (1.0f / (lightSource->attenuation.x + lightSource->attenuation.y * lightDistance + lightSource->attenuation.z * lightDistance * lightDistance));
                }

                lightColor *= std::max(dot(lightDirection, currentIntersection->normal), 0.0f);
                closestIntersection.color += primitive->color * lightColor;
            }
        }

        if (hasIntersection)
        {
            return std::make_optional(closestIntersection);
        }
        else
        {
            return std::nullopt;
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
    void setRayDepth(int depth) override
    {
        rayDepth = depth;
    }

    int getRayDepth() const override
    {
        return rayDepth;
    }

    int getWidth() const override
    {
        return width;
    }

    int getHeight() const override
    {
        return height;
    }

    ~ImageScene() override
    {
        pixels.clear();
    }

private:
    int width, height, rayDepth;
    PixelData pixels;
    Scene::Color_t bg_color;

    glm::uvec3 normal_to_rgb(glm::vec3 val)
    {
        glm::uvec3 res = glm::round(glm::clamp(val * 255.f, 0.f, 255.f));
        return res;
    }

    const glm::vec3 gamma = {1.f / 2.2f, 1.f / 2.2f, 1.f / 2.2f};
    const float shift = 1e-4;

    glm::vec3 aces_tonemap(const glm::vec3 &x)
    {
        const float a = 2.51f;
        const float b = 0.03f;
        const float c = 2.43f;
        const float d = 0.59f;
        const float e = 0.14f;

        return glm::clamp(
            (x * (a * x + b)) /
                (x * (c * x + d) + e),
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(1.f, 1.f, 1.f));
    }
};