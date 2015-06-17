#pragma once

#include <memory>

#include "ogl.h"


class Texture
{
public:
	Texture(size_t width, size_t height);
	~Texture();

	void update(const void * pixels);

private:
	class Impl_;
	std::unique_ptr<Impl_> impl_;	
};
