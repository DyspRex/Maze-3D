#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
#pragma ide diagnostic ignored "readability-make-member-function-const"

#include "Window.h"
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <SDL.h>
#include <SDL_image.h>

static constexpr double Pi = acos(-1.0);

Window::Window(int w, int h) : _width(w), _height(h) {
  create_window(w, h);
  create_gl_context();
  
  _wall_tex    = std::make_shared<Texture>("wall.jpg");
  _floor_tex   = std::make_shared<Texture>("floor.jpg");
  _ceiling_tex = std::make_shared<Texture>("ceiling.jpg");
  
  _map = std::make_shared<Map>("map.txt");
  _player.spawn(_map);
  
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Window::create_window(int w, int h) {
  
  _window = std::shared_ptr<SDL_Window>(
          SDL_CreateWindow("MAZE 3D window",
                           SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED,
                           w, h, SDL_WINDOW_OPENGL),
          SDL_DestroyWindow);
  if (_window == nullptr)
    throw std::runtime_error(
            std::string("Can't create a window: ") +
            std::string(SDL_GetError()));
}

void Window::draw_minimap() {
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, _width, _height, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  double map_width  = _map->width()*CELL_SIZE + MAP_OFFSET*2;
  double map_height = _map->height()*CELL_SIZE + MAP_OFFSET*2;
  
  glBegin(GL_QUADS);
  glColor4d(0.0, 0.200, 0.250, 0.3700);
  glVertex2d(0.0, 0.0);
  glVertex2d(map_width, 0);
  glVertex2d(map_width, map_width);
  glVertex2d(0, map_height);
  
  for (int y = 0; y < _map->height(); ++y) {
    for (int x = 0; x < _map->width(); ++x) {
      double x1 = MAP_OFFSET + (x + 0)*CELL_SIZE;
      double x2 = MAP_OFFSET + (x + 1)*CELL_SIZE;
      double y1 = MAP_OFFSET + (y + 0)*CELL_SIZE;
      double y2 = MAP_OFFSET + (y + 1)*CELL_SIZE;
      if (_map->is_wall(x, y)) {
        if (_map->get(x, y) == '@')
          glColor4d(0.0, 0.5, 0.0, 1.0);
        else
          glColor4d(0.0, 0.500, 0.550, 1.0);
        glVertex2d(x1, y1);
        glVertex2d(x2, y1);
        glVertex2d(x2, y2);
        glVertex2d(x1, y2);
      }
    }
  }
  glEnd();
  
  int x1, y1, x2, y2;
  x1 = MAP_OFFSET + CELL_SIZE*_player.x();
  y1 = MAP_OFFSET + CELL_SIZE*_player.y();
  x2 = x1 + PLAYER_ARROW*cos(_player.dir());
  y2 = y1 + PLAYER_ARROW*sin(_player.dir());
  
  glPointSize(PLAYER_SIZE);
  glBegin(GL_POINTS);
  glColor4d(0.0, 1.0, 0.0, 1.0);
  glVertex2d(x1, y1);
  glEnd();
  
  glLineWidth(PLAYER_SIZE/3.0);
  glBegin(GL_LINES);
  glColor4d(0.0, 0.0, 0.5, 0.0);
  glColor4d(0.8, 0.8, 0.9, 1.0);
  glVertex2d(x1, y1);
  glVertex2d(x2, y2);
  glEnd();
  
  glLineWidth(3.0);
  glBegin(GL_LINE_LOOP);
  glEnd();
//  SDL_Rect cr{
//          MAP_OFFSET + x*CELL_SIZE,
//          MAP_OFFSET + y*CELL_SIZE,
//          CELL_SIZE, CELL_SIZE};
//  if (_map->is_wall(x, y))
//    SDL_RenderFillRect(r, &cr);
//}
//}

//SDL_Rect pr{x1 - PLAYER_SIZE/2,
//            y1 - PLAYER_SIZE/2,
//            PLAYER_SIZE, PLAYER_SIZE};
//SDL_SetRenderDrawColor(r,
//0, 255, 0, 255);
//SDL_RenderFillRect(r, &pr
//);
//
//SDL_SetRenderDrawColor(r,
//0, 255, 0, 255);
//SDL_RenderDrawLine(r, x1, y1, x2, y2
//); // TODO Был FILL line
}

void Window::create_gl_context() {
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  
  _gl_context = std::shared_ptr<void>(
          SDL_GL_CreateContext(_window.get()), SDL_GL_DeleteContext);
  if (_window == nullptr)
    throw std::runtime_error(
            std::string("Can't create an OpenGL context: ") + std::string(SDL_GetError()));
}

void Window::main_loop() {
  
  bool      want_quit{false};
  SDL_Event e;
  
  auto update_thread = std::thread([&]() {
      using clk = std::chrono::high_resolution_clock;
      auto delay     = std::chrono::microseconds(1'000'000/60);
      auto next_time = clk::now() + delay;
      while (not want_quit) {
        std::this_thread::sleep_until(next_time);
        next_time += delay;
        update();
      }
  });
  
  setup_gl();
  for (;;) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        want_quit = true;
        update_thread.join();
        return;
      }
      event(e);
    }
    render();
    SDL_GL_SwapWindow(_window.get());
  }
}
void Window::setup_gl() {
  glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
  glEnable(GL_TEXTURE);
}
void Window::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_view();
  draw_minimap();
}
void Window::draw_view() const {
  static double a = 0.0;
  a += 2.0;
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, double(_width)/double(_height), 0.001, 30.0);
  
  double x1, y1, z1, x2, y2, z2;
  x1 = _player.x();
  y1 = _player.y();
  z1 = WALL_HEIGHT/2;
  x2 = x1 + cos(_player.dir())*sin(_player.tilt());
  y2 = y1 + sin(_player.dir())*sin(_player.tilt());
  z2 = z1 + cos(_player.tilt());
  
  
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(y1, x1, z1,
            y2, x2, z2,
            0.0, 0.0, 1.0);
  
  for (int x = 0; x < _map->width(); ++x) {
    for (int y = 0; y < _map->height(); ++y) {
      bool is_wall = _map->is_wall(y, x);
      glPushMatrix();
      glTranslated(x, y, 0.0);
      if (is_wall)
        draw_wall();
      else {
        draw_floor();
      }
      glPopMatrix();
    }
  }
}
void Window::draw_floor() const {
  
  _floor_tex->bind();
  glBegin(GL_QUADS);
  glColor4d(1.0, 1.0, 1.0, 1.0);
  glTexCoord2d(0.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0);
  glTexCoord2d(1.0, 0.0);
  glVertex3d(0.0, 1.0, 0.0);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(1.0, 1.0, 0.0);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(1.0, 0.0, 0.0);
  glEnd();
  
  _ceiling_tex->bind();
  glBegin(GL_QUADS);
  
  glTexCoord2d(0.0, 0.0);
  glVertex3d(0.0, 0.0, WALL_HEIGHT);
  glTexCoord2d(1.0, 0.0);
  glVertex3d(0.0, 1.0, WALL_HEIGHT);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(1.0, 1.0, WALL_HEIGHT);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(1.0, 0.0, WALL_HEIGHT);
  
  glEnd();
}
void Window::draw_wall() const {
  
  _wall_tex->bind();
  glBegin(GL_QUADS);
  glColor4d(1.0, 1.0, 1.0, 1.0);
  
  glTexCoord2d(0.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0);
  glTexCoord2d(1.0, 0.0);
  glVertex3d(1.0, 0.0, 0.0);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(1.0, 0.0, WALL_HEIGHT);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(0.0, 0.0, WALL_HEIGHT);
  
  glTexCoord2d(0.0, 0.0);
  glVertex3d(0.0, 1.0, 0.0);
  glTexCoord2d(1.0, 0.0);
  glVertex3d(1.0, 1.0, 0.0);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(1.0, 1.0, WALL_HEIGHT);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(0.0, 1.0, WALL_HEIGHT);
  
  glTexCoord2d(0.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0);
  glTexCoord2d(1.0, 0.0);
  glVertex3d(0.0, 1.0, 0.0);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(0.0, 1.0, WALL_HEIGHT);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(0.0, 0.0, WALL_HEIGHT);
  
  glTexCoord2d(0.0, 0.0);
  glVertex3d(1.0, 0.0, 0.0);
  glTexCoord2d(1.0, 0.0);
  glVertex3d(1.0, 1.0, 0.0);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(1.0, 1.0, WALL_HEIGHT);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(1.0, 0.0, WALL_HEIGHT);
  glEnd();
}

