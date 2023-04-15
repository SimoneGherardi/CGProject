#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <corecrt_math_defines.h>

struct AnglesQuaternion {
    float Quat[4];

    AnglesQuaternion(rp3d::Quaternion quaternion) {
		Quat[0] = quaternion.x;
		Quat[1] = quaternion.y;
		Quat[2] = quaternion.z;
		Quat[3] = quaternion.w;
	}
    AnglesQuaternion(float x, float y, float z, float w) {
		Quat[0] = x;
		Quat[1] = y;
		Quat[2] = z;
		Quat[3] = w;
	}
    AnglesQuaternion() {
		Quat[0] = 0;
		Quat[1] = 0;
		Quat[2] = 0;
		Quat[3] = 1;
    }
};

struct AnglesEulerAngles {
    float XYZ[3];

public:
    AnglesEulerAngles() {
		XYZ[0] = 0;
		XYZ[1] = 0;
		XYZ[2] = 0;
	}
    AnglesEulerAngles(float x, float y, float z) {
		XYZ[0] = x;
		XYZ[1] = y;
		XYZ[2] = z;
	}
	void toRadians() {
		XYZ[0] = XYZ[0] * M_PI / 180;
		XYZ[1] = XYZ[1] * M_PI / 180;
		XYZ[2] = XYZ[2] * M_PI / 180;
	}
	void toDegrees() {
		XYZ[0] = XYZ[0] * 180 / M_PI;
		XYZ[1] = XYZ[1] * 180 / M_PI;
		XYZ[2] = XYZ[2] * 180 / M_PI;
	}
};

// this implementation assumes normalized AnglesQuaternion
// converts to Euler angles in 3-2-1 sequence
AnglesEulerAngles ToAnglesEulerAngles(AnglesQuaternion q) {
    AnglesEulerAngles angles;

    // RPY[0] (Quat[0]-axis rotation)
    float sinr_cosp = 2 * (q.Quat[3] * q.Quat[0] + q.Quat[1] * q.Quat[2]);
    float cosr_cosp = 1 - 2 * (q.Quat[0] * q.Quat[0] + q.Quat[1] * q.Quat[1]);
    angles.XYZ[0] = std::atan2(sinr_cosp, cosr_cosp);

    // RPY[1] (Quat[1]-axis rotation)
    float sinp = std::sqrt(1 + 2 * (q.Quat[3] * q.Quat[1] - q.Quat[0] * q.Quat[2]));
    float cosp = std::sqrt(1 - 2 * (q.Quat[3] * q.Quat[1] - q.Quat[0] * q.Quat[2]));
    angles.XYZ[1] = 2 * std::atan2(sinp, cosp) - M_PI / 2;

    // RPY[2] (Quat[2]-axis rotation)
    float siny_cosp = 2 * (q.Quat[3] * q.Quat[2] + q.Quat[0] * q.Quat[1]);
    float cosy_cosp = 1 - 2 * (q.Quat[1] * q.Quat[1] + q.Quat[2] * q.Quat[2]);
    angles.XYZ[2] = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}

