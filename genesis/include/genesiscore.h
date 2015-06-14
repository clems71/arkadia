#pragma once

#include <vector>

#include "surface.h"

enum class Players {
  Player1 = 0,
  Player2 = 1,
};

enum class Buttons {
  Up,
  Down,
  Left, 
  Right,
  A,
  B,
  C,
  Start,
  COUNT
};

class GenesisCore
{
public:
  GenesisCore(const char * rompath);
  ~GenesisCore();

  void update();

  //! @brief Return the video framebuffer for the newly computed frame
  inline const Surface & videoBuffer() const {
    return video_buffer_;
  }

  //! @brief Return the audio samples for the newly computed frame
  inline const std::vector<short> & audioBuffer() const {
    return audio_buffer_frame_;
  }

  void buttonPressed(Players p, Buttons b);

  void buttonReleased(Players p, Buttons b);

  //! @brief Frame interval in seconds
  double frameInterval() const;

private:
  Surface video_buffer_;
  std::vector<short> audio_buffer_frame_;
  std::unique_ptr<short []> audio_buffer_;
};
