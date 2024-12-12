#pragma once

#include "types.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Component.h"

class GameObject;

class Transform : public Component {

	mat4 localTransformMatrix;
	vec3 localPosition;
	quat localRotation;
	vec3 localScale;

	mat4 globalTransformMatrix;
	vec3 globalPosition;
	quat globalRotation;
	vec3 globalScale;

public:

	Transform(GameObject* containerGO);
	Transform(GameObject* containerGO, Transform* ref);
	virtual ~Transform();
	
	void update() override;

	const auto* data() const { return &localTransformMatrix[0][0]; }

	void Translate(const vec3& translation);
	void SetLocalPosition(const vec3& newPosition);
	void SetGlobalPosition(const vec3& newPosition);

	void Rotate(const vec3& eulerAngles, bool global = false);
	void SetLocalRotation(const vec3& eulerAngles);
	void SetGlobalRotation(const vec3& eulerAngles);

	void Scale(const vec3& scaleFactors);
	void SetLocalScale(const vec3& newScale);
	void SetGlobalScale(const vec3& newScale);

	vec3 GetGlobalForward() const;
	vec3 GetGlobalUp() const;
	vec3 GetGlobalRight() const;
	vec3 GetForward() const;
	vec3 GetUp() const;
	vec3 GetRight() const;

	void SetRight(vec3 newRight);
	void SetUp(vec3 newUp);
	void SetForward(vec3 newForward);

	vec3 GetGlobalPosition() const;
	quat GetGlobalRotation() const;
	vec3 GetGlobalScale() const;
	vec3 GetLocalPosition() const;
	quat GetLocalRotation() const;
	vec3 GetLocalScale() const;

	mat4 GetLocalTransform() const;
	mat4 GetLocalTransform();
	void SetLocalTransform(mat4 transform);
	mat4 GetGlobalTransform() const;
	mat4 GetGlobalTransform();
	void SetGlobalTransform(mat4 transform);

	vec3 GetRotationEuler() const;


	void DecomposeTransform();
	void DecomposeGlobalTransform();
	mat4 CalculateWorldTransform();
	void UpdateCameraIfPresent();
	//Transform operator*(const mat4& other) { return Transform(globalTransformMatrix * other); }
	//Transform operator*(const Transform& other) { return Transform(globalTransformMatrix * other.globalTransformMatrix); }
};

//inline Transform operator*(const mat4& m, const Transform& t) { return Transform(m * t.globalTransformMatrix()); }
