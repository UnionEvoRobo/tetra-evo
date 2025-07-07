#ifndef GRAPHICS_TEXTURE_HANDLER_HPP
#define GRAPHICS_TEXTURE_HANDLER_HPP


#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <memory>
#include <stdexcept>

#include <GL/glut.h>

using namespace std;      // for string, vector, iostream and other standard C++ stuff
//using namespace std::tr1; // for shared_ptr


class TextureHandler{
    
    public:
        void initTextures();
        GLuint getTexture(int index); //use const global
        
    	
    private:
    	GLuint textures[10];
    	void loadPpm(GLuint texHandle, const char *ppmFilename);
    	void loadBmp(GLuint texHandle, const char *bmpFilename);
    	unsigned char* ppmRead(const char* filename, int* width, int* height);
    	unsigned char* bmpRead(const char* FilePath, int* width, int* height);
    	static const int numTextures = 4;
    	
        
        
        
};


#endif
