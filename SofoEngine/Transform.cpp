#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GameObject.h"
#include "Component.h"
#include "glm/glm.hpp"
#include "Camera.h"

Transform::Transform(GameObject* containerGO) : Component(containerGO, ComponentType::Transform),
localTransformMatrix(1.0f),
localPosition(0.0f),
localRotation(1, 0, 0, 0),
localScale(1.0f),
globalTransformMatrix(1.0f)
{}

Transform::Transform(GameObject* containerGO, Transform* ref) : Component(containerGO, ComponentType::Transform),
    localTransformMatrix(ref->localTransformMatrix),
    localPosition(ref->localPosition), localRotation(ref->localRotation), localScale(ref->localScale),
    globalTransformMatrix(ref->globalTransformMatrix)
{}

Transform::~Transform() {}

void Transform::update()
{
	CalculateWorldTransform();
    DecomposeTransform();
    DecomposeGlobalTransform();
    UpdateCameraIfPresent();
}


// @Transform ----------------------------------------------------
void Transform::Translate(const vec3& translation)
{
    mat4 newTransform = localTransformMatrix;
    localTransformMatrix[3] += vec4(translation, 1);
    localPosition = localTransformMatrix[3];
    UpdateCameraIfPresent();
}

void Transform::SetLocalPosition(const vec3& newPosition)
{
    mat4 newTransform = localTransformMatrix;
    newTransform[3] = glm::vec4(newPosition, 1.0f);

    localTransformMatrix = newTransform;
    localPosition = localTransformMatrix[3];

    UpdateCameraIfPresent();
}


void Transform::SetGlobalPosition(const vec3& newPosition)
{
    mat4 newTransform = CalculateWorldTransform();
    newTransform[3] = glm::vec4(newPosition, 1.0f);

    //localTransformMatrix = WorldToLocalTransform(containerGO.lock().get(), newTransform);
    localPosition = localTransformMatrix[3];

    UpdateCameraIfPresent();
}

void Transform::Rotate(const vec3& eulerAngles, bool global)
{
    glm::mat3x3 referenceFrameMat = glm::mat3x3(
        vec3(localTransformMatrix[0][0], localTransformMatrix[0][1], localTransformMatrix[0][2]),
        vec3(localTransformMatrix[1][0], localTransformMatrix[1][1], localTransformMatrix[1][2]),
        vec3(localTransformMatrix[2][0], localTransformMatrix[2][1], localTransformMatrix[2][2]));

    glm::vec3 vecInRefFrame = eulerAngles;
    if (!global)
        vecInRefFrame = referenceFrameMat * eulerAngles;

    //Generate the localRotation matrix that corresponds to the localRotation
    glm::mat3x3 rotX = glm::mat3x3(1, 0, 0,
        0, glm::cos(glm::radians(vecInRefFrame.x)), -glm::sin(glm::radians(vecInRefFrame.x)),
        0, glm::sin(glm::radians(vecInRefFrame.x)), glm::cos(glm::radians(vecInRefFrame.x)));

    glm::mat3x3 rotY = glm::mat3x3(glm::cos(glm::radians(vecInRefFrame.y)), 0, glm::sin(glm::radians(vecInRefFrame.y)),
        0, 1, 0,
        -glm::sin(glm::radians(vecInRefFrame.y)), 0, glm::cos(glm::radians(vecInRefFrame.y)));

    glm::mat3x3 rotZ = glm::mat3x3(glm::cos(glm::radians(vecInRefFrame.z)), -glm::sin(glm::radians(vecInRefFrame.z)), 0,
        glm::sin(glm::radians(vecInRefFrame.z)), glm::cos(glm::radians(vecInRefFrame.z)), 0,
        0, 0, 1);

    glm::mat3x3 rotMatrix = rotZ * rotY * rotX;

    //Apply the generated localRotation matrix to the existing director vectors
    vec3 temp = rotMatrix * vec3(localTransformMatrix[2][0], localTransformMatrix[2][1], localTransformMatrix[2][2]);
    localTransformMatrix[2] = vec4(temp, 0.0f);
    temp = rotMatrix * vec3(localTransformMatrix[0][0], localTransformMatrix[0][1], localTransformMatrix[0][2]);
    localTransformMatrix[0] = vec4(temp, 0.0f);
    temp = rotMatrix * vec3(localTransformMatrix[1][0], localTransformMatrix[1][1], localTransformMatrix[1][2]);
    localTransformMatrix[1] = vec4(temp, 0.0f);

    UpdateCameraIfPresent();
}

void Transform::SetLocalRotation(const vec3& eulerAngles)
{
    quat quaternion = glm::angleAxis(eulerAngles.x, vec3(1, 0, 0));
    quaternion *= glm::angleAxis(eulerAngles.y, vec3(0, 1, 0));
    quaternion *= glm::angleAxis(eulerAngles.z, vec3(0, 0, 1));

    localRotation = quaternion;

    localTransformMatrix = glm::translate(mat4(1.0f), localPosition) * glm::mat4_cast(localRotation) * glm::scale(mat4(1.0f), localScale);

    UpdateCameraIfPresent();
}

void Transform::SetGlobalRotation(const vec3& eulerAngles)
{
    quat quaternion = glm::angleAxis(eulerAngles.x, vec3(1, 0, 0));
    quaternion *= glm::angleAxis(eulerAngles.y, vec3(0, 1, 0));
    quaternion *= glm::angleAxis(eulerAngles.z, vec3(0, 0, 1));

    globalRotation = quaternion;

    globalTransformMatrix = glm::translate(mat4(1.0f), localPosition) * glm::mat4_cast(localRotation) * glm::scale(mat4(1.0f), localScale);

    UpdateCameraIfPresent();
}

