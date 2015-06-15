#pragma once

#include <config.h>

// OpenGL ES 2.0 takes precedence
#if defined(HAVE_OPENGLES2) 
  #include <GLES2/gl2.h>
#elif defined(HAVE_OPENGL_FRAMEWORK)
  #include <OpenGL/gl.h>
#elif defined(HAVE_OPENGL)
  #include <GL/gl.h>
#else
  #error "No valid OpenGL gl.h header available"
#endif
