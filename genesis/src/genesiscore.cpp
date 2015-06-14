#include <iostream>
#include <stdexcept>

#include <genesiscore.h>

extern "C" {
  #include "shared.h"
  #include "scrc32.h"
}

static const double pal_fps = 53203424.0 / (3420.0 * 313.0);
static const double ntsc_fps = 53693175.0 / (3420.0 * 262.0);

char GG_ROM[256];
char AR_ROM[256];
char SK_ROM[256];
char SK_UPMEM[256];
char GG_BIOS[256];
char MS_BIOS_EU[256];
char MS_BIOS_JP[256];
char MS_BIOS_US[256];
char CD_BIOS_EU[256];
char CD_BIOS_US[256];
char CD_BIOS_JP[256];
char CD_BRAM_JP[256];
char CD_BRAM_US[256];
char CD_BRAM_EU[256];
char CART_BRAM[256];

// Mega CD backup RAM stuff
static uint32_t brm_crc[2];
static uint8_t brm_format[0x40] =
{
    0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x00,0x00,0x00,0x00,0x40,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x53,0x45,0x47,0x41,0x5f,0x43,0x44,0x5f,0x52,0x4f,0x4d,0x00,0x01,0x00,0x00,0x00,
    0x52,0x41,0x4d,0x5f,0x43,0x41,0x52,0x54,0x52,0x49,0x44,0x47,0x45,0x5f,0x5f,0x5f
};

void osd_input_update(void)
{
    /* Update RAM patches */
    // RAMCheatUpdate();
}

#define CHUNKSIZE   (0x10000)

int load_archive(char *filename, unsigned char *buffer, int maxsize, char *extension)
{
    int size, left;

    /* Open file */
    FILE *fd = fopen(filename, "rb");

    if (!fd)
    {
        /* Master System & Game Gear BIOS are optional files */
        if (!strcmp(filename,MS_BIOS_US) || !strcmp(filename,MS_BIOS_EU) || !strcmp(filename,MS_BIOS_JP) || !strcmp(filename,GG_BIOS))
        {
            return 0;
        }

        /* Mega CD BIOS are required files */
        if (!strcmp(filename,CD_BIOS_US) || !strcmp(filename,CD_BIOS_EU) || !strcmp(filename,CD_BIOS_JP))
        {
            fprintf(stderr, "ERROR - Unable to open CD BIOS: %s.\n", filename);
            return 0;
        }

        fprintf(stderr, "ERROR - Unable to open file.\n");
        return 0;
    }

    /* Get file size */
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);

    /* size limit */
    if(size > maxsize)
    {
        fclose(fd);
        fprintf(stderr, "ERROR - File is too large.\n");
        return 0;
    }

    fprintf(stderr, "INFORMATION - Loading %d bytes ...\n", size);

    /* filename extension */
    if (extension)
    {
        memcpy(extension, &filename[strlen(filename) - 3], 3);
        extension[3] = 0;
    }

    /* Read into buffer */
    left = size;
    fseek(fd, 0, SEEK_SET);
    while (left > CHUNKSIZE)
    {
        fread(buffer, CHUNKSIZE, 1, fd);
        buffer += CHUNKSIZE;
        left -= CHUNKSIZE;
    }

    /* Read remaining bytes */
    fread(buffer, left, 1, fd);

    /* Close file */
    fclose(fd);

    /* Return loaded ROM size */
    return size;
}

GenesisCore::GenesisCore(const char * rompath) 
: video_buffer_(320, 224)
, audio_buffer_(new short[2048*2])
{
  /* sound options */
  config.psg_preamp     = 150;
  config.fm_preamp      = 100;
  config.hq_fm          = 1; /* high-quality resampling */
  config.psgBoostNoise  = 1;
  config.filter         = 0; /* no filter */
  config.lp_range       = 0x9999; /* 0.6 in 16.16 fixed point */
  config.low_freq       = 880;
  config.high_freq      = 5000;
  config.lg             = 1.0;
  config.mg             = 1.0;
  config.hg             = 1.0;
  config.dac_bits       = 14; /* MAX DEPTH */
  config.ym2413         = 2; /* AUTO */
  config.mono           = 0; /* STEREO output */

  /* system options */
  config.system         = 0; /* AUTO */
  config.region_detect  = 0; /* AUTO */
  config.vdp_mode       = 0; /* AUTO */
  config.master_clock   = 0; /* AUTO */
  config.force_dtack    = 0;
  config.addr_error     = 1;
  config.bios           = 0;
  config.lock_on        = 0;

  /* video options */
  config.overscan = 0; /* 3 == FULL */
  config.gg_extra = 0; /* 1 = show extended Game Gear screen (256x192) */
  config.ntsc     = 0;
  config.render   = 0;

  /* initialize bitmap */
  memset(&bitmap, 0, sizeof(bitmap));
  bitmap.width      = video_buffer_.width;
  bitmap.height     = video_buffer_.height;
  bitmap.pitch      = video_buffer_.pitch();
  bitmap.data       = (unsigned char*)video_buffer_.pixels.get();

  // Init input system
  input.system[0] = SYSTEM_GAMEPAD;
  input.system[1] = SYSTEM_GAMEPAD;
  for (unsigned i = 0; i < 2; i++)
      config.input[i].padtype = DEVICE_PAD2B | DEVICE_PAD3B | DEVICE_PAD6B;


  if (!load_rom((char *)rompath))
    throw std::runtime_error("cannot load ROM");

  // DEBUG TRACE
  printf("SPECIAL : %d\n", cart.special);
  printf("SIZE : %d\n", cart.romsize);
  printf("COMPANY : %s\n", get_company());

  audio_init(48000, vdp_pal ? pal_fps : ntsc_fps);

  system_init();
  system_reset();

  bitmap.viewport.w = video_buffer_.width;
  bitmap.viewport.h = video_buffer_.height;
}

GenesisCore::~GenesisCore() 
{
}

void GenesisCore::update()
{
  if (system_hw == SYSTEM_MCD)
    system_frame_scd(0);
  else if ((system_hw & SYSTEM_PBC) == SYSTEM_MD)
    system_frame_gen(0);
  else
    system_frame_sms(0);


  // Swizzle BGRA -> RGBA
  for (size_t i=0; i<video_buffer_.width*video_buffer_.height; i++) 
  {
    const pixel_t pixSrc = video_buffer_.pixels[i];
    pixel_t pixOut;
    pixOut.r = pixSrc.b;
    pixOut.g = pixSrc.g;
    pixOut.b = pixSrc.r;
    video_buffer_.pixels[i] = pixOut;
  }

  // int samples = audio_update(soundBuffer);
  const auto sz = audio_update(audio_buffer_.get()) * 2; // Stereo
  audio_buffer_frame_.clear();
  std::copy(audio_buffer_.get(), audio_buffer_.get() + sz, std::back_inserter(audio_buffer_frame_));
}

double GenesisCore::frameInterval() const
{
  return vdp_pal ? (1.0 / pal_fps) : (1.0 / ntsc_fps);
}

namespace {

  const size_t ButtonsToGenPlus[int(Buttons::COUNT)] = {
    INPUT_UP, INPUT_DOWN, INPUT_LEFT, INPUT_RIGHT, INPUT_A, INPUT_B, INPUT_C, INPUT_START 
  };

} // anonymous namespace

void GenesisCore::buttonPressed(Players p, Buttons b) {
  input.pad[int(p) << 2] |= ButtonsToGenPlus[int(b)];
}

void GenesisCore::buttonReleased(Players p, Buttons b) {
  input.pad[int(p) << 2] &= ~ButtonsToGenPlus[int(b)];
}
