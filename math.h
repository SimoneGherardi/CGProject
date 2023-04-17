#pragma once

#include "reactphysics3d/reactphysics3d.h"
#include <glm/mat4x4.hpp>

#define M_PI 3.14159265358979323846

rp3d::Matrix3x3 GetConjucate(rp3d::Quaternion q);
glm::mat4 TRS(rp3d::Vector3, rp3d::Quaternion, rp3d::Vector3);