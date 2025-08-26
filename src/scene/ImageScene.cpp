#include "ImageScene.h"

glm::vec3 ImageScene::processPixel(const glm::ivec2 &pixel_position) {
  glm::vec3 color = {0.f, 0.f, 0.f};

  for (int ts = 0; ts < samples; ++ts) {
    Ray ray = camera.trace(pixel_position);
    ray.depth = rayDepth;
    color += collectColor(ray);
  }

  return color / static_cast<float>(samples);
}

void ImageScene::storePixelColor(const glm::ivec2 &pixel_position,
                                 const glm::vec3 &color) {
  glm::uvec3 new_color = normal_to_rgb(glm::pow(aces_tonemap(color), gamma));
  pixels[pixel_position.y][pixel_position.x] = new_color;
}

void ImageScene::render() {
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      glm::ivec2 pixel_position = {i, j};
      glm::vec3 color = processPixel(pixel_position);
      storePixelColor(pixel_position, color);
    }
  }
}

std::optional<Intersection> ImageScene::findIntersection(const Ray &ray,
                                                         float maxDistance) {
  Intersection res;
  res.dist = maxDistance;

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

glm::vec3 ImageScene::calculateReflectionDirection(const glm::vec3 &direction,
                                                   const glm::vec3 &normal) {
  return direction - 2.0f * normal * glm::dot(normal, direction);
}

glm::vec3 ImageScene::reflect(const glm::vec3 &pos, const glm::vec3 &normal,
                              const Ray &r) {
  glm::vec3 reflectedDirection =
      calculateReflectionDirection(r.direction, normal);
  Ray reflectedRay(pos, reflectedDirection);
  reflectedRay.depth = r.depth - 1;
  return collectColor(reflectedRay);
}

Ray ImageScene::createRayFromIntersection(const Ray& originalRay, const Intersection& intersection, const glm::vec3& direction) {
    return Ray(originalRay.start_position + originalRay.direction * intersection.dist + shift * direction, direction);
}

glm::vec3 ImageScene::sampleDirectLighting(const Ray& ray, const Intersection& intersection, const Primitive* primitive) {
    glm::vec3 directLighting = glm::vec3(0.0f);
    glm::vec3 hitPoint = ray.start_position + ray.direction * intersection.dist;
    
    // Sample emissive primitives
    for (const auto& emissivePrimitive : primitives) {
        // Skip non-emissive primitives
        if (emissivePrimitive->emission == glm::vec3(0.0f)) {
            continue;
        }
        
        // Skip self-emission
        if (emissivePrimitive == primitive) {
            continue;
        }
        
        // For now, we'll treat all emissive primitives as point lights
        glm::vec3 lightPoint = emissivePrimitive->position;
        glm::vec3 lightDir = glm::normalize(lightPoint - hitPoint);
        
        // Check if the light is visible
        Ray shadowRay(hitPoint + intersection.normal * shift, lightDir);
        std::optional<Intersection> shadowIntersection = findIntersection(shadowRay, glm::length(lightPoint - hitPoint));
        
        if (!shadowIntersection.has_value()) {
            // Calculate the BRDF value based on material type
            float brdf = 1.0f;
            if (primitive->material == Material::Diffuse) {
                brdf = glm::max(0.0f, glm::dot(intersection.normal, lightDir)) / M_PI;
            } else if (primitive->material == Material::Metallic) {
                glm::vec3 halfVector = glm::normalize(-ray.direction + lightDir);
                brdf = glm::max(0.0f, glm::dot(intersection.normal, lightDir)) * 
                       glm::max(0.0f, glm::dot(intersection.normal, halfVector));
            }
            
            // Calculate the light contribution
            float distanceSquared = glm::length(lightPoint - hitPoint) * glm::length(lightPoint - hitPoint);
            float lightArea = 1.0f; // Treat as point light for now
            
            directLighting += primitive->color * emissivePrimitive->emission * brdf * 
                            glm::max(0.0f, glm::dot(intersection.normal, lightDir)) /
                            (distanceSquared * lightArea);
        }
    }
    
    return directLighting;
}

glm::vec3 ImageScene::handleDiffuseMaterial(const Ray& ray, const Intersection& intersection, const Primitive* primitive) {
    // Add direct lighting contribution
    glm::vec3 directLighting = sampleDirectLighting(ray, intersection, primitive);
    
    // Continue with existing indirect lighting
    float r1 = gen(rand);
    float r2 = gen(rand);
    float phi = 2.0f * M_PI * r1;
    float cosTheta = std::sqrt(1.0f - r2);
    float sinTheta = std::sqrt(r2);
    
    glm::vec3 w = intersection.normal;
    glm::vec3 u = glm::normalize(glm::cross(
        (std::abs(w.x) > 0.1f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0)), w));
    glm::vec3 v = glm::cross(w, u);
    
    glm::vec3 reflectionDirection = glm::normalize(
        u * std::cos(phi) * sinTheta +
        v * std::sin(phi) * sinTheta +
        w * cosTheta
    );
    
    Ray reflection = createRayFromIntersection(ray, intersection, reflectionDirection);
    reflection.depth = ray.depth - 1;
    
    return primitive->emission + primitive->color * (directLighting + collectColor(reflection));
}

