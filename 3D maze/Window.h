
#ifndef WINDOW_H_
#define WINDOW_H_

#include <SDL.h>
#include <memory>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Map.h"
#include "player.h"
#include "Texture.h"

class Window final {

public:
  static constexpr int    CELL_SIZE         = 10;
  static constexpr int    MAP_OFFSET        = 20;
  static constexpr int    PLAYER_SIZE       = 7;
  static constexpr int    PLAYER_ARROW      = CELL_SIZE;
  static constexpr double PLAYER_WALK_DIST  = 0.03;
  static constexpr double PLAYER_TURN_ANGLE = 0.05;
  static constexpr double MOUSE_SENS        = 0.001;
  
  static constexpr double WALL_HEIGHT = 0.5;

private:
  int                         _width, _height;
  std::shared_ptr<SDL_Window> _window;
  std::shared_ptr<void>       _gl_context;
  
  Player               _player;
  std::shared_ptr<Map> _map;
  
  std::shared_ptr<Texture> _wall_tex;
  std::shared_ptr<Texture> _floor_tex;
  std::shared_ptr<Texture> _ceiling_tex;
  
  struct {
    bool walk_forward{false};
    bool walk_back{false};
    bool shift_right{false};
    bool shift_left{false};
    bool turn_right{false};
    bool turn_left{false};
    bool tilt_up{false};
    bool tilt_down{false};
  }                        _controls;
public:
  Window(int w, int h);
  void main_loop();
  void render();
  void event(const SDL_Event &e);
  void update();
  void create_gl_context();
  void create_window(int w, int h);
  void setup_gl();
  void draw_view() const;
  void draw_minimap();
  void draw_wall() const;
  void draw_floor() const;
};
#endif /* WINDOW_H_ */