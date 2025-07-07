#include "graphics/texture_handler.hpp"
#include <ios>
#include <fstream>

/****************************************/
/*          Public Functions            */
/****************************************/


//initializes a specific textures for use
void TextureHandler::initTextures()
{
	
	//handles for texture buffers
	GLuint tex0, tex1, tex2, tex3; 
	
	//assign handles to actual buffers
	glGenTextures(1, &tex0);
	glGenTextures(1, &tex1);
	glGenTextures(1, &tex2);
	glGenTextures(1, &tex3);
  	
  	//set a bunch parameters
  	glActiveTexture(GL_TEXTURE0);
  	glBindTexture(GL_TEXTURE_2D, tex0); //
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  	glActiveTexture(GL_TEXTURE1); //TODO change back to 1
  	glBindTexture(GL_TEXTURE_2D, tex1);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  	glActiveTexture(GL_TEXTURE2);
  	glBindTexture(GL_TEXTURE_2D, tex2);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  	
  	glActiveTexture(GL_TEXTURE3);
  	glBindTexture(GL_TEXTURE_2D, tex3);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  	
  	//bind textures to images
  	loadPpm(tex0, "./src/graphics/smiley.ppm");
  	loadPpm(tex1, "./src/graphics/reachup.ppm");
  	loadPpm(tex2, "./src/graphics/secret.ppm");
  	loadPpm(tex3, "./src/graphics/grass.ppm");
  	
  	//put each handle in array for later access
  	textures[0] = tex0;
  	textures[1] = tex1;
  	textures[2] = tex2;
  	textures[3] = tex3;
  	
  	
}

//gets the texture at index
GLuint TextureHandler::getTexture(int index)
{
	return textures[index];
}


/****************************************/
/*          Private Functions           */
/****************************************/

/*PPM FILES*/

//creates a texture from a .ppm image file
void TextureHandler::loadPpm(GLuint texHandle, const char *ppmFilename)
{
	int texWidth, texHeight;
  	unsigned char* image; 
  	
  	image = ppmRead(ppmFilename, &texWidth, &texHeight);

  	glActiveTexture(GL_TEXTURE0);
  	glBindTexture(GL_TEXTURE_2D, texHandle);
  	
  	//this is where the image becomes a texture
  	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}


/* 
 * ppmRead: read a PPM raw (type P6) file.  The PPM file has a header
 * that should look something like:
 *
 *   P6
 *   # comment
 *   width height max_value
 *   rgbrgbrgb...
 *
 * where "P6" is the magic cookie which identifies the file type and
 * should be the only characters on the first line followed by a
 * carriage return.  Any line starting with a # mark will be treated
 * as a comment and discarded.  After the magic cookie, three integer
 * values are expected: width, height of the image and the maximum
 * value for a pixel (max_value must be < 256 for PPM raw files).  The
 * data section consists of width*height rgb triplets (one byte each)
 * in binary format (i.e., such as that written with fwrite() or
 * equivalent).
 *
 * The rgb data is returned as an array of unsigned chars (packed
 * rgb).  The malloc()'d memory should be free()'d by the caller.  If
 * an error occurs, an error message is sent to stderr and NULL is
 * returned.
 *
 */

//reads ppm file and returns an image
unsigned char* TextureHandler::ppmRead(const char* filename, int* width, int* height ) {

   FILE* fp;
   int i, w, h, d;
   unsigned char* image;
   char head[70];		// max line <= 70 in PPM (per spec).
   
   fp = fopen( filename, "rb" );
   if ( !fp ) {
      perror(filename);
      return NULL;
   }

   // Grab first two chars of the file and make sure that it has the
   // correct magic cookie for a raw PPM file.
   char* check = (char *)malloc(70 * sizeof(char));
   check = fgets(head, 70, fp);
   
   //this is just so it compiles
   char* p6 = (char *)malloc(70 * sizeof(char));
   if(check == p6)
   {
   		printf("jusstt worrkk\n");
   }
   //
   if (strncmp(head, "P6", 2)) {
      fprintf(stderr, "%s: Not a raw PPM file\n", filename);
      return NULL;
   }

   // Grab the three elements in the header (width, height, maxval).
   i = 0;
   while( i < 3 ) {
      check = fgets( head, 70, fp );
      if ( head[0] == '#' )		// skip comments.
         continue;
      if ( i == 0 )
         i += sscanf( head, "%d %d %d", &w, &h, &d );
      else if ( i == 1 )
         i += sscanf( head, "%d %d", &h, &d );
      else if ( i == 2 )
         i += sscanf( head, "%d", &d );
   }

   // Grab all the image data in one fell swoop.
   image = (unsigned char*) malloc( sizeof( unsigned char ) * w * h * 3 );
   int check2 = 0;
   check2 = fread( image, sizeof( unsigned char ), w * h * 3, fp );
   if(check2 == 0)
   {
   		printf("didnt work\n");
   }
   fclose( fp );

   *width = w;
   *height = h;
   return image;

}

/*BMP FILES*/

//creates a texture from bmp file
void TextureHandler::loadBmp(GLuint texHandle, const char *bmpFilename)
{
	int texWidth, texHeight;
  	unsigned char* image; 
  	
  	image = bmpRead(bmpFilename, &texWidth, &texHeight);

  	glActiveTexture(GL_TEXTURE0);
  	glBindTexture(GL_TEXTURE_2D, texHandle);
  	
  	//this is where the image becomes a texture
  	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}


//reads a bmp file and returns an image to be used for texturing
unsigned char* TextureHandler::bmpRead(const char* FilePath, int* width, int* height)
{
	int h, w;
	short BitsPerPixel = 0;
    std::vector<unsigned char> Pixels;
    
	std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
    if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");

    hFile.seekg(0, std::ios::end);
    int Length = hFile.tellg();
    hFile.seekg(0, std::ios::beg);
    std::vector<std::uint8_t> FileInfo(Length);
    hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);

    if(FileInfo[0] != 'B' && FileInfo[1] != 'M')
    {
        hFile.close();
        throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
    }

    if (FileInfo[28] != 24 && FileInfo[28] != 32)
    {
        hFile.close();
        throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
    }

    BitsPerPixel = FileInfo[28];
    w = FileInfo[18] + (FileInfo[19] << 8);
    h = FileInfo[22] + (FileInfo[23] << 8);
    std::uint32_t PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
    std::uint32_t size = ((w * BitsPerPixel + 31) / 32) * 4 * h;
    Pixels.resize(size);

    hFile.seekg (PixelsOffset, std::ios::beg);
    hFile.read(reinterpret_cast<char*>(Pixels.data()), size);
    hFile.close();
    
    *width = w;
    *height = h;
    return Pixels.data();
}
















