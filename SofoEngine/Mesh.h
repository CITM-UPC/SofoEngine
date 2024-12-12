#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "BufferObject.h"
#include "BoundingBox.h"
#include "Component.h"

struct MeshData
{
	MeshData() = default;

	MeshData(const MeshData&) = delete;
	MeshData& operator=(const MeshData&) = delete;

	MeshData(MeshData&& other) noexcept = default;
	MeshData& operator=(MeshData&& other) noexcept = default;

	BufferObject _texCoordBuffer;
	BufferObject _normalBuffer;
	BufferObject _colorBuffer;
	BufferObject _vertexBuffer;
	BufferObject _indexBuffer;

	glm::u8vec3 _color = glm::u8vec3(255, 255, 255);

	std::vector<glm::vec3> _vertices;
	std::vector<unsigned int> _indices;

	BoundingBox _boundingBox;

	glm::mat4 _modelMatrix = glm::mat4(1.0f);

	std::string _meshName = "";

	void loadData(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs);
	void loadTexCoordsData(const glm::vec2* texCoords);
	void loadNormalsData(const glm::vec3* normals);
	void loadColorsData(const glm::u8vec3* colors);
	void loadModelMatrixData(const glm::mat4& modelMatrix) { _modelMatrix = modelMatrix; }
	void setMeshDataName(const std::string& name) { _meshName = name; }
};

class Mesh : public Component {
public:
	MeshData _meshData;

	Mesh(GameObject* containerGO);
	Mesh(GameObject* containerGO, Mesh* ref);
	Mesh(GameObject* containerGO,const MeshData& ref);
	virtual ~Mesh();

	const auto& vertices() const { return _meshData._vertices; }
	const auto& indices() const { return _meshData._indices; }
	const auto& boundingBox() const { return _meshData._boundingBox; }
	const auto& modelMatrix() const { return _meshData._modelMatrix; }
	const auto& name() const { return _meshData._meshName; }

	void load(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs);
	void loadTexCoords(const glm::vec2* texCoords);
	void loadNormals(const glm::vec3* normals);
	void loadColors(const glm::u8vec3* colors);
	void loadModelMatrix(const glm::mat4& modelMatrix) { _meshData.loadModelMatrixData(modelMatrix); }
	void setMeshName(const std::string& name) { _meshData.setMeshDataName(name); }

	void drawTriangleNormals(float normalLength = 0.1f) const;
	void drawFaceNormals(float normalLength = 0.1f) const;

};