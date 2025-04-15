#pragma once

#include "primitives/Primitive.h"
#include "scene/Scene.h"
#include <gtc/random.hpp>
#include <limits>
#include <optional>
#include <random>
#include <vector>

class ImageScene : public Scene {
public:
  ImageScene() { rand.seed(std::random_device{}()); }

  void render() override;

  std::optional<Intersection>
  findIntersection(Ray ray,
                   float maxDistance = std::numeric_limits<float>::max());

  glm::vec3 reflect(const glm::vec3 &pos, const glm::vec3 &normal, const Ray r);
  glm::vec3 collectColor(const Ray ray);

  Pixel getPixel(int x, int y) override { return pixels[y][x]; }

  void setDimensions(int width, int height) override {
    pixels.resize(height, std::vector<Pixel>(width, {0, 0, 0}));
    this->width = width;
    this->height = height;
    camera.setDimensions(glm::vec2{width, height});
  }

  void setBgColor(glm::vec3 color) override { bg_color = color; }

  void updateCameraFovY() override {
    camera.updateFovY((float)width / (float)height);
  }
  void setRayDepth(int depth) override { rayDepth = depth; }

  int getRayDepth() const override { return rayDepth; }

  int getWidth() const override { return width; }

  int getHeight() const override { return height; }

  ~ImageScene() override { pixels.clear(); }

private:
  int width, height, rayDepth;
  PixelData pixels;
  glm::vec3 bg_color;

  glm::uvec3 normal_to_rgb(glm::vec3 val) {
    glm::uvec3 res = glm::round(glm::clamp(val * 255.f, 0.f, 255.f));
    return res;
  }

  const glm::vec3 gamma = {1.f / 2.2f, 1.f / 2.2f, 1.f / 2.2f};
  const float shift = 1e-4;

  glm::vec3 aces_tonemap(const glm::vec3 &x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;

    return glm::clamp((x * (a * x + b)) / (x * (c * x + d) + e),
                      glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f));
  }

  std::minstd_rand rand;
  std::uniform_real_distribution<float> gen;
};