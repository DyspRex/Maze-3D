
#include "Texture.h"
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <stdexcept>
#include "Texture.h"


Texture::Texture(const char *filename) {
  auto image1 = std::shared_ptr<SDL_Surface>(
          IMG_Load(filename), SDL_FreeSurface);
  if (image1 == nullptr)
    throw std::runtime_error(
            std::string("Can't load a texture: ") +
            std::string(SDL_GetError()));
  auto image2 = std::shared_ptr<SDL_Surface>(
          SDL_CreateRGBSurface(
                  0, image1->w, image1->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000),
          SDL_FreeSurface);
  SDL_UpperBlit(image1.get(), nullptr, image2.get(), nullptr);
  
  glGenTextures(1, &_tex);
  glBindTexture(GL_TEXTURE_2D, _tex);
  gluBuild2DMipmaps(
          GL_TEXTURE_2D, GL_RGBA, image2->w, image2->h, GL_RGBA, GL_UNSIGNED_BYTE, image2->pixels);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}
Texture::~Texture(){
  glDeleteTextures(1, &_tex);
}