#include "ogltexture.h"

class Texture::Impl_
{
public:
	GLuint id_;
	size_t width_;
	size_t height_;
};

Texture::Texture(size_t width, size_t height) 
: impl_(new Impl_)
{
	impl_->width_ = width;
	impl_->height_ = height;
	glGenTextures(1, &impl_->id_);
	glBindTexture(GL_TEXTURE_2D, impl_->id_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

Texture::~Texture()
{
	glDeleteTextures(1, &impl_->id_);
}

void Texture::update(const void * pixels)
{
	glBindTexture(GL_TEXTURE_2D, impl_->id_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, impl_->width_, impl_->height_, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}
