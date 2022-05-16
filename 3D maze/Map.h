#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <vector>
#include <cmath>

class Map {
private:
  int                      _width{}, _height{};
  double                   _start_x{}, _start_y{}, _start_dir{};
  std::vector<std::string> _data;
  
  void next_line(std::ifstream &file,
                 std::string &line);

public:
  Map(const char *filename);
  
  int width() const { return _width; }
  int height() const { return _height; }
  double start_x() const { return _start_x; }
  double start_y() const { return _start_y; }
  double start_dir() const { return _start_dir; }
  
  bool is_wall(int x, int y) {
    if (x < 0 or x >= _width or y < 0 or y >= _height)
      return true;
    return _data[y][x] != ' ';
  }
  bool is_wall(double x, double y) {
    return is_wall(int(floor(x)), int(floor(y)));
  }
  char get(int x, int y) {
    if (x < 0 or x >= _width or y < 0 or y >= _height)
      return '#';
    return _data[y][x];
  }
  char get(double x, double y) {
    return get(int(floor(x)), int(floor(y)));
  }
};

#endif /*MAP_H_*/