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

#include "sbwrp/sbwrp.h"

// -----------------------------------------------------------------------------

#ifdef __linux__
  #define SDL_VIDEO_DRIVER_X11 1
#endif

// -----------------------------------------------------------------------------

#ifndef NDEBUG
  #include <iostream>
#endif

#include <vector>
#include <map>

// -----------------------------------------------------------------------------

#include "SDL.h"
#include "SDL_syswm.h"

#include "bx/bx.h"
#include "config.h" // from libs/bgfx/src
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

// -----------------------------------------------------------------------------

namespace sbwrp {

// -----------------------------------------------------------------------------

namespace {

// -----------------------------------------------------------------------------

#ifndef NDEBUG
  #define SBWRP_ASSERT(condition, message) \
    do \
    { \
      if (! (condition)) \
      { \
        std::cerr << "sbwrp Assertion `" #condition "` failed in " << __FILE__ \
                  << " (" << __LINE__ << "): " << message << "\n"; \
        std::terminate(); \
      } \
    } \
    while (false)

  #define SBWRP_LOG(message) \
    do \
    { \
      std::cout << "sbwrp Log in " << __FILE__ << " (" << __LINE__  \
                << "):\n\t" << message << "\n"; \
    } \
    while (false)
#else
  #define SBWRP_ASSERT(condition, message) do {} while (false)
  #define SBWRP_LOG(message) do {} while (false)
#endif

// -----------------------------------------------------------------------------

std::string
strVFormat(const char* fmt, va_list ap)
{
  size_t size = 1024;
  char buffer[1024];

  va_list apcopy;
  va_copy(apcopy, ap);
  int needed = vsnprintf(&buffer[0], size, fmt, ap);
  // On Windows, vsnprintf returns -1 if the string didn't fit the buffer.
  // On Linux & OSX, it returns the length it would have needed.

  if (needed <= static_cast<int>(size) && needed >= 0)
  {
    return std::string(&buffer[0]);
  }
  else
  {
    std::vector<char> buffer;
    size = needed;
    buffer.resize(size);
    needed = vsnprintf(&buffer[0], size, fmt, apcopy);
    return std::string(&buffer[0]);
  }
}

std::string
strFormat(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  std::string buffer = strVFormat(fmt, ap);
  va_end(ap);
  return buffer;
}

// -----------------------------------------------------------------------------

bgfx::PlatformData
getBgfxPlatformData(SDL_Window* window)
{
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);

  bool hasSuccessSdlGetWMInfo = SDL_GetWindowWMInfo(window, &wmi);
  SBWRP_ASSERT(hasSuccessSdlGetWMInfo, "SDL_GetWindowWMInfo failed");

  const std::map<int, std::string> platformNameMap = {
    {SDL_SYSWM_UNKNOWN,  "SDL_SYSWM_UNKNOWN"},
    {SDL_SYSWM_WINDOWS,  "SDL_SYSWM_WINDOWS"},
    {SDL_SYSWM_X11,      "SDL_SYSWM_X11"},
    {SDL_SYSWM_DIRECTFB, "SDL_SYSWM_DIRECTFB"},
    {SDL_SYSWM_COCOA,    "SDL_SYSWM_COCOA"},
    {SDL_SYSWM_UIKIT,    "SDL_SYSWM_UIKIT"},
    {SDL_SYSWM_WAYLAND,  "SDL_SYSWM_WAYLAND"},
    {SDL_SYSWM_MIR,      "SDL_SYSWM_MIR"},
    {SDL_SYSWM_WINRT,    "SDL_SYSWM_WINRT"},
    {SDL_SYSWM_ANDROID,  "SDL_SYSWM_ANDROID"},
    {SDL_SYSWM_VIVANTE,  "SDL_SYSWM_VIVANTE"},
    {SDL_SYSWM_OS2,      "SDL_SYSWM_OS2"}
  };

