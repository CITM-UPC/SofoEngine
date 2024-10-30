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

    // Limitar el pitch para evitar voltear la cámara
    if (pitch > 1.5) pitch = 1.5;
    if (pitch < -1.5) pitch = -1.5;

    // Calcula la dirección en base a yaw y pitch
    vec3 direction;
    direction.x = cos(pitch) * sin(yaw);
    direction.y = sin(pitch);
    direction.z = cos(pitch) * cos(yaw);

    // Calcula los ejes locales a partir de la dirección
    vec3 right = glm::normalize(glm::cross(vec3(0.0f, 1.0f, 0.0f), direction));
    vec3 up = glm::normalize(glm::cross(direction, right));

    // Crea la matriz de rotación usando yaw y pitch
    mat4 rotation = mat4(1.0f);
    rotation[0] = vec4(right, 0.0f);         // Eje X
    rotation[1] = vec4(up, 0.0f);            // Eje Y
    rotation[2] = vec4(-direction, 0.0f);    // Eje Z invertido

    // Aplica la matriz de rotación a la matriz de traslación
    _mat = glm::translate(mat4(1.0f), _pos) * rotation;
}

void Transform::lookAt(double yawOffset, double pitchOffset) {
    // Limita el pitch para evitar que la cámara se voltee completamente
    pitch += pitchOffset;
    if (pitch > 1.5) pitch = 1.5;
    if (pitch < -1.5) pitch = -1.5;

    // Ajusta yaw sin acumulación infinita
    yaw += yawOffset;
    if (yaw > glm::pi<double>()) yaw -= glm::two_pi<double>();
    if (yaw < -glm::pi<double>()) yaw += glm::two_pi<double>();

    // Calcula la dirección en base a yaw y pitch
    vec3 direction;
    direction.x = cos(pitch) * sin(yaw);
    direction.y = sin(pitch);
    direction.z = cos(pitch) * cos(yaw);

    // Calcula la matriz de vista usando lookAt
    vec3 target = _pos + direction;  // Objetivo de la cámara
    vec3 up = vec3(0.0f, 1.0f, 0.0f); // Vector "arriba" global

    _mat = glm::lookAt(_pos, target, up); // Genera la matriz de vista usando la posición actual y el objetivo
}
