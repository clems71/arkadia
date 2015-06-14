#include <chrono>
#include <iostream>
#include <thread>

#include <SDL.h>

#include <genesiscore.h>

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

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  if (SDL_GetNumAudioDevices(0) == 0)
    return -2;

  SDL_Window * win = SDL_CreateWindow("GenesisCore", 100, 100, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
  SDL_Surface * winSurface = SDL_GetWindowSurface(win);

  GenesisCore core(argv[1]);

  std::vector<short> audioSamples;

  std::cout << "Using audio device : " << SDL_GetAudioDeviceName(0, 0) << std::endl;

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

  const Surface & vbuf = core.videoBuffer();

  SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(
    vbuf.pixels.get(),
    vbuf.width,
    vbuf.height,
    sizeof(pixel_t) * 8,
    vbuf.pitch(),
    0xFF,
    0xFF00,
    0xFF0000,
    0x00
  );

  SDL_Event event;
  size_t frameCtr = 0;
  const auto t0 = std::chrono::system_clock::now();
  while (true)
  {
    const auto t0Frame = std::chrono::high_resolution_clock::now();

    while(SDL_PollEvent(&event)) {
      switch (event.key.keysym.sym) {
        case SDLK_ESCAPE: {
          const auto t1 = std::chrono::system_clock::now();
          std::cout << 1000.0 * double(frameCtr) / double(std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()) << std::endl;
          return 0;
        }

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

    core.update();

    // TODO purge : circular buffer with fixed capacity!
    std::copy(core.audioBuffer().begin(), core.audioBuffer().end(), std::back_inserter(audioSamples));

    memcpy(surf->pixels, vbuf.pixels.get(), vbuf.height * vbuf.pitch());

    SDL_BlitScaled(surf, NULL, winSurface, NULL);    
    SDL_UpdateWindowSurface(win);

    frameCtr++;

    while (std::chrono::high_resolution_clock::now() - t0Frame < std::chrono::duration<double>(core.frameInterval())) {
      std::this_thread::yield();
    }
  }

  SDL_FreeSurface(surf);

  // // 4 times CPU upscale
  // uint32_t resizedVideoBuffer[(320*4)*(240*4)];
  // stbir_resize_uint8((const uint8_t *)videoBuffer, 320, 240, 0, (uint8_t *)resizedVideoBuffer, 320*4, 240*4, 0, 4);

  // stbi_write_tga("capture.tga", 320*4, 224*4, 4, resizedVideoBuffer);

  return 0;
}
