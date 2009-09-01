/*
  
  Modified by: Daniel Gomez Rico
  danielgomezrico@gmail.com   
  Student of system engineering, 5 semester
  at the EAFIT university from Colombia
  30 - May - 2009
  
  Original file:
  File for "Textures" lesson of the OpenGL tutorial on
  www.videotutorialsrock.com
  
*/

#include <assert.h>
#include <fstream>

#include "Imageloader.h"

using namespace std;

Image::Image(){}

Image::Image(char* ps, int w, int h) : pixels(ps), width(w), height(h) {
  
}

Image::~Image() {
  delete[] pixels;
}

void Image::operator=(Image *newImage){
  pixels = newImage->getPixels();
  width = newImage->getWidth();
  height = newImage->getHeight();
}

namespace {
  //Converts a four-character array to an integer, using little-endian form
  int toInt(const char* bytes) {
    return (int)(((unsigned char)bytes[3] << 24) |
		 ((unsigned char)bytes[2] << 16) |
		 ((unsigned char)bytes[1] << 8) |
		 (unsigned char)bytes[0]);
  }
  
  //Converts a two-character array to a short, using little-endian form
  short toShort(const char* bytes) {
    return (short)(((unsigned char)bytes[1] << 8) |
		   (unsigned char)bytes[0]);
  }
  
  //Reads the next four bytes as an integer, using little-endian form
  int readInt(ifstream &input) {
    char buffer[4];
    input.read(buffer, 4);
    return toInt(buffer);
  }
  
  //Reads the next two bytes as a short, using little-endian form
  short readShort(ifstream &input) {
    char buffer[2];
    input.read(buffer, 2);
    return toShort(buffer);
  }
  
  //Just like auto_ptr, but for arrays
  template<class T>
  class auto_array {
  private:
    T* array;
    mutable bool isReleased;
  public:
    explicit auto_array(T* array_ = NULL) :
      array(array_), isReleased(false) {
    }
    
    auto_array(const auto_array<T> &aarray) {
      array = aarray.array;
      isReleased = aarray.isReleased;
      aarray.isReleased = true;
    }
    
    ~auto_array() {
      if (!isReleased && array != NULL) {
	delete[] array;
      }
    }
    
    T* get() const {
      return array;
    }
    
    T &operator*() const {
      return *array;
    }
    
    void operator=(const auto_array<T> &aarray) {
      if (!isReleased && array != NULL) {
	delete[] array;
      }
      array = aarray.array;
      isReleased = aarray.isReleased;
      aarray.isReleased = true;
    }
    
    T* operator->() const {
      return array;
    }
    
    T* release() {
      isReleased = true;
      return array;
    }
    
    void reset(T* array_ = NULL) {
      if (!isReleased && array != NULL) {
	delete[] array;
      }
      array = array_;
    }
    
    T* operator+(int i) {
      return array + i;
    }
    
    T &operator[](int i) {
      return array[i];
			}
  };
}



Image* loadBMP(const char* filename) {
  ifstream input;
  input.open(filename, ifstream::binary);
  assert(!input.fail() || !"Could not find file");
  char buffer[2];
  input.read(buffer, 2);
  assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
  input.ignore(8);
  int dataOffset = readInt(input);
  
  //Read the header
  int headerSize = readInt(input);
  int width;
  int height;
  switch(headerSize) {
  case 40:
    //V3
    width = readInt(input);
    height = readInt(input);
    input.ignore(2);
    assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
    assert(readShort(input) == 0 || !"Image is compressed");
    break;
  case 12:
    //OS/2 V1
    width = readShort(input);
    height = readShort(input);
    input.ignore(2);
    assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
    break;
  case 64:
    //OS/2 V2
    assert(!"Can't load OS/2 V2 bitmaps");
    break;
  case 108:
    //Windows V4
    assert(!"Can't load Windows V4 bitmaps");
    break;
  case 124:
    //Windows V5
    assert(!"Can't load Windows V5 bitmaps");
    break;
  default:
    assert(!"Unknown bitmap format");
  }
  
  //Read the data
  int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
  int size = bytesPerRow * height;
  auto_array<char> pixels(new char[size]);
  input.seekg(dataOffset, ios_base::beg);
  input.read(pixels.get(), size);
  
  //Get the data into the right format
  auto_array<char> pixels2(new char[width * height * 3]);
  for(int y = 0; y < height; y++) {
    for(int x = 0; x < width; x++) {
      for(int c = 0; c < 3; c++) {
	pixels2[3 * (width * y + x) + c] =
	  pixels[bytesPerRow * y + 3 * x + (2 - c)];
      }
    }
  }
  
  input.close();
  return new Image(pixels2.release(), width, height);
}

char* Image::getPixels(){ return pixels;}
int Image::getWidth(){ return width;}
int Image::getHeight(){ return height;}

Image* loadRAW(int width ,int height, const char * filename  ){
  // open texture data
  char * data;
  FILE * file = fopen( filename, "rb" );
  if ( file == NULL ) {
    printf("File not found'n");
    return NULL;
  }
  
  data = (char*)malloc( width * height * 3 );
  // read texture data
  fread( data, width * height * 3, 1, file );
  fclose( file );   
  return new Image(data, width, height);  
}


void loadTextures(Image *images, GLuint *texturesId, int numText) {
  glGenTextures(numText, texturesId); //Make room for our texture
  
  for(int i = 0; i < numText; i++){    
    glBindTexture(GL_TEXTURE_2D, texturesId[i]); //Tell OpenGL which texture to edit
    //Map the image to the texture
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    // when texture area is large, bilinear filter the original
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // the texture wraps over at the edges (repeat)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, images[i].getWidth(), images[i].getHeight(),
		      GL_RGB, GL_UNSIGNED_BYTE, images[i].getPixels());
  }
  
  // return textureId; //Returns the id of the texture
}




