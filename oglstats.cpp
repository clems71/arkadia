#include "oglstats.h"

#include <iostream>

#include "ogl.h"

void oglPrintDebugStats()
{
  std::cout << "OpenGL Info" << std::endl;
  std::cout << "   Version : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "    Vendor : " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "  Renderer : " << glGetString(GL_RENDERER) << std::endl;
}
