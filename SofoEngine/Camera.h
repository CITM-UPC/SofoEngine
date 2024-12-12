#pragma once

#include "Transform.h"
#include "Component.h"
#include "types.h"

//class Camera : public Component {
//
//public:
//	double fov = glm::radians(60.0);
//	double aspect = 16.0 / 9.0 ;
//	double zNear = 0.1;
//	double zFar = 128.0;
//
//private:
//	Transform _transform;
//
//public:
//	const auto& transform() const { return _transform; }
//	auto& transform() { return _transform; }
//
//	mat4 projection() const;
//	mat4 view() const;
//	
//};

class Camera : public Component {

    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;

public:

    Camera(GameObject* containerGO);
    Camera(GameObject* containerGO, Camera* ref);
    virtual ~Camera();

    mat4 getViewMatrix() const;
    mat4 getProjectionMatrix() const;
	mat4 getViewProjectionMatrix() const;

    void UpdateCamera();
    void UpdateCameraVectors();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void UpdateViewProjectionMatrix();

public:

    double fov;
    double aspect;

    double zNear;
    double zFar;

    float yaw, pitch;

    vec3f lookAt;
};
