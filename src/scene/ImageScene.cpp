#include "ImageScene.h"

void ImageScene::render() {
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {

      glm::ivec2 pixel_position = {i, j};
      glm::vec3 color = {0.f, 0.f, 0.f};

      for (int ts = 0; ts < samples; ++ts) {
        Ray ray = camera.trace(pixel_position);
        ray.depth = rayDepth;
        color += collectColor(ray);
      }
      color /= (float)samples;
      glm::uvec3 new_color =
          normal_to_rgb(glm::pow(aces_tonemap(color), gamma));
      pixels[pixel_position.y][pixel_position.x] = new_color;
    }
  }
}

std::optional<Intersection> ImageScene::findIntersection(Ray ray,
                                                         float maxDistance) {
  Intersection res;

  for (const auto &primitive : primitives) {
    std::optional<Intersection> cur = primitive->intersectWith(ray);
    if (!cur.has_value() || cur->dist > res.dist) {
      continue;
    }
    res = cur.value();
    res.primitive = primitive;
  }
  return res.primitive == nullptr ? std::nullopt : std::make_optional(res);
}

glm::vec3 ImageScene::reflect(const glm::vec3 &pos, const glm::vec3 &normal,
                              const Ray r) {
  Ray reflected_ray(pos, r.direction -
                             2.0f * normal * glm::dot(normal, r.direction));
  reflected_ray.depth = r.depth - 1;
  return collectColor(reflected_ray);
}

glm::vec3 ImageScene::collectColor(const Ray ray) {
  if (ray.depth <= 0) {
    return glm::vec3(0.f);
  }

  std::optional<Intersection> intersection = findIntersection(ray);
  if (!intersection.has_value()) {
    return bg_color;
  }

  auto primitive = intersection->primitive;
  switch (primitive->material) {
  case Material::Diffuse: {
    auto reflectionDirection = glm::sphericalRand(1.f);
    Ray reflection(ray.start_position + ray.direction * intersection->dist +
                       shift * intersection->normal,
                   reflectionDirection);
    reflection.depth = ray.depth - 1;
    return primitive->emission +
           collectColor(reflection) * primitive->color * 2.f *
               dot(reflectionDirection, intersection->normal);
  }

  case Material::Dielectric: {
    float cosThetaIncident = -glm::dot(intersection->normal, ray.direction);
    float refractiveIndex1 = 1.0f;
    float refractiveIndex2 = primitive->ior;

    if (intersection->isInside) {
      std::swap(refractiveIndex1, refractiveIndex2);
    }

    float sinThetaRefracted =
        refractiveIndex1 / refractiveIndex2 *
        std::sqrt(1 - cosThetaIncident * cosThetaIncident);
    float reflectionCoefficient;

    if (sinThetaRefracted >= 1.0f) {
      glm::vec3 reflected_direction =
          ray.direction - 2.0f * intersection->normal *
                              glm::dot(intersection->normal, ray.direction);
      glm::vec3 position = ray.start_position +
                           ray.direction * intersection->dist +
                           shift * reflected_direction;
      return reflect(position, intersection->normal, ray);
    }
    float r0 = std::pow((refractiveIndex1 - refractiveIndex2) /
                            (refractiveIndex1 + refractiveIndex2),
                        2.0f);
    reflectionCoefficient =
        r0 + (1 - r0) * std::pow(1 - cosThetaIncident, 5.0f);
    float cosThetaRefracted =
        std::sqrt(1 - sinThetaRefracted * sinThetaRefracted);
    glm::vec3 refractedDirection = glm::normalize(
        (refractiveIndex1 / refractiveIndex2) * ray.direction +
        ((refractiveIndex1 / refractiveIndex2) * cosThetaIncident -
         cosThetaRefracted) *
            intersection->normal);

    if (gen(rand) < reflectionCoefficient) {
      glm::vec3 reflected_direction =
          ray.direction - 2.0f * intersection->normal *
                              glm::dot(intersection->normal, ray.direction);
      glm::vec3 position = ray.start_position +
                           ray.direction * intersection->dist +
                           shift * reflected_direction;
      return reflect(position, intersection->normal, ray);
    }

    Ray refractedRay(ray.start_position + ray.direction * intersection->dist -
                         shift * intersection->normal,
                     refractedDirection);
    refractedRay.depth = ray.depth - 1;
    auto refractedColor = collectColor(refractedRay);
    if (intersection->isInside && refractedColor != bg_color) {
      refractedColor *= primitive->color;
    }
    return refractedColor;
  }

  case Material::Metallic: {
    glm::vec3 position = ray.start_position +
                         ray.direction * intersection->dist +
                         intersection->normal * shift;
    return primitive->emission +
           primitive->color * reflect(position, intersection->normal, ray);
  }
  }
}