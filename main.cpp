#include <chrono>
#include <iostream>
#include <map>
#include <thread>

#include <SDL.h>

#include <genesiscore.h>

#include "ogl.h"
#include "font.h"
#include "log.h"

// Need ogl.h
#if defined(HAVE_OPENGLES2)
  #define NANOVG_GLES2_IMPLEMENTATION
  #define nvgCreate(x) nvgCreateGLES2(x)
#else
  #define NANOVG_GL3_IMPLEMENTATION
  #define nvgCreate(x) nvgCreateGL3(x)
#endif

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


class Timer
{
public:
  Timer() {}

  size_t getAndRestart() {
    const auto t1 = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0_);
    t0_ = t1;
    return ms.count();
  }

private:
  decltype(std::chrono::high_resolution_clock::now()) t0_ = std::chrono::high_resolution_clock::now();
};


int main(int argc, char *argv[])
{
  if (argc == 1) return -1;

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

  SDL_Joystick * joy = nullptr;

  if (SDL_NumJoysticks() > 0) {
    joy = SDL_JoystickOpen(0);
  }
  

#if defined(HAVE_OPENGLES2)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  auto win = SDL_CreateWindow("Arkadia", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (!win) {
    throw std::runtime_error("cannot initialize OpenGL window");
  }
  SDL_GL_CreateContext(win);

  glextInit();

  // glEnable(GL_STENCIL_TEST);
  NVGcontext * vg = nvgCreate(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
  nvgCreateFontMem(vg, "arcade", &font_speedwagon[0], font_speedwagon.size(), 0);
  nvgCreateFontMem(vg, "stats", &font_oswald[0], font_oswald.size(), 0);

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
  int fbImage = nvgCreateImageRGBA(vg, vbuf.width, vbuf.height, NVG_IMAGE_PREMULTIPLIED, (const unsigned char *)vbuf.pixels.get());

  auto tframe = std::chrono::high_resolution_clock::now();

  std::map<std::string, size_t> timings;

  SDL_Event event = {0};
  while (true)
  {
    // FPS Calculus
    const auto tnew = std::chrono::high_resolution_clock::now();
    const float fps = 1.0 / std::chrono::duration<double>(tnew - tframe).count();
    tframe = tnew;

    int width, height;
    SDL_GetWindowSize(win, &width, &height);

    Timer timer;

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

    if (joy) {

      static uint32_t state = 0u;

      Sint16 x_move = SDL_JoystickGetAxis(joy, 0);
      Sint16 y_move = SDL_JoystickGetAxis(joy, 1);

      uint32_t new_state = 0u;
      if(x_move > 0) {
        new_state |= 1 << (int)Buttons::Right;
      } else if(x_move < 0) {
        new_state |= 1 << (int)Buttons::Left;
      }

      if(y_move > 0) {
        new_state |= 1 << (int)Buttons::Down;
      } else if(y_move < 0) {
        new_state |= 1 << (int)Buttons::Up;
      }

      if(SDL_JoystickGetButton(joy, 0)) {
        new_state |= 1 << (int)Buttons::A;
      }

      if(SDL_JoystickGetButton(joy, 1)) {
        new_state |= 1 << (int)Buttons::B;
      }

      uint32_t changes = new_state ^ state;
      for(int i = 0 ; i < 32; ++i) {
        if((changes >> i) & 1) {
          if((new_state >> i) & 1) {
            core.buttonPressed(Players::Player1, Buttons(i));
          } else {
            core.buttonReleased(Players::Player1, Buttons(i));
          }
        }
      } 
      state = new_state;
    }

    timings["tEvents"] = timer.getAndRestart();

    core.update();

    timings["tCore"] = timer.getAndRestart();

    // TODO purge : circular buffer with fixed capacity!
    std::copy(core.audioBuffer().begin(), core.audioBuffer().end(), std::back_inserter(audioSamples));

    timings["tAudioCopy"] = timer.getAndRestart();

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

    nvgFontFace(vg, "stats");
    nvgFontSize(vg, 20.0f);
    nvgTextAlign(vg,NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(vg, nvgRGBA(220, 220, 220, 128));
    auto dispStr = "FPS : " + std::to_string(int(fps)) + '\n';
    dispStr += core.romInfo() + '\n';
    dispStr += std::string("Vendor : ") + (const char *)glGetString(GL_VENDOR) + '\n';
    dispStr += std::string("Renderer : ") + (const char *)glGetString(GL_RENDERER) + '\n';
    dispStr += std::string("Version : ") + (const char *)glGetString(GL_VERSION) + '\n';

    // Print timings
    for (const auto & pair : timings) {
      dispStr += pair.first + ':' + std::to_string(pair.second) + "ms\n";
    }
    nvgTextBox(vg, 10, 10, width, dispStr.c_str(), NULL);

    nvgEndFrame(vg);
    SDL_GL_SwapWindow(win);

    timings["tRender"] = timer.getAndRestart();

    while (std::chrono::high_resolution_clock::now() - tframe < std::chrono::duration<double>(core.frameInterval())) {
      std::this_thread::yield();
    }

    timings["tIdle"] = timer.getAndRestart();
  }

  return 0;
}
