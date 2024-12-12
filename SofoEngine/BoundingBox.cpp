#include "BoundingBox.h"

#define DEFAULT_BBOX_SIZE 0.1

BoundingBox::BoundingBox()
{
	min = vec3(-DEFAULT_BBOX_SIZE);
	max = vec3(DEFAULT_BBOX_SIZE);
}

BoundingBox::BoundingBox(double MAX, double MIN)
{
	min = vec3(-MIN);
	max = vec3(MAX);
}

BoundingBox::BoundingBox(const glm::vec3& pMin, const glm::vec3& pMax)
	: min(glm::min(pMin, pMax)), max(glm::max(pMin, pMax)) {}

BoundingBox::BoundingBox(const vec3* vertices, size_t num_verts) {
	min = max = vertices[0];
	for (size_t i = 1; i < num_verts; ++i) {
		min = glm::min(min, vertices[i]);
		max = glm::max(max, vertices[i]);
	}
}

BoundingBox BoundingBox::operator+(const BoundingBox& other) const {
	BoundingBox result;
	result.min = glm::min(min, other.min);
	result.max = glm::max(max, other.max);
	return result;
}

BoundingBox operator*(mat4& mat, BoundingBox& bbox) {
	auto vertices = bbox.vertices();
	for (auto& v : vertices) v = mat * vec4(v, 1);
	return BoundingBox(vertices.data(), vertices.size());
}

BoundingBox operator*(const mat4& mat, const BoundingBox& bbox) {
	auto vertices = bbox.vertices();
	for (auto& v : vertices) v = mat * vec4(v, 1);
	return BoundingBox(vertices.data(), vertices.size());
}
