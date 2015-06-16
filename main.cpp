#include <chrono>
#include <iostream>
#include <thread>

#include <SDL.h>

#include <genesiscore.h>

#include "ogl.h"
#include "font.h"
#include "log.h"

// Need ogl.h
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>


void audioCallback(void*  userdata,
                       Uint8* stream,
                       int    len)
{
  len /= sizeof(short);
  std::vector<short> * audioSamples = (std::vector<short> *)userdata;
  short * streami16 = (short *)stream;
  const int nSamples = std::min<int>(len, audioSamples->size());
  for (size_t i=0; i<nSamples; i++) {
    streami16[i] = audioSamples->at(i);
  }
  for (size_t i=nSamples; i<len; i++) {
    streami16[i] = 0; // silence padding
  }
  audioSamples->erase(audioSamples->begin(), audioSamples->begin() + nSamples);
}


#define IMPL_BUTTON(sdlk, btn) \
  case sdlk: \
    if (event.key.state == SDL_PRESSED) core.buttonPressed(Players::Player1, Buttons::btn); \
    else core.buttonReleased(Players::Player1, Buttons::btn); \
    break; \


int main(int argc, char *argv[])
{
  if (argc == 1) return -1;

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  auto win = SDL_CreateWindow("Arkadia", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (!win) {
    throw std::runtime_error("cannot initialize OpenGL window");
  }
  SDL_GL_CreateContext(win);

  glextInit();

  // glEnable(GL_STENCIL_TEST);
  NVGcontext * vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
  nvgCreateFontMem(vg, "arcade", &font_speedwagon[0], font_speedwagon.size(), 0);

  GenesisCore core(argv[1]);

  if (SDL_GetNumAudioDevices(0) == 0)
    return -3;

  LOG_INFO("Using audio device '%s'", SDL_GetAudioDeviceName(0, 0));

  std::vector<short> audioSamples;  
  SDL_AudioSpec want, have;
  SDL_zero(want);
  want.freq = 48000;
  want.format = AUDIO_S16LSB;
  want.channels = 2;
  want.samples = 4096;
  want.callback = audioCallback;
  want.userdata = &audioSamples;
  const auto audioDevice = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, 0), 0, &want, &have, 0);

  if (!audioDevice) {
    throw std::runtime_error("cannot open audio device");
  }

  SDL_PauseAudioDevice(audioDevice, 0);  // start audio playing.

  // Initialize core framebuffer image
  const Surface & vbuf = core.videoBuffer();
  int fbImage = nvgCreateImageRGBA(vg, vbuf.width, vbuf.height, 0, (const unsigned char *)vbuf.pixels.get());

  auto tframe = std::chrono::high_resolution_clock::now();

  SDL_Event event = {0};
  while (true)
  {
    // FPS Calculus
    const auto tnew = std::chrono::high_resolution_clock::now();
    const float fps = 1.0 / std::chrono::duration<double>(tnew - tframe).count();
    tframe = tnew;

    while(SDL_PollEvent(&event)) {
      switch (event.key.keysym.sym) {
        case SDLK_ESCAPE: return 0;
        IMPL_BUTTON(SDLK_UP, Up)
        IMPL_BUTTON(SDLK_DOWN, Down)
        IMPL_BUTTON(SDLK_LEFT, Left)
        IMPL_BUTTON(SDLK_RIGHT, Right)
        IMPL_BUTTON(SDLK_z, A)
        IMPL_BUTTON(SDLK_x, B)
        IMPL_BUTTON(SDLK_c, C)
        IMPL_BUTTON(SDLK_RETURN, Start)

        default:
          break;
      }
    }

    int width, height;
    SDL_GetWindowSize(win, &width, &height);

    core.update();

    // TODO purge : circular buffer with fixed capacity!
    std::copy(core.audioBuffer().begin(), core.audioBuffer().end(), std::back_inserter(audioSamples));

    // No really needed for color buffer, we redraw the full color buffer
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    nvgUpdateImage(vg, fbImage, (const unsigned char *)vbuf.pixels.get());

    nvgBeginFrame(vg, width, height, 1);
    nvgResetTransform(vg);

    NVGpaint pattern = nvgImagePattern(vg, 0, 0, width, height, 0, fbImage, 1.0);
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, width, height);
    nvgFillPaint(vg, pattern);
    nvgFill(vg);

    nvgFontFace(vg, "arcade");
    nvgFontSize(vg, 22.0f);
    nvgTextAlign(vg,NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(vg, nvgRGBA(120, 120, 120, 128));
    auto dispStr = "FPS : " + std::to_string(int(fps)) + '\n';
    dispStr += core.romInfo() + '\n';
    dispStr += std::string("Vendor : ") + (const char *)glGetString(GL_VENDOR) + '\n';
    dispStr += std::string("Renderer : ") + (const char *)glGetString(GL_RENDERER) + '\n';
    dispStr += std::string("Version : ") + (const char *)glGetString(GL_VERSION) + '\n';
    nvgTextBox(vg, 10, 10, width, dispStr.c_str(), NULL);

    nvgEndFrame(vg);
    SDL_GL_SwapWindow(win);

    while (std::chrono::high_resolution_clock::now() - tframe < std::chrono::duration<double>(core.frameInterval())) {
      std::this_thread::yield();
    }
  }

  return 0;
}
