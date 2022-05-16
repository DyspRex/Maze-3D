
#ifndef DYSPREX_TEXTURE_H
#define DYSPREX_TEXTURE_H

#include <GL/gl.h>
#include <GL/glu.h>

class Texture final {
private:
  GLuint _tex{0};
public:
  Texture(const char *filename);
  ~Texture();
  void bind() { glBindTexture(GL_TEXTURE_2D, _tex); }
};


#endif //DYSPREX_TEXTURE_H