glm::vec3 ImageScene::handleDielectricMaterial(const Ray &ray,
                                               const Intersection &intersection,
                                               const Primitive *primitive) {
  float cosThetaIncident = -glm::dot(intersection.normal, ray.direction);
  float refractiveIndex1 = 1.0f;
  float refractiveIndex2 = primitive->ior;

  if (intersection.isInside) {
    std::swap(refractiveIndex1, refractiveIndex2);
  }

  float sinThetaRefracted = refractiveIndex1 / refractiveIndex2 *
                            std::sqrt(1 - cosThetaIncident * cosThetaIncident);

  if (sinThetaRefracted >= 1.0f) {
    glm::vec3 reflected_direction =
        calculateReflectionDirection(ray.direction, intersection.normal);
    glm::vec3 position = ray.start_position +
                         ray.direction * intersection.dist +
                         shift * reflected_direction;
    return reflect(position, intersection.normal, ray);
  }

  float r0 = std::pow((refractiveIndex1 - refractiveIndex2) /
                          (refractiveIndex1 + refractiveIndex2),
                      2.0f);
  float reflectionCoefficient =
      r0 + (1 - r0) * std::pow(1 - cosThetaIncident, 5.0f);
  float cosThetaRefracted =
      std::sqrt(1 - sinThetaRefracted * sinThetaRefracted);

  glm::vec3 refractedDirection =
      glm::normalize((refractiveIndex1 / refractiveIndex2) * ray.direction +
                     ((refractiveIndex1 / refractiveIndex2) * cosThetaIncident -
                      cosThetaRefracted) *
                         intersection.normal);

  if (gen(rand) < reflectionCoefficient) {
    glm::vec3 reflected_direction =
        calculateReflectionDirection(ray.direction, intersection.normal);
    glm::vec3 position = ray.start_position +
                         ray.direction * intersection.dist +
                         shift * reflected_direction;
    return reflect(position, intersection.normal, ray);
  }

    Ray refractedRay = createRayFromIntersection(ray, intersection, refractedDirection);
    refractedRay.depth = ray.depth - 1;
    auto refractedColor = collectColor(refractedRay);
    if (intersection.isInside && refractedColor != bg_color) {
        refractedColor *= primitive->color;
    }
    return refractedColor;
}

glm::vec3 ImageScene::handleMetallicMaterial(const Ray& ray, const Intersection& intersection, const Primitive* primitive) {
    // Add direct lighting contribution
    glm::vec3 directLighting = sampleDirectLighting(ray, intersection, primitive);
    
    // Continue with existing reflection
    glm::vec3 position = ray.start_position + ray.direction * intersection.dist + intersection.normal * shift;
    return primitive->emission + primitive->color * (directLighting + reflect(position, intersection.normal, ray));
}

glm::vec3 ImageScene::collectColor(const Ray &ray) {
  if (ray.depth <= 0) {
    return glm::vec3(0.f);
  }

  std::optional<Intersection> intersection = findIntersection(ray);
  if (!intersection.has_value()) {
    return bg_color;
  }

  auto primitive = intersection->primitive;

  switch (primitive->material) {
  case Material::Diffuse:
    return handleDiffuseMaterial(ray, intersection.value(), primitive);
  case Material::Dielectric:
    return handleDielectricMaterial(ray, intersection.value(), primitive);
  case Material::Metallic:
    return handleMetallicMaterial(ray, intersection.value(), primitive);
  }
}