  SBWRP_LOG("render platform: " << platformNameMap.at(wmi.subsystem));

  SBWRP_ASSERT(
    wmi.subsystem != SDL_SYSWM_UNKNOWN &&
    wmi.subsystem != SDL_SYSWM_DIRECTFB &&
    wmi.subsystem != SDL_SYSWM_UIKIT &&
    wmi.subsystem != SDL_SYSWM_WAYLAND &&
    wmi.subsystem != SDL_SYSWM_MIR &&
    wmi.subsystem != SDL_SYSWM_WINRT &&
    wmi.subsystem != SDL_SYSWM_ANDROID &&
    wmi.subsystem != SDL_SYSWM_OS2,
    strFormat(
      "The backend renderer %s is not yet supported by sbwrp.",
      platformNameMap.at(wmi.subsystem)
    )
  );

  bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  pd.ndt = wmi.info.x11.display;
  pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
  pd.ndt = NULL;
  pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
  pd.ndt = NULL;
  pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_STEAMLINK
  pd.ndt = wmi.info.vivante.display;
  pd.nwh = wmi.info.vivante.window;
#endif // BX_PLATFORM_
  pd.context = NULL;
  pd.backBuffer = NULL;
  pd.backBufferDS = NULL;

  return pd;
}

// -----------------------------------------------------------------------------

} // empty namesapce

// -----------------------------------------------------------------------------

Config::Limits::Limits()
  : maxEncoders(BGFX_CONFIG_DEFAULT_MAX_ENCODERS)
  , transientVbSize(BGFX_CONFIG_TRANSIENT_VERTEX_BUFFER_SIZE)
  , transientIbSize(BGFX_CONFIG_TRANSIENT_INDEX_BUFFER_SIZE)
{
}

Config::Config()
  : bgfxDebug(false)
  , bgfxProfile(false)
  , x(0)
  , y(0)
  , w(600)
  , h(400)
  , sdlInitFlags(SDL_INIT_EVERYTHING)
  , sdlWindowFlags(SDL_WINDOW_SHOWN)
  , windowTitle("sbwrp window")
{
}

// -----------------------------------------------------------------------------

SDL_Window*
createWindow(Config* config)
{
  if (SDL_Init(config->sdlInitFlags) < 0)
  {
    SBWRP_ASSERT(
      false,
      strFormat("SDL could not initialize! SDL_Error: %s\n", SDL_GetError())
    );
    return nullptr;
  }

  SBWRP_LOG("SDL initialized");

  SDL_Window* window = SDL_CreateWindow(
    config->windowTitle.c_str(),
    config->x,
    config->y,
    config->w,
    config->h,
    config->sdlWindowFlags
  );

  if (window == nullptr)
  {
    SBWRP_ASSERT(
      window != nullptr,
      strFormat(
        "SDL_Window could not be created! SDL_Error: %s\n",
        SDL_GetError()
      )
    );
    return nullptr;
  }

  bgfx::Init initConfig;
  initConfig.platformData = getBgfxPlatformData(window);
  initConfig.resolution.width = config->w;
  initConfig.resolution.height = config->h;
  initConfig.debug = config->bgfxDebug;
  initConfig.profile = config->bgfxProfile;
  initConfig.limits.maxEncoders = config->bgfxLimits.maxEncoders;
  initConfig.limits.transientVbSize = config->bgfxLimits.transientVbSize;
  initConfig.limits.transientIbSize = config->bgfxLimits.transientIbSize;

  bgfx::init(initConfig);

  bgfx::reset(config->w, config->h, BGFX_RESET_VSYNC);
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xccffccff, 1.f, 0);

  return window;
}

void
destroyWindow(SDL_Window* window)
{
  bgfx::shutdown();
  while (bgfx::RenderFrame::NoContext != bgfx::renderFrame())
  {}

  SDL_DestroyWindow(window);
  SDL_Quit();
}

// -----------------------------------------------------------------------------

} // namespace sbwrp
