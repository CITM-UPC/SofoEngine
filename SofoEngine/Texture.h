#pragma once

#include <memory>
#include "Image.h"

class Texture
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
	unsigned int id() const { return _img_ptr ? _img_ptr->id() : 0; }
	void bind() const;
	void setImage(std::shared_ptr<Image> img_ptr) { _img_ptr = img_ptr; _originalTexture = img_ptr; }
	const auto& image() const { return *_img_ptr; }

	void applyCheckerTexture();
	void applyOriginalTexture();
};

