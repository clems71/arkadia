#pragma once

#include <memory>
#include <string>

#include "ogl.h"


class Program
{
public:
	Program(const std::string & vertSrc, const std::string & fragSrc);
	~Program();

private:
	class Impl_;
	std::unique_ptr<Impl_> impl_;
};
