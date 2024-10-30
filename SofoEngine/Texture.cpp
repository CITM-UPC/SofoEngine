#include "Texture.h"
#include <GL/glew.h>
#include <vector>

static auto GLWrapMode(Texture::WrapModes mode) {
	switch (mode) {
	case Texture::Repeat: return GL_REPEAT;
	case Texture::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case Texture::Clamp: return GL_CLAMP_TO_EDGE;
	default: return GL_REPEAT;
	}
}

static auto GLMagFilter(Texture::Filters filter) {
	switch (filter) {
	case Texture::Nearest: return GL_NEAREST;
	case Texture::Linear: return GL_LINEAR;
	default: return GL_NEAREST;
	}
}

static auto GLMinFilter(Texture::Filters filter) {
	switch (filter) {
	case Texture::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
	case Texture::Linear: return GL_LINEAR_MIPMAP_LINEAR;
	default: return GL_NEAREST_MIPMAP_LINEAR;
	}
}

void Texture::bind() const {
	_img_ptr->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLWrapMode(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLWrapMode(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLMinFilter(filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLMagFilter(filter));
}

void Texture::applyCheckerTexture()
{
    if (!_checkerTexture) {
        int width = 64;
        int height = 64;
        int channels = 3;

        std::vector<unsigned char> checkerData(width * height * channels);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                bool isWhite = ((x / 8) % 2) == ((y / 8) % 2);
                unsigned char color = isWhite ? 255 : 0;
                int index = (y * width + x) * channels;
                checkerData[index] = color;        // R
                checkerData[index + 1] = color;    // G
                checkerData[index + 2] = color;    // B
            }
        }

        _checkerTexture = std::make_shared<Image>();
        _checkerTexture->load(width, height, channels, checkerData.data());
    }

    _img_ptr = _checkerTexture;
}

void Texture::applyOriginalTexture()
{
    if (_img_ptr != _checkerTexture) return;
    _img_ptr = _originalTexture;
}
