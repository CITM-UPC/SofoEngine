#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GameObject.h"
#include "Transform.h"

Camera::Camera(GameObject* containerGO) : Component(containerGO, ComponentType::Camera),
aspect(1.777), fov(65),
zNear(0.1), zFar(200),
yaw(0), pitch(0),
viewMatrix(1.0f),
lookAt(0, 0, 0),
frustum(),
projectionMatrix(1.0f),
viewProjectionMatrix(1.0f),
drawFrustum(true)
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
lookAt(ref->lookAt),
frustum(ref->frustum),
projectionMatrix(ref->projectionMatrix),
viewProjectionMatrix(ref->viewProjectionMatrix),
drawFrustum(ref->drawFrustum)
{}

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
    UpdateFrustum();
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

void Camera::UpdateFrustum()
{
    frustum.Update(viewProjectionMatrix);
}

Ray Camera::ComputeCameraRay(float x, float y)
{
    glm::mat4 viewProjInverse = glm::inverse(viewProjectionMatrix);

    //glm::vec4 worldDirection = viewProjInverse * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 worldDirection = viewProjInverse * glm::vec4(x, y, 1.0f, 1.0f);

    if (gameObject)
    {
        Transform* transform = gameObject->GetComponent<Transform>();
        return Ray(transform->GetGlobalPosition(), glm::normalize(worldDirection));
    }
    else
    {
        return Ray();
    }
}

//Ray Camera::ComputeCameraRay(float x, float y) {
//    glm::mat4 viewProjInverse = glm::inverse(viewProjectionMatrix);
//
//    // Puntos en el espacio de recorte (clip space)
//    glm::vec4 nearPoint = glm::vec4(x, y, -1.0f, 1.0f); // Near plane
//    glm::vec4 farPoint = glm::vec4(x, y, 1.0f, 1.0f);   // Far plane
//
//    // Transformar los puntos al espacio mundial
//    glm::vec4 worldNear = viewProjInverse * nearPoint;
//    glm::vec4 worldFar = viewProjInverse * farPoint;
//
//    // Convertir de coordenadas homogéneas a coordenadas 3D
//    worldNear /= worldNear.w;
//    worldFar /= worldFar.w;
//
//    // Calcular la dirección del rayo
//    glm::vec3 rayDirection = glm::normalize(glm::vec3(worldFar - worldNear));
//
//    if (gameObject) {
//        Transform* transform = gameObject->GetComponent<Transform>();
//        return Ray(transform->GetGlobalPosition(), rayDirection);
//    }
//    else {
//        return Ray(glm::vec3(worldNear), rayDirection);
//    }
//}
