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

#include "SDL.h"
#include "bgfx/bgfx.h"

// -----------------------------------------------------------------------------

int main(int, char**)
{
  printf("sbwrp example\n");

  sbwrp::Config config;
  config.windowTitle = "sbwrp example";
  config.bgfxDebug = true;
  config.bgfxProfile = true;
  config.sdlInitFlags = SDL_INIT_VIDEO;
  config.sdlWindowFlags = SDL_WINDOW_SHOWN;
  config.x = 0;
  config.y = 0;
  config.w = 800;
  config.h = 640;

  SDL_Window* window = sbwrp::createWindow(&config);

  bool isRunning = true;
  SDL_Event event;

  while(isRunning)
  {
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          isRunning = false;
          break;
        case SDL_WINDOWEVENT:
        {
          SDL_WindowEvent wev = event.window;
          switch (wev.event)
          {
          case SDL_WINDOWEVENT_CLOSE:
            isRunning = false;
            break;
          }
          break;
        }
        case SDL_KEYUP:
        case SDL_KEYDOWN:
        {
          if (event.key.keysym.sym == SDLK_ESCAPE)
          {
            isRunning = false;
            break;
          }
        }
      }
    }

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xccffccff, 1.f, 0);
    bgfx::setViewRect(0, config.x, config.y, uint16_t(config.w), uint16_t(config.h));
    bgfx::touch(0);

    // do render smth. in here

    bgfx::frame(false);
  }

  sbwrp::destroyWindow(window);

  printf("sbwrp example has ended\n");

  return 0;
}
