#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GameObject.h"
#include "Transform.h"

Camera::Camera(GameObject* containerGO) : Component(containerGO, ComponentType::Camera),
aspect(1.777), fov(65),
zNear(0.1), zFar(3000),
yaw(0), pitch(0),
viewMatrix(1.0f),
lookAt(0, 0, 0)
//drawFrustum(true),
//primaryCam(false)
{
    Transform* transform = containerGO->GetComponent<Transform>();

    if (transform)
    {
        lookAt = transform->GetGlobalPosition() + transform->GetGlobalForward();
    }
}

Camera::Camera(GameObject* containerGO, Camera* ref) : Component(containerGO, ComponentType::Camera),
aspect(ref->aspect), fov(ref->fov),
zNear(ref->zNear), zFar(ref->zFar),
yaw(ref->yaw), pitch(ref->pitch),
viewMatrix(ref->viewMatrix),
lookAt(ref->lookAt)
//drawFrustum(ref->drawFrustum),
//primaryCam(ref->primaryCam)
{
    //frustum = ref->frustum;
    projectionMatrix = ref->projectionMatrix;
    viewProjectionMatrix = ref->viewProjectionMatrix;
}

Camera::~Camera() {}

mat4 Camera::getViewMatrix() const
{
    return viewMatrix;
}

mat4 Camera::getProjectionMatrix() const
{
    return projectionMatrix;
}

mat4 Camera::getViewProjectionMatrix() const
{
    return viewProjectionMatrix;
}


void Camera::UpdateCamera()
{
    UpdateCameraVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    UpdateViewProjectionMatrix();
    //UpdateFrustum();
}

void Camera::UpdateCameraVectors()
{
    if (gameObject)
    {
        Transform* transform = gameObject->GetComponent<Transform>();
        lookAt = transform->GetGlobalPosition() + transform->GetGlobalForward();
    }
}

void Camera::UpdateViewMatrix()
{
    if (gameObject)
    {
        Transform* transform = gameObject->GetComponent<Transform>();
        Camera* editorCamera = gameObject->GetComponent<Camera>();
        glm::vec3 tempPosition = transform->GetGlobalPosition();
        glm::vec3 tempUp = transform->GetGlobalUp();

        viewMatrix = glm::lookAt(tempPosition, editorCamera->lookAt, tempUp);
    }
}

void Camera::UpdateProjectionMatrix()
{
    projectionMatrix = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
}

void Camera::UpdateViewProjectionMatrix()
{
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

//void Camera::UpdateFrustum()
//{
//    frustum.Update(viewProjectionMatrix);
//}