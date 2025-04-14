#pragma once 

#include "glm.hpp"

typedef struct {
    glm::vec3 normal, color;
    float dist;
    bool isInside = false;
} Intersection;