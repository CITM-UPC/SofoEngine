#include "Mesh.h"
#include <GL/glew.h>

void Mesh::load(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs) {
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

void Mesh::loadTexCoords(const glm::vec2* texCoords) {
	_texCoordBuffer.loadElements(_vertices.size(), texCoords);
}

void Mesh::loadNormals(const glm::vec3* normals) {
	_normalBuffer.loadElements(_vertices.size(), normals);
}

void Mesh::loadColors(const glm::u8vec3* colors) {
	_colorBuffer.loadElements(_vertices.size(), colors);
}

void Mesh::draw() const {
	
	if (_texCoordBuffer.id()) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		_texCoordBuffer.bind();
		glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	}

	if (_normalBuffer.id()) {
		glEnableClientState(GL_NORMAL_ARRAY);
		_normalBuffer.bind();
		glNormalPointer( GL_FLOAT, 0, nullptr);
	}

	if (_colorBuffer.id()) {
		glEnableClientState(GL_COLOR_ARRAY);
		_colorBuffer.bind();
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, nullptr);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	_vertexBuffer.bind();
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	_indexBuffer.bind();
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(_indices.size()), GL_UNSIGNED_INT, nullptr);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (_colorBuffer.id()) glDisableClientState(GL_COLOR_ARRAY);
	if (_normalBuffer.id()) glDisableClientState(GL_NORMAL_ARRAY);
	if (_texCoordBuffer.id()) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
