
#include "Player.h"
#include <cmath>

constexpr double Pi = acos(-1.0);

void Player::spawn(std::shared_ptr<Map> &m) {
  _map  = m;
  _x    = _map->start_x();
  _y    = _map->start_y();
  _dir  = _map->start_dir();
  _tilt = Pi/2;
}

void Player::walk(double angle, double dist) {
  
  if (_map == nullptr)
    return;
  
  double dx = dist*cos(angle);
  double dy = dist*sin(angle);
  double nx = _x + dx;
  double ny = _y + dy;
  
  if (nx >= _map->width())
    nx -= _map->width();
  if (nx < 0)
    nx += _map->width();
  
  if (ny >= _map->height())
    ny -= _map->height();
  if (ny < 0)
    ny += _map->height();
  
  if (not _map->is_wall(nx, ny)) {
    _x = nx;
    _y = ny;
  }
}

void Player::walk_forward(double dist) {
  walk(_dir, dist);
}
void Player::walk_back(double dist) {
  walk(_dir, -dist);
}
void Player::shift_left(double dist) {
  walk(_dir - Pi/2, dist);
}
void Player::shift_right(double dist) {
  walk(_dir + Pi/2, dist);
}
void Player::turn_left(double angle) {
  
  _dir -= angle;
  if (_dir < 0)
    _dir += Pi*2.0;
}
void Player::turn_right(double angle) {
  
  _dir += angle;
  if (_dir >= Pi*2.0)
    _dir -= Pi*2.0;
}
void Player::tilt_down(double angle) {
  _tilt += angle;
  if (_tilt >= Pi*0.95)
    _tilt = Pi*0.95;
}
void Player::tilt_up(double angle) {
  _tilt -= angle;
  if (_tilt < Pi*0.05)
    _tilt = Pi*0.05;
}
