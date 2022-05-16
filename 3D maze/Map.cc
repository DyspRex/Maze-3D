#include "Map.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

void Map::next_line(std::ifstream &file, std::string &line) {
  
  while (std::getline(file, line)) {
    if (line.length() > 0 && line[0] != ';')
      return;
  }
  throw std::runtime_error(
          "В файле карты недостаточно строк.");
}

Map::Map(const char *filename) {
  std::ifstream file{filename};
  if (not file.good())
    throw std::runtime_error(
            std::string("Не могу открыть файл карты ") +
            std::string(filename));
  
  std::string line;
  next_line(file, line);
  std::stringstream(line)
          >> _width >> _height
          >> _start_x >> _start_y
          >> _start_dir;
  _data.resize(_height);
  for (auto &&s: _data)
    next_line(file, s);
}