#pragma once

#include <memory>
#include <stdint.h>

struct pixel_t
{
  union {
    uint32_t u32;
    struct {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t a;
    };
  };
};

struct Surface
{
  Surface(size_t width, size_t height) 
  : width(width)
  , height(height)
  , pixels(new pixel_t[width * height])
  {
  }

  inline size_t pitch() const { return width * 4; }

  std::unique_ptr<pixel_t[]> pixels;
  size_t width;
  size_t height;
};