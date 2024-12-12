#include "GameObject.h"
#include <GL/glew.h>
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"

GameObject::GameObject(std::string name) :
	name(name),
	components(),
	enabled(true)
{
	enable();
}

GameObject::~GameObject() {}

GameObject::GameObject(GameObject&& other) noexcept
	: TreeExt(std::move(other)),
	name(std::move(other.name)),
	components(std::move(other.components)),
	enabled(other.enabled),
	aabb(std::move(other.aabb)) {}

void GameObject::update()
{
	for (auto& component : components)
	{
		if (component->isEnabled())
			component->update();
	}

	for (auto& child : children())
	{
		if (child.isEnabled())
			child.update();
	}
}

void GameObject::draw()
{
	glPushMatrix();

	Transform* transform = GetComponent<Transform>();
	glMultMatrixd(transform->data());

	Texture* texture = GetComponent<Texture>();
	if (texture != nullptr && &texture->image() != nullptr) {
		glEnable(GL_TEXTURE_2D);
		texture->bind();
	}

	Mesh* mesh = GetComponent<Mesh>();
	if (mesh != nullptr)
	{
		glColor3ubv(&mesh->_meshData._color.r);

		if (mesh->_meshData._texCoordBuffer.id()) {
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			mesh->_meshData._texCoordBuffer.bind();
			glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
		}

		if (mesh->_meshData._normalBuffer.id()) {
			glEnableClientState(GL_NORMAL_ARRAY);
			mesh->_meshData._normalBuffer.bind();
			glNormalPointer(GL_FLOAT, 0, nullptr);
		}

		if (mesh->_meshData._colorBuffer.id()) {
			glEnableClientState(GL_COLOR_ARRAY);
			mesh->_meshData._colorBuffer.bind();
			glColorPointer(3, GL_UNSIGNED_BYTE, 0, nullptr);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		mesh->_meshData._vertexBuffer.bind();
		glVertexPointer(3, GL_FLOAT, 0, nullptr);

		mesh->_meshData._indexBuffer.bind();
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh->_meshData._indices.size()), GL_UNSIGNED_INT, nullptr);

		glDisableClientState(GL_VERTEX_ARRAY);
		if (mesh->_meshData._colorBuffer.id())
			glDisableClientState(GL_COLOR_ARRAY);
		if (mesh->_meshData._normalBuffer.id())
			glDisableClientState(GL_NORMAL_ARRAY);
		if (mesh->_meshData._texCoordBuffer.id())
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	}

	if (texture != nullptr && &texture->image() != nullptr) glDisable(GL_TEXTURE_2D);

	for (auto& child : children()) child.draw();

	glPopMatrix();
}

void GameObject::destroy()
{
	parent().removeChild(*this);
}

std::vector<Component*> GameObject::GetAllComponents()
{
	std::vector<Component*> tempComponents;
	for (const auto& item : components)
	{
		tempComponents.push_back(item.get());
	}
	return tempComponents;
}

GameObject GameObject::Clone() {
	GameObject duplicate(this->name);
	duplicate.enabled = this->enabled;
	duplicate.aabb = this->aabb;

	for (const auto& item : GetAllComponents())
	{
		switch (item->GetType())
		{
		case ComponentType::Transform:
			duplicate.AddCopiedComponent<Transform>((Transform*)item);
			break;
		case ComponentType::Camera:
			duplicate.AddCopiedComponent<Camera>((Camera*)item);
			break;
		case ComponentType::Mesh:
			duplicate.AddCopiedComponent<Mesh>((Mesh*)item);
			break;
		case ComponentType::Texture:
			duplicate.AddCopiedComponent<Texture>((Texture*)item);
			break;
		case ComponentType::Unknown:
			break;
		default:
			break;
		}
	}

	for (auto& child : children())
	{
		duplicate.emplaceChild(child.Clone());
	}

	return duplicate;
}

BoundingBox GameObject::localBoundingBox() {
	Mesh* _mesh_ptr = GetComponent<Mesh>();
	if (children().size()) {
		BoundingBox bbox = _mesh_ptr ? _mesh_ptr->boundingBox() : children().front().boundingBox();
		for (auto& child : children()) bbox = bbox + child.boundingBox();
		return bbox;
	}
	else return _mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox();
}

BoundingBox GameObject::worldBoundingBox() {
	Mesh* _mesh_ptr = GetComponent<Mesh>();
	BoundingBox bbox = GetComponent<Transform>()->GetGlobalTransform() * (_mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox());
	//for (auto& child : children()) bbox = bbox + child.worldBoundingBox();
	return bbox;
}