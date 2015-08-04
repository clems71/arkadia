#include "core.h"

#include <dlfcn.h>
#include <iostream>
#include <map>
#include <tuple>

#include "retro.h"


namespace
{

  struct CoreState
  {
    CoreState(const std::string & corePath)
    {
      dlHandle = dlopen(corePath.c_str(), RTLD_LAZY);
    }

    ~CoreState()
    {
      if (dlHandle) dlclose(dlHandle);
    }

    void * dlHandle = nullptr;
    SettingsDesc settingsDesc;
    std::map<std::string, std::string> settings;
    double fps = 0.0;
    double audioSampleRate = 0.0;
    retro_pixel_format format = RETRO_PIXEL_FORMAT_UNKNOWN;
    size_t width = 0;
    size_t height = 0;
    std::vector<uint32_t> videoBuf;
    std::vector<int16_t> audioBuf;

    typedef std::tuple<size_t, size_t, size_t> JoypadId;
    typedef std::map<JoypadId, std::string> JoypadDesc;
    std::vector<JoypadDesc> joypads;

    typedef std::map<std::string, bool> JoypadState;
    std::vector<JoypadState> joypadsState;
  };

  std::unique_ptr<CoreState> gCoreState;

} // anonymous namespace


std::string settingsName(const std::string & val)
{
  return std::string(val.begin(), std::find(val.begin(), val.end(), ';'));
}

std::vector<std::string> settingsChoices(const std::string & val)
{
  std::vector<std::string> result;
  const char * p = val.c_str();
  while (*p++ != ';');
  while (*p) {
    const char * pStart = ++p;
    while (*p != 0x00 && *p != '|') p++;
    result.push_back(std::string(pStart, p));
  }
  return result;
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
        SettingsEntryDesc sed = SettingsEntryDesc {
          .key = variables->key,
          .name = settingsName(variables->value),
          .choices = settingsChoices(variables->value),
        };
        gCoreState->settingsDesc.push_back(sed);
        gCoreState->settings[sed.key] = sed.choices[0];
        variables++;
      }
      return true;
    }

    case RETRO_ENVIRONMENT_GET_VARIABLE: {
      retro_variable * variable = (retro_variable *)data;
      variable->value = gCoreState->settings[variable->key].c_str();
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
      gCoreState->format = *(retro_pixel_format *)data;
      std::cout << "Format is " << gCoreState->format << std::endl;
      return true;
    }

    case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO: {
      const retro_system_av_info * avInfo = (retro_system_av_info*)data;
      gCoreState->fps = avInfo->timing.fps;
      gCoreState->audioSampleRate = avInfo->timing.sample_rate;
      return true;
    }

    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE: {
      retro_log_callback * logInfo = (retro_log_callback *)data;
      logInfo->log = &retro_log;
      return true;
    }

    case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO: {
      // TODO: Use this info!!!
      retro_controller_info * ctrlInfo = (retro_controller_info *)data;
      for (size_t i=0; i<ctrlInfo->num_types; i++) {
        const auto type = ctrlInfo->types[i];
        std::cout << "Controller type : " << type.desc << std::endl;
      }
      return true;
    }

    case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS: {
      retro_input_descriptor * inputDesc = (retro_input_descriptor *)data;
      size_t i = 0;
      while (inputDesc[i].description) {
        const auto & cur = inputDesc[i];

        // if (!cur.port) {
        //   std::cout << cur.description << '-' << cur.device << '-' << cur.index << '-' << cur.id << std::endl;
        // }

        // Set joypad desc
        gCoreState->joypads.resize(cur.port + 1);
        gCoreState->joypads[cur.port][std::make_tuple(cur.device, cur.id, cur.index)] = cur.description;

        // Set default state
        gCoreState->joypadsState.resize(cur.port + 1);
        gCoreState->joypadsState[cur.port][cur.description] = false;
        i++;
      }
      return true;
    }

    default:
      // std::cout << ">> COMMAND = " << cmd << std::endl;
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
  // Bind a dynamic library function easily (casting done for you)
  template<typename FType>
  void coreBind(FType & f, const std::string & funcName)
  {
    f = (FType)dlsym(gCoreState->dlHandle, funcName.c_str());
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
    gCoreState->width = width;
    gCoreState->height = height;
    gCoreState->videoBuf.resize(width * height);

    switch (gCoreState->format) {
      case RETRO_PIXEL_FORMAT_RGB565:
        for (size_t y=0; y<height; y++) {
          for (size_t x=0; x<width; x++) {
            gCoreState->videoBuf[width * y + x] = rgb565_to_xrgb8888(*(uint16_t*)&vData[x * 2]);
          }
          vData += pitch;
        }
        break;
      case RETRO_PIXEL_FORMAT_XRGB8888:
        for (size_t y=0; y<height; y++) {
          for (size_t x=0; x<width; x++) {
            gCoreState->videoBuf[width * y + x] = xbgr8888_to_xrgb8888(*(uint32_t*)&vData[x * 4]);
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
    std::cerr << __FUNCTION__ << " : NYI" << std::endl;
  }

  size_t retro_audio_sample_batch(const int16_t *data, size_t frames)
  {
    for (size_t i=0; i<frames*2; i++) {
      gCoreState->audioBuf.push_back(*data++);
    }
    return frames;
  }

  void retro_input_poll(void)
  {
    // std::cout << "Poll" << std::endl;
  }

  int16_t retro_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
  {
    return gCoreState->joypadsState[port][gCoreState->joypads[port][std::make_tuple(device, id, index)]] ? 1 : 0;
  }

} // anonymous namespace

void coreClose()
{
  gCoreState.reset();
}

void coreInit(const std::string & corePath)
{
  coreClose(); // Close any previously opened core
  gCoreState.reset(new CoreState(corePath));

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

  retro_system_av_info avInfo;
  retro_get_system_av_info(&avInfo);
  gCoreState->fps = avInfo.timing.fps;
  gCoreState->audioSampleRate = avInfo.timing.sample_rate;
}

void coreUpdate()
{
  retro::run();
}

const std::vector<uint32_t> & coreVideoData(size_t & width, size_t & height)
{
  width = gCoreState->width;
  height = gCoreState->height;
  return gCoreState->videoBuf;
}

std::vector<int16_t> coreAudioData()
{
  const auto res = gCoreState->audioBuf;
  gCoreState->audioBuf.clear();
  return res;
}

void coreTimings(double & fps, double & audioSampleRate)
{
  fps = gCoreState->fps;
  audioSampleRate = gCoreState->audioSampleRate;
}

SettingsDesc coreSettingsDesc()
{
  return gCoreState->settingsDesc;
}

void coreSettingsSet(const std::string & key, const std::string & value)
{
  gCoreState->settings[key] = value;
}

std::vector<std::string> coreJoypadDesc()
{
  std::vector<std::string> result;
  if (gCoreState->joypads.empty()) return result;
  for (const auto & pair : gCoreState->joypads[0]) {
    result.push_back(pair.second);
  }
  return result;
}

void coreJoypadPress(const std::string & name)
{
  gCoreState->joypadsState[0][name] = true;
}

void coreJoypadRelease(const std::string & name)
{
  gCoreState->joypadsState[0][name] = false;
}
