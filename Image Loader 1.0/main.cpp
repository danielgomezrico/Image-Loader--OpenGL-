#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>

using namespace std;

void init (void);
void initTextures();

//arrays to manage the textures
GLuint arrayTexturesId[2];
string arrayNameTextures[2] ={"Textures/1.bmp","Textures/vtr.bmp"}; 
Image arrayImages[2];

void init (void)
{           
  initTextures();
}

void initTextures(){
  int numTex = sizeof(arrayNameTextures)/sizeof(string);
  
  for(int i = 0; i < numTex; i++){
    Image *imageTemp = loadBMP(arrayNameTextures[i].c_str()); 
    arrayImages[i] = imageTemp;
  }
  loadTextures(arrayImages, (GLuint*)arrayTexturesId, numTex );
}


int main(int argc, char** argv)
{

  return 0;
}