void Transform::Scale(const vec3& scaleFactors)
{
    localTransformMatrix = glm::scale(localTransformMatrix, scaleFactors);
    localScale *= scaleFactors;
}

void Transform::SetLocalScale(const vec3& newScale)
{
    mat4 newScaleMatrix = glm::scale(mat4(1.0f), vec3(newScale.x, newScale.y, newScale.z));
    localTransformMatrix = glm::translate(mat4(1.0f), localPosition) * glm::mat4_cast(localRotation) * newScaleMatrix;

    localScale = newScale;
}

void Transform::SetGlobalScale(const vec3& newScale)
{
    mat4 newScaleMatrix = glm::scale(mat4(1.0f), vec3(newScale.x, newScale.y, newScale.z));
    globalTransformMatrix = glm::translate(mat4(1.0f), globalPosition) * glm::mat4_cast(globalRotation) * newScaleMatrix;

    globalScale = newScale;
}


// @Utils --------------------------------------------------------
void Transform::DecomposeTransform()
{
    localPosition = localTransformMatrix[3];

    for (int i = 0; i < 3; i++)
        localScale[i] = glm::length(vec3(localTransformMatrix[i]));

    const glm::mat3 rotMtx(
        vec3(localTransformMatrix[0]) / localScale[0],
        vec3(localTransformMatrix[1]) / localScale[1],
        vec3(localTransformMatrix[2]) / localScale[2]);

    localRotation = glm::quat_cast(rotMtx);
}

void Transform::DecomposeGlobalTransform()
{
    globalPosition = globalTransformMatrix[3];

    for (int i = 0; i < 3; i++)
        globalScale[i] = glm::length(vec3(globalTransformMatrix[i]));

    const glm::mat3 rotMtx(
        vec3(globalTransformMatrix[0]) / globalScale[0],
        vec3(globalTransformMatrix[1]) / globalScale[1],
        vec3(globalTransformMatrix[2]) / globalScale[2]);

    globalRotation = glm::quat_cast(rotMtx);
}

mat4 Transform::CalculateWorldTransform()
{
    mat4 worldTransform = localTransformMatrix;

    if (&gameObject->parent() == nullptr)
    {
        globalTransformMatrix = worldTransform;
        return worldTransform;
    }

    GameObject* parent = gameObject->parent().isRoot() ? nullptr : &gameObject->parent();

    while (parent != nullptr)
    {
        worldTransform = parent->GetComponent<Transform>()->localTransformMatrix * worldTransform;
        parent = parent->isRoot() ? nullptr : &parent->parent();
    }

    globalTransformMatrix = worldTransform;

    return worldTransform;
}

void Transform::UpdateCameraIfPresent()
{
    Camera* editorCamera = gameObject->GetComponent<Camera>();
    if (editorCamera) 
    { 
        editorCamera->UpdateCamera(); 
    }
}



// @Get/Set ------------------------------------------------------
vec3 Transform::GetRight() const
{
    return glm::normalize(localTransformMatrix[0]);
}

void Transform::SetRight(vec3 newRight)
{
    localTransformMatrix[0] = vec4(newRight, 0.0f);
}

vec3 Transform::GetUp() const
{
    return glm::normalize(localTransformMatrix[1]);
}

void Transform::SetUp(vec3 newUp)
{
    localTransformMatrix[1] = vec4(newUp, 0.0f);
}

vec3 Transform::GetGlobalForward() const
{
    return glm::normalize(globalTransformMatrix[2]);
}

vec3 Transform::GetGlobalUp() const
{
    return glm::normalize(globalTransformMatrix[1]);
}

vec3 Transform::GetGlobalRight() const
{
    return glm::normalize(globalTransformMatrix[0]);
}

vec3 Transform::GetForward() const
{
    return glm::normalize(localTransformMatrix[2]);
}

void Transform::SetForward(vec3 newForward)
{
    localTransformMatrix[2] = vec4(newForward, 0.0f);
}

vec3 Transform::GetGlobalPosition() const
{
    return globalPosition;
}

quat Transform::GetGlobalRotation() const
{
    return globalRotation;
}

vec3 Transform::GetGlobalScale() const
{
    return globalScale;
}

vec3 Transform::GetLocalPosition() const
{
    return localPosition;
}

quat Transform::GetLocalRotation() const
{
    return localRotation;
}

vec3 Transform::GetLocalScale() const
{
    return localScale;
}

mat4 Transform::GetLocalTransform() const
{
    return localTransformMatrix;
}

mat4 Transform::GetLocalTransform()
{
    return localTransformMatrix;
}

void Transform::SetLocalTransform(mat4 transform)
{
    SetLocalPosition({ transform[3][0], transform[3][1], transform[3][2] });
    this->localTransformMatrix = transform;
    DecomposeTransform();
}

mat4 Transform::GetGlobalTransform() const
{
    return globalTransformMatrix;
}

mat4 Transform::GetGlobalTransform()
{
    return globalTransformMatrix;
}

void Transform::SetGlobalTransform(mat4 transform)
{
	globalTransformMatrix = transform;
	DecomposeGlobalTransform();
}

vec3 Transform::GetRotationEuler() const
{
    return glm::eulerAngles(localRotation);
}