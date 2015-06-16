#include "ogl.h"

#include <array>

#include "log.h"

namespace 
{

auto createShader(const std::string & src, GLenum shaderType)
{
	auto shaderId = glCreateShader(shaderType);
	auto sources = std::array<const char*, 1>{ src.c_str() };
	glShaderSource(shaderId, 1, &sources[0], NULL);
	glCompileShader(shaderId);
	GLint result;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		GLint bufSz;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &bufSz);
		std::unique_ptr<char[]> logStr(new char[bufSz]);
		GLsizei bufSz2;
		glGetShaderInfoLog(shaderId, bufSz, &bufSz2, logStr.get());
		LOG_ERROR("Cannot compile shader\n%s", logStr.get());
		throw std::runtime_error("cannot compile shader");
	}
	return shaderId;
}

} // anonymous namespace

class Program::Impl_ {
public:
	GLint program_;
	GLint vertex_;
	GLint fragment_;
};

Program::Program(const std::string & vertSrc, const std::string & fragSrc)
: impl_(new Impl_)
{
	impl_->program_ = glCreateProgram();
}

Program::~Program()
{
}