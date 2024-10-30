#pragma once

#include "types.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

class Transform {

	union {
		mat4 _mat = mat4(1.0);
		struct {
			vec3 _left; mat4::value_type _left_w;
			vec3 _up; mat4::value_type _up_w;
			vec3 _fwd; mat4::value_type _fwd_w;
			vec3 _pos; mat4::value_type _pos_w;
		};
	};

	double yaw = 0.0;
	double pitch = 0.0;

public:
	const auto& mat() const { return _mat; }
	const auto& left() const { return _left; }
	const auto& up() const { return _up; }
	const auto& fwd() const { return _fwd; }
	const auto& pos() const { return _pos; }
	auto& pos() { return _pos; }

	const auto* data() const { return &_mat[0][0]; }

	Transform() = default;
	Transform(const mat4& mat) : _mat(mat) {}


	void translate(const vec3& v);
	void rotate(double rads, const vec3& v);
	void rotateYawPitch(double deltaYaw, double deltaPitch);

	void lookAt(double yawOffset, double pitchOffset);

	glm::vec3 scale() const {
		return glm::vec3(glm::length(_left), glm::length(_up), glm::length(_fwd));
	}

	glm::quat rotation() const {
		glm::mat3 rotationMatrix(
			glm::normalize(_left),
			glm::normalize(_up),
			glm::normalize(_fwd)
		);
		return glm::quat_cast(rotationMatrix);
	}

	Transform operator*(const mat4& other) { return Transform(_mat * other); }
	Transform operator*(const Transform& other) { return Transform(_mat * other._mat); }
};

inline Transform operator*(const mat4& m, const Transform& t) { return Transform(m * t.mat()); }
