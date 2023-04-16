#pragma once
#include "reactphysics3d/reactphysics3d.h"
#include <glm/mat4x4.hpp>

rp3d::Matrix3x3 GetConjucate(rp3d::Quaternion q);
glm::mat4 TRS(rp3d::Vector3, rp3d::Quaternion, rp3d::Vector3);