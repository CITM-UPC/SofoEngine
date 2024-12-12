#pragma once

#include <memory>
#include "Image.h"
#include "Component.h"

class Texture : public Component
{
public:
	enum WrapModes { Repeat, MirroredRepeat, Clamp };
	WrapModes wrapMode = Repeat;

	enum Filters { Nearest, Linear };
	Filters filter = Nearest;

private:
	std::shared_ptr<Image> _img_ptr;
	std::shared_ptr<Image> _originalTexture;
	std::shared_ptr<Image> _checkerTexture;

public:
	Texture(GameObject* containerGO);
	Texture(GameObject* containerGO, Texture* ref);
	virtual ~Texture() {}

	unsigned int id() const { return _img_ptr ? _img_ptr->id() : 0; }
	void bind() const;
	void setImage(std::shared_ptr<Image> img_ptr) { _img_ptr = img_ptr; _originalTexture = img_ptr; }
	const auto& image() const { return *_img_ptr; }
	bool hasTexture() const { return _originalTexture.get() == nullptr; }

	void applyCheckerTexture();
	void applyOriginalTexture();
};

