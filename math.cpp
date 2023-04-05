#include "math.h"
#include <glm/vec4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

rp3d::Matrix3x3 GetConjucate(rp3d::Quaternion q)
{
	rp3d::Matrix3x3 m;
	m[0][0] = q.w;
	m[0][1] = -q.z;
	m[0][2] = q.y;
	m[1][0] = q.z;
	m[1][1] = q.w;
	m[1][2] = -q.x;
	m[2][0] = -q.y;
	m[2][1] = q.x;
	m[2][2] = q.w;
	return m;
}

// Convert position, rotation and scale to a 4x4 matrix
glm::mat4 TRS(rp3d::Vector3 position, rp3d::Quaternion rotation, rp3d::Vector3 scale)
{
	glm::mat4 m = glm::mat4(1.0f);
	m = glm::translate(m, glm::vec3(position.x, position.y, position.z));
	m *= glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
	m = glm::scale(m, glm::vec3(scale.x, scale.y, scale.z));
	return m;
}