#include "readtexture.h"
#include <string.h>

int ispowerof2(int n){
  if ( n == 0 ) return 0;
  while( n%2 == 0 ){
    n >>= 1;
  }
  return (n == 1)?1:0;
}

char* get_word(FILE* texturefile){
  static char word[80];
  static char readbuf[80];
  static char* ptr = readbuf;
  static char* ptr2 = word;
  while(1){
    if(*ptr == '\0'){
      fgets(readbuf, 80, texturefile);
      while(*readbuf == '#'){
	fgets(readbuf, 80, texturefile);
      }
      ptr = readbuf;
      continue;
    }else if(isspace(*ptr)){
      ptr++;
      continue;
    }else{
      break;
    }
  }
  ptr2 = word;
  while(!isspace(*ptr)){
    *ptr2++ = *ptr++;
  }
  *ptr2 = '\0';
  return word;
}

int SetupTexture(char* filename, GLuint id){
  int i, j, isize, jsize;
  GLubyte* data;
  FILE* texturefile;

  if( NULL == (texturefile = fopen(filename, "r"))){
    fprintf(stderr, "Cannot open texure file %s\n", filename);
    return 0;
  }

  if(strcmp("P3", get_word(texturefile)) != 0 ){
    fprintf(stderr, "Texure file %s is not an ASCII PPM format\n", filename);
    return 0;
  }

  jsize = atoi(get_word(texturefile));
  if ( !ispowerof2(jsize) ){
    fprintf(stderr, "Height of texture file %s is not power of 2\n", filename);
    return 0;
  }

  isize = atoi(get_word(texturefile));
  if ( !ispowerof2(isize) ){
    fprintf(stderr, "Width of texture file %s is not power of 2\n", filename);
    return 0;
  }

  if(255 < atoi(get_word(texturefile))){
    fprintf(stderr, "Color component in %s does not fit in a byte.\n", 
	    filename);
    return 0;
  }
      
  if(NULL == (data = (GLubyte*)malloc(3*isize*jsize*sizeof(GLubyte)))){
    fprintf(stderr, "Cannot allocate memory for texture data\n");
    return 0;
  }

  for (i = 0; i < isize; i++) {
    for (j = 0; j < jsize; j++) {
      *(data + 3*(jsize*i + j)) = (GLubyte) atoi(get_word(texturefile));
      *(data + 3*(jsize*i + j)+1) = (GLubyte) atoi(get_word(texturefile));
      *(data + 3*(jsize*i + j)+2) = (GLubyte) atoi(get_word(texturefile));
    }
  }
  fclose(texturefile);
  
  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, jsize, isize, 
	       0, GL_RGB, GL_UNSIGNED_BYTE, data);
  return 1;
}

int SetupTexturewithLOD(char* filename, GLuint id, GLint lod){
  int i, j, isize, jsize;
  GLubyte* data;
  FILE* texturefile;

  if( NULL == (texturefile = fopen(filename, "r"))){
    fprintf(stderr, "Cannot open texure file %s\n", filename);
    return 0;
  }

  if(strcmp("P3", get_word(texturefile)) != 0 ){
    fprintf(stderr, "Texure file %s is not an ASCII PPM format\n", filename);
    return 0;
  }

  jsize = atoi(get_word(texturefile));
  if ( !ispowerof2(jsize) ){
    fprintf(stderr, "Height of texture file %s is not power of 2\n", filename);
    return 0;
  }

  isize = atoi(get_word(texturefile));
  if ( !ispowerof2(isize) ){
    fprintf(stderr, "Width of texture file %s is not power of 2\n", filename);
    return 0;
  }

  if(255 < atoi(get_word(texturefile))){
    fprintf(stderr, "Color component in %s does not fit in a byte.\n", 
	    filename);
    return 0;
  }
      
  if(NULL == (data = (GLubyte*)malloc(3*isize*jsize*sizeof(GLubyte)))){
    fprintf(stderr, "Cannot allocate memory for texture data\n");
    return 0;
  }

  for (i = 0; i < isize; i++) {
    for (j = 0; j < jsize; j++) {
      *(data + 3*(jsize*i + j)) = (GLubyte) atoi(get_word(texturefile));
      *(data + 3*(jsize*i + j)+1) = (GLubyte) atoi(get_word(texturefile));
      *(data + 3*(jsize*i + j)+2) = (GLubyte) atoi(get_word(texturefile));
    }
  }
  fclose(texturefile);
  
  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, lod, GL_RGB, jsize, isize, 
	       0, GL_RGB, GL_UNSIGNED_BYTE, data);
  return 1;
}

int SetupTexturewithMipmap(char* filename, GLuint id){
  int i, j, isize, jsize;
  GLubyte* data;
  FILE* texturefile;

  if( NULL == (texturefile = fopen(filename, "r"))){
    fprintf(stderr, "Cannot open texure file %s\n", filename);
    return 0;
  }

  if(strcmp("P3", get_word(texturefile)) != 0 ){
    fprintf(stderr, "Texure file %s is not an ASCII PPM format\n", filename);
    return 0;
  }

  jsize = atoi(get_word(texturefile));
  if ( !ispowerof2(jsize) ){
    fprintf(stderr, "Height of texture file %s is not power of 2\n", filename);
    return 0;
  }

  isize = atoi(get_word(texturefile));
  if ( !ispowerof2(isize) ){
    fprintf(stderr, "Width of texture file %s is not power of 2\n", filename);
    return 0;
  }

  if(255 < atoi(get_word(texturefile))){
    fprintf(stderr, "Color component in %s does not fit in a byte.\n", 
	    filename);
    return 0;
  }
      
  if(NULL == (data = (GLubyte*)malloc(3*isize*jsize*sizeof(GLubyte)))){
    fprintf(stderr, "Cannot allocate memory for texture data\n");
    return 0;
  }

  for (i = 0; i < isize; i++) {
    for (j = 0; j < jsize; j++) {
      *(data + 3*(jsize*i + j)) = (GLubyte) atoi(get_word(texturefile));
      *(data + 3*(jsize*i + j)+1) = (GLubyte) atoi(get_word(texturefile));
      *(data + 3*(jsize*i + j)+2) = (GLubyte) atoi(get_word(texturefile));
    }
  }
  fclose(texturefile);
  
  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if (0 == gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, jsize, isize, 
	       GL_RGB, GL_UNSIGNED_BYTE, data)){
    return 1;
  }else{
    return 0;
  }
}

