#include <iostream>
#include <SDL.h>
#include <SDL_main.h>

#include "Map.h"
#include "Window.h"

int main(int argc, char** argv) {
 
  SDL_Init(SDL_INIT_EVERYTHING);
  std::cout << "start" << std::endl;
 
//  Window w {720, 480};
  Window w {1080, 720};
//  Window w {2560, 1440};
  w.main_loop();
  
  
  std::cout << "end" << std::endl;
  return 0;
}