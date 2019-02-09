// Copyright 2019 Petra Baranski (progsource)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// -----------------------------------------------------------------------------

#pragma once

// -----------------------------------------------------------------------------

#include <string>

// -----------------------------------------------------------------------------

class SDL_Window;

// -----------------------------------------------------------------------------

namespace sbwrp {

// -----------------------------------------------------------------------------

struct Config
{
  struct Limits
  {
    uint16_t maxEncoders;
    uint32_t transientVbSize;
    uint32_t transientIbSize;

    Limits();
  };

  bool bgfxDebug;
  bool bgfxProfile;

  int x;
  int y;
  int w; // width
  int h; // height

  uint32_t sdlInitFlags;
  uint32_t sdlWindowFlags;

  std::string windowTitle;

  Limits bgfxLimits;

  Config();
};

// -----------------------------------------------------------------------------

/**
 * Create SDL window
 *
 * Initializes SDL2 and bgfx.
 *
 * @param {sbwrp::Config*} config
 * @return {SDL_Window*} if failed for any reason, it will return a nullptr
 */
SDL_Window* createWindow(Config* config);

/**
 * Destroy window
 *
 * Cleanup of bgfx and SDL2
 *
 * @param {SDL_Window*} window
 */
void destroyWindow(SDL_Window* window);

// -----------------------------------------------------------------------------

} // namespace sbwrp