void Window::event(const SDL_Event &e) {
  switch (e.type) {
    case SDL_KEYDOWN:
      switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_W:
          _controls.walk_forward = true;
          break;
        case SDL_SCANCODE_S:
          _controls.walk_back = true;
          break;
        case SDL_SCANCODE_D:
          _controls.shift_right = true;
          break;
        case SDL_SCANCODE_A:
          _controls.shift_left = true;
          break;
        case SDL_SCANCODE_E:
          _controls.turn_right = true;
          break;
        case SDL_SCANCODE_Q:
          _controls.turn_left = true;
          break;
          case SDL_SCANCODE_R:
          _controls.tilt_up = true;
          break;
        case SDL_SCANCODE_F:
          _controls.tilt_down = true;
          break;
        default:;
      }
      break;
    case SDL_KEYUP:
      switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_W:
          _controls.walk_forward = false;
          break;
        case SDL_SCANCODE_S:
          _controls.walk_back = false;
          break;
        case SDL_SCANCODE_D:
          _controls.shift_right = false;
          break;
        case SDL_SCANCODE_A:
          _controls.shift_left = false;
          break;
        case SDL_SCANCODE_E:
          _controls.turn_right = false;
          break;
        case SDL_SCANCODE_Q:
          _controls.turn_left = false;
          break;
        case SDL_SCANCODE_R:
          _controls.tilt_up = false;
          break;
        case SDL_SCANCODE_F:
          _controls.tilt_down = false;
          break;
        default:;
        
      }
      break;
    case SDL_MOUSEMOTION:
  
      if (e.motion.xrel >= 0)
        _player.turn_right(e.motion.xrel * MOUSE_SENS);
      else
        _player.turn_left(-e.motion.xrel * MOUSE_SENS);
      if (e.motion.yrel >= 0)
        _player.tilt_down(e.motion.yrel * MOUSE_SENS);
      else
        _player.tilt_up(-e.motion.yrel * MOUSE_SENS);
      break;
    
    default:
      ;
  }
}

void Window::update() {
  if (_controls.walk_forward)
    _player.walk_forward(PLAYER_WALK_DIST);
  if (_controls.walk_back)
    _player.walk_back(PLAYER_WALK_DIST);
  if (_controls.shift_right)
    _player.shift_right(PLAYER_WALK_DIST);
  if (_controls.shift_left)
    _player.shift_left(PLAYER_WALK_DIST);
  if (_controls.turn_right)
    _player.turn_right(PLAYER_TURN_ANGLE);
  if (_controls.turn_left)
    _player.turn_left(PLAYER_TURN_ANGLE);
  if (_controls.tilt_up)
    _player.tilt_up(PLAYER_TURN_ANGLE);
  if (_controls.tilt_down)
    _player.tilt_down(PLAYER_TURN_ANGLE);
}

#pragma clang diagnostic pop