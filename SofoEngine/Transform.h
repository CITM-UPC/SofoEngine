#pragma once

#include "types.h"

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

	// Variables para almacenar los ángulos acumulativos de rotación
	double yaw = 0.0;    // Rotación acumulada en el eje Y
	double pitch = 0.0;  // Rotación acumulada en el eje X

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

	Transform operator*(const mat4& other) { return Transform(_mat * other); }
	Transform operator*(const Transform& other) { return Transform(_mat * other._mat); }
};

inline Transform operator*(const mat4& m, const Transform& t) { return Transform(m * t.mat()); }
