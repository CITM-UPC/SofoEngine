#pragma once

#include "Transform.h"
#include "Component.h"
#include "types.h"
#include "Frustrum.h"
#include "Ray.h"

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
    void UpdateFrustum();

    Ray ComputeCameraRay(float x, float y);

public:

    double fov;
    double aspect;

    double zNear;
    double zFar;

    float yaw, pitch;

    vec3f lookAt;

    Frustum frustum;
    bool drawFrustum;
};
