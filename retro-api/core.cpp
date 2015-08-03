#include "core.h"

#include <dlfcn.h>
#include <iostream>
#include <map>

#include "retro.h"

std::map<std::string, std::string> gVariables;
retro_pixel_format gFormat;

std::string takeFirstValue(const std::string & val)
{
  const char * p = val.c_str();
  while (*p++ != ';');
  p++; // Whitespace to skip
  const char * pStart = p;
  while (*p++ != '|');
  const char * pEnd = p;
  std::string res(pStart, pEnd-1);
  // cout << res << endl;
  return res;
}

#define CORE_LIBRARY_BIND(name) \
  coreBind(retro::name, "retro_" #name)

#define CORE_LIBRARY_DECL(name) \
  namespace retro { decltype(retro_ ## name) *  name = nullptr; }

const char * SAVE_DIR = "./";
const char * ASSET_DIR = "./";
const char * SYS_DIR = "./bios";

void retro_log(enum retro_log_level lv, const char *fmt, ...)
{
  std::string level = "RETRO_LOG_DEBUG";

  switch (lv) {
    case RETRO_LOG_INFO:
      level = "RETRO_LOG_INFO";
      break;
    case RETRO_LOG_WARN:
      level = "RETRO_LOG_WARN";
      break;
    case RETRO_LOG_ERROR:
      level = "RETRO_LOG_ERROR";
      break;
  }

  printf("[%s] ", level.c_str());

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

bool retro_environment(unsigned cmd, void * data)
{
  switch (cmd) {
    case RETRO_ENVIRONMENT_SET_VARIABLES: {
      retro_variable * variables = (retro_variable *)data;
      while (variables->key) {
        gVariables[variables->key] = takeFirstValue(variables->value);
        std::cout << variables->key << std::endl;
        std::cout << variables->value << std::endl;
        variables++;
      }
      return true;
    }

    case RETRO_ENVIRONMENT_GET_VARIABLE: {
      retro_variable * variable = (retro_variable *)data;
      variable->value = gVariables[variable->key].c_str();
      return true;
    }

    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: {
      const char ** ppath = (const char **)data;
      *ppath = SYS_DIR;
      return true;
    }

    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY: {
      const char ** ppath = (const char **)data;
      *ppath = SAVE_DIR;
      return true;
    }

    case RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY: {
      const char ** ppath = (const char **)data;
      *ppath = ASSET_DIR;
      return true;
    }

    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: {
      gFormat = *(retro_pixel_format *)data;
      std::cout << "Format is " << gFormat << std::endl;
      return true;
    }

    case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO: {
      const retro_system_av_info * avInfo = (retro_system_av_info*)data;
      return true;
    }

    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE: {
      retro_log_callback * logInfo = (retro_log_callback *)data;
      logInfo->log = &retro_log;
      return true;
    }

    default:
      std::cout << ">> COMMAND = " << cmd << std::endl;
      return false;
  }
}

CORE_LIBRARY_DECL(init);
CORE_LIBRARY_DECL(run);
CORE_LIBRARY_DECL(load_game);
CORE_LIBRARY_DECL(set_environment);
CORE_LIBRARY_DECL(set_video_refresh);
CORE_LIBRARY_DECL(set_audio_sample);
CORE_LIBRARY_DECL(set_audio_sample_batch);
CORE_LIBRARY_DECL(set_input_poll);
CORE_LIBRARY_DECL(set_input_state);
CORE_LIBRARY_DECL(get_system_info);

namespace
{
  void * gDlHandle = nullptr;
  size_t gWidth = 0;
  size_t gHeight = 0;
  std::vector<uint32_t> gVideoBuf;

  // Bind a dynamic library function easily (casting done for you)
  template<typename FType>
  void coreBind(FType & f, const std::string & funcName)
  {
    f = (FType)dlsym(gDlHandle, funcName.c_str());
    if (f == nullptr) std::cerr << "Cannot load " << funcName << std::endl;
  }

  inline uint32_t rgb565_to_xrgb8888(uint16_t val)
  {
    const uint32_t R5 = val & 0x1f; val >>= 5;
    const uint32_t G6 = val & 0x3f; val >>= 6;
    const uint32_t B5 = val & 0x1f;

    const uint32_t R8 = ( R5 * 527 + 23 ) >> 6;
    const uint32_t G8 = ( G6 * 259 + 33 ) >> 6;
    const uint32_t B8 = ( B5 * 527 + 23 ) >> 6;

    return B8 + (G8 << 8) + (R8 << 16) + 0xFF000000;
  }

  inline uint32_t xbgr8888_to_xrgb8888(uint32_t val)
  {
    const uint32_t R8 = val & 0xFF; val >>= 8;
    const uint32_t G8 = val & 0xFF; val >>= 8;
    const uint32_t B8 = val & 0xFF;
    return B8 + (G8 << 8) + (R8 << 16) + 0xFF000000;
  }

  void retro_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
  {
    // std::cout << width << 'x' << height << " - " << pitch << std::endl;

    const uint8_t * vData = (const uint8_t *)data;
    gWidth = width;
    gHeight = height;
    gVideoBuf.resize(width * height);

    switch (gFormat) {
      case RETRO_PIXEL_FORMAT_RGB565:
        for (size_t y=0; y<height; y++) {
          for (size_t x=0; x<width; x++) {
            gVideoBuf[width * y + x] = rgb565_to_xrgb8888(*(uint16_t*)&vData[x * 2]);
          }
          vData += pitch;
        }
        break;
      case RETRO_PIXEL_FORMAT_XRGB8888:
        for (size_t y=0; y<height; y++) {
          for (size_t x=0; x<width; x++) {
            gVideoBuf[width * y + x] = xbgr8888_to_xrgb8888(*(uint32_t*)&vData[x * 4]);
          }
          vData += pitch;
        }
        break;
      default:
        break;
    }
  }

  void retro_audio_sample(int16_t left, int16_t right)
  {

  }

  size_t retro_audio_sample_batch(const int16_t *data, size_t frames)
  {
    return 0;
  }

  void retro_input_poll(void)
  {
  }

  int16_t retro_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
  {
    return 0;
  }

} // anonymous namespace

void coreClose()
{
  if (gDlHandle) dlclose(gDlHandle);
  gDlHandle = nullptr;
}

void coreInit(const std::string & corePath)
{
  coreClose(); // Close any previously opened core
  gDlHandle = dlopen(corePath.c_str(), RTLD_LAZY);

  CORE_LIBRARY_BIND(init);
  CORE_LIBRARY_BIND(run);
  CORE_LIBRARY_BIND(load_game);
  CORE_LIBRARY_BIND(set_environment);
  CORE_LIBRARY_BIND(set_video_refresh);
  CORE_LIBRARY_BIND(set_audio_sample);
  CORE_LIBRARY_BIND(set_audio_sample_batch);
  CORE_LIBRARY_BIND(set_input_poll);
  CORE_LIBRARY_BIND(set_input_state);
  CORE_LIBRARY_BIND(get_system_info);

  retro::set_environment(&retro_environment);
  retro::set_video_refresh(&retro_video_refresh);
  retro::set_audio_sample(&retro_audio_sample);
  retro::set_audio_sample_batch(&retro_audio_sample_batch);
  retro::set_input_poll(&retro_input_poll);
  retro::set_input_state(&retro_input_state);
  retro::init();

  retro_system_info info;
  retro::get_system_info(&info);
  std::cout << info.library_name << " - " << info.library_version << std::endl;
}

void coreLoadGame(const std::string & romPath)
{
  retro_game_info gi;
  gi.path = romPath.c_str();
  gi.data = NULL;
  gi.size = 0;
  gi.meta = NULL;
  retro::load_game(&gi);
}

void coreUpdate()
{
  retro::run();
}

const std::vector<uint32_t> & coreVideoData(size_t & width, size_t & height)
{
  width = gWidth;
  height = gHeight;
  return gVideoBuf;
}
