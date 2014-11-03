#ifndef _READTEXTURE_H_
#define _READTEXTURE_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

char* get_word(FILE* texturefile);
int SetupTexture(char* filename, GLuint id);
int SetupTexturewithLOD(char* filename, GLuint id, GLint LOD);
int SetupTexturewithMipmap(char* filename, GLuint id);

#endif /* _READTEXTURE_H_ */ 