#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

void Transform::translate(const vec3& v) { 
	_mat = glm::translate(_mat,v);
}

void Transform::rotate(double rads, const vec3& v) {
	_mat = glm::rotate(_mat,rads, v);
}

void Transform::rotateYawPitch(double deltaYaw, double deltaPitch) {
    yaw += deltaYaw;
    pitch += deltaPitch;

    if (pitch > 1.5) pitch = 1.5;
    if (pitch < -1.5) pitch = -1.5;

    vec3 direction;
    direction.x = cos(pitch) * sin(yaw);
    direction.y = sin(pitch);
    direction.z = cos(pitch) * cos(yaw);

    vec3 right = glm::normalize(glm::cross(vec3(0.0f, 1.0f, 0.0f), direction));
    vec3 up = glm::normalize(glm::cross(direction, right));

    mat4 rotation = mat4(1.0f);
    rotation[0] = vec4(right, 0.0f);     
    rotation[1] = vec4(up, 0.0f);        
    rotation[2] = vec4(-direction, 0.0f);

    _mat = glm::translate(mat4(1.0f), _pos) * rotation;
}

void Transform::lookAt(double yawOffset, double pitchOffset) {
    pitch += pitchOffset;
    if (pitch > 1.5) pitch = 1.5;
    if (pitch < -1.5) pitch = -1.5;

    yaw += yawOffset;
    if (yaw > glm::pi<double>()) yaw -= glm::two_pi<double>();
    if (yaw < -glm::pi<double>()) yaw += glm::two_pi<double>();

    vec3 direction;
    direction.x = cos(pitch) * sin(yaw);
    direction.y = sin(pitch);
    direction.z = cos(pitch) * cos(yaw);

    vec3 target = _pos + direction;  
    vec3 up = vec3(0.0f, 1.0f, 0.0f);

    _mat = glm::lookAt(_pos, target, up);
}
