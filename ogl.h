#pragma once

#include <memory>
#include <string>

#include <config.h>

// OpenGL ES 2.0 takes precedence
#if defined(HAVE_OPENGLES2) 
  #include <GLES2/gl2.h>
  #define glextInit()
#elif defined(HAVE_OPENGL_FRAMEWORK)
  #include <OpenGL/gl3.h>
  #define glextInit()
#elif defined(HAVE_OPENGL)
  #include <GL/glew.h>
  inline void glextInit() {
  	glewExperimental = GL_TRUE;
  	glewInit();
  }
#else
  #error "No valid OpenGL gl.h header available"
#endif

class Program
{
public:
	Program(const std::string & vertSrc, const std::string & fragSrc);
	~Program();

private:
	class Impl_;
	std::unique_ptr<Impl_> impl_;
};
