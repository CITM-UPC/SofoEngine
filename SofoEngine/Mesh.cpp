#include "Mesh.h"
#include <GL/glew.h>
#include "Transform.h"
#include "GameObject.h"
#include "Texture.h"

Mesh::Mesh(GameObject* containerGO) : Component(containerGO, ComponentType::Mesh)
{
	_meshData._meshName = "";
	_meshData._color = glm::u8vec3(255, 255, 255);
	_meshData._vertices = std::vector<glm::vec3>();
	_meshData._indices = std::vector<unsigned int>();
	_meshData._boundingBox = BoundingBox();
	_meshData._modelMatrix = glm::mat4(1.0f);
}

Mesh::Mesh(GameObject* containerGO, Mesh* ref) : Component(containerGO, ComponentType::Mesh)
{
	_meshData._meshName = ref->_meshData._meshName;
	_meshData._color = ref->_meshData._color;
	_meshData._vertices = ref->_meshData._vertices;
	_meshData._indices = ref->_meshData._indices;
	_meshData._boundingBox = ref->_meshData._boundingBox;
	_meshData._modelMatrix = ref->_meshData._modelMatrix;
	_meshData._vertexBuffer = ref->_meshData._vertexBuffer;
	_meshData._indexBuffer = ref->_meshData._indexBuffer;
	_meshData._texCoordBuffer = ref->_meshData._texCoordBuffer;
	_meshData._normalBuffer = ref->_meshData._normalBuffer;
	_meshData._colorBuffer = ref->_meshData._colorBuffer;
}

Mesh::Mesh(GameObject* containerGO, const MeshData& ref) : Component(containerGO, ComponentType::Mesh)
{
	_meshData._meshName = ref._meshName;
	_meshData._color = ref._color;
	_meshData._vertices = ref._vertices;
	_meshData._indices = ref._indices;
	_meshData._boundingBox = ref._boundingBox;
	_meshData._modelMatrix = ref._modelMatrix;
	_meshData._vertexBuffer = ref._vertexBuffer;
	_meshData._indexBuffer = ref._indexBuffer;
	_meshData._texCoordBuffer = ref._texCoordBuffer;
	_meshData._normalBuffer = ref._normalBuffer;
	_meshData._colorBuffer = ref._colorBuffer;
}

Mesh::~Mesh() {}

void Mesh::load(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs) {
	_meshData.loadData(verts, num_verts, indexs, num_indexs);
}

void Mesh::loadTexCoords(const glm::vec2* texCoords) {
	_meshData.loadTexCoordsData(texCoords);
}

void Mesh::loadNormals(const glm::vec3* normals) {
	_meshData.loadNormalsData(normals);
}

void Mesh::loadColors(const glm::u8vec3* colors) {
	_meshData.loadColorsData(colors);
}

void Mesh::drawTriangleNormals(float normalLength) const
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (size_t i = 0; i < _meshData._indices.size(); i += 3) {
		glm::vec3 v0 = _meshData._vertices[_meshData._indices[i]];
		glm::vec3 v1 = _meshData._vertices[_meshData._indices[i + 1]];
		glm::vec3 v2 = _meshData._vertices[_meshData._indices[i + 2]];

		glm::vec3 center = (v0 + v1 + v2) / 3.0f;

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		glVertex3fv(&center[0]);
		glm::vec3 end = center + normal * normalLength;
		glVertex3fv(&end[0]);
	}
	glEnd();
}

void Mesh::drawFaceNormals(float normalLength) const
{
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);

	for (size_t i = 0; i < _meshData._indices.size(); i += 3) {
		glm::vec3 v0 = _meshData._vertices[_meshData._indices[i]];
		glm::vec3 v1 = _meshData._vertices[_meshData._indices[i + 1]];
		glm::vec3 v2 = _meshData._vertices[_meshData._indices[i + 2]];

		glm::vec3 n0 = glm::normalize(v0);
		glm::vec3 n1 = glm::normalize(v1);
		glm::vec3 n2 = glm::normalize(v2);

		glVertex3fv(&v0[0]);
		glVertex3fv(&(v0 + n0 * normalLength)[0]);

		glVertex3fv(&v1[0]);
		glVertex3fv(&(v1 + n1 * normalLength)[0]);

		glVertex3fv(&v2[0]);
		glVertex3fv(&(v2 + n2 * normalLength)[0]);
	}
	glEnd();
}

void MeshData::loadData(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs)
{
	_vertices.assign(verts, verts + num_verts);
	_indices.assign(indexs, indexs + num_indexs);
	_vertexBuffer.loadElements(_vertices.size(), _vertices.data());
	_indexBuffer.loadIndices(_indices.size(), _indices.data());
	_texCoordBuffer.unload();
	_normalBuffer.unload();
	_colorBuffer.unload();

	_boundingBox.min = _vertices.front();
	_boundingBox.max = _vertices.front();

	for (const auto& v : _vertices) {
		_boundingBox.min = glm::min(_boundingBox.min, glm::dvec3(v));
		_boundingBox.max = glm::max(_boundingBox.max, glm::dvec3(v));
	}
}

void MeshData::loadTexCoordsData(const glm::vec2* texCoords)
{
	_texCoordBuffer.loadElements(_vertices.size(), texCoords);
}

void MeshData::loadNormalsData(const glm::vec3* normals)
{
	_normalBuffer.loadElements(_vertices.size(), normals);
}

void MeshData::loadColorsData(const glm::u8vec3* colors)
{
	_colorBuffer.loadElements(_vertices.size(), colors);
}
