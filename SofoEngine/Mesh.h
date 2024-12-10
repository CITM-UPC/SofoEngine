#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "BufferObject.h"
#include "BoundingBox.h"

class Mesh {

	BufferObject _texCoordBuffer;
	BufferObject _normalBuffer;
	BufferObject _colorBuffer;
	BufferObject _vertexBuffer;
	BufferObject _indexBuffer;

	std::vector<glm::vec3> _vertices;
	std::vector<unsigned int> _indices;

	BoundingBox _boundingBox;

	glm::mat4 _modelMatrix = glm::mat4(1.0f);

	std::string _name = "";

public:
	const auto& vertices() const { return _vertices; }
	const auto& indices() const { return _indices; }
	const auto& boundingBox() const { return _boundingBox; }
	const auto& modelMatrix() const { return _modelMatrix; }
	const auto& name() const { return _name; }

	void load(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs);
	void loadTexCoords(const glm::vec2* texCoords);
	void loadNormals(const glm::vec3* normals);
	void loadColors(const glm::u8vec3* colors);
	void loadModelMatrix(const glm::mat4& modelMatrix) { _modelMatrix = modelMatrix; }
	void setName(const std::string& name) { _name = name; }
	void draw() const;

	void drawTriangleNormals(float normalLength = 0.1f) const;
	void drawFaceNormals(float normalLength = 0.1f) const;

};