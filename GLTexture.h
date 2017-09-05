/*
 * Copyright (c) 2005-2015 Michael Shafae.
 * All rights reserved.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * $Id: GLTexture.h 5640 2015-04-01 16:26:14Z mshafae $
 *
 * Utility functions for loading a texture.
 *
 * STUDENTS _MAY_ NEED TO MAKE CHANGES TO ACCOMMODATE IMAGE TYPES USED.
 *
 */

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
/* Apple's weird location of their OpenGL & GLUT implementation */
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <FreeImage.h>

#ifndef _GLTEXTURE_H_
#define _GLTEXTURE_H_

// An almost-all-purpose image loader using FreeImage
FIBITMAP* FreeImageGenericLoader(const char* filename, int flag){
	FIBITMAP* dib = NULL;
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename, 0);
	if( fif == FIF_UNKNOWN ){
		fif = FreeImage_GetFIFFromFilename(filename);
	}
	if( (fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)){
	  dib = FreeImage_Load(fif, filename, flag);
		assert(dib);	
	}
	return dib;
}

class Texture2D{
private:
	char *_filename;
	GLuint _id;
	GLint _minFilter;
	GLint _magFilter;

public:
	Texture2D(char *filename, GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST){

		_filename = filename;
    _minFilter = minFilter;
    _magFilter = magFilter;
    
		FIBITMAP *bitmap = FreeImageGenericLoader(_filename, 0);
		assert(bitmap);
		FREE_IMAGE_COLOR_TYPE fic = FreeImage_GetColorType(bitmap);
		assert( fic == FIC_RGB || fic == FIC_RGBALPHA );
		unsigned int bpp = FreeImage_GetBPP(bitmap);
		assert(bpp == 24 || bpp == 32);
		GLenum pixelFormat;
		if(bpp == 24){
			pixelFormat = GL_RGB;
		}else{
			pixelFormat = GL_RGBA;
		}
	  int width = FreeImage_GetWidth(bitmap);
	  int height = FreeImage_GetHeight(bitmap);
		
    glGenTextures(1, &_id);
		bind( );
		
		// glTexEnv is deprecated in Core profile
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _magFilter);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap));

    // Sanity check
    /*
    fprintf(stderr, "Width: %d, Height: %d\n", width, height);
    if( pixelFormat == GL_RGB ){
      fprintf(stderr, "Pixel format is RGB\n");
    }else{
      fprintf(stderr, "Pixel format is RGBA\n");
    }
    GLubyte *p = (GLubyte*)FreeImage_GetBits(bitmap);
    for(int i = 0; i < (width * height); i++){
      printf("%d ", p[i]);
    }
    */
	}
	~Texture2D( ){
		glDeleteTextures(1, &_id);
	}

	void bind( ){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _magFilter);
    glBindTexture(GL_TEXTURE_2D, _id);
	}
	
	GLuint id( ){
		return _id;
	}
	GLint minFilter( ){
		return _minFilter;
	}
	GLint magFilter( ){
		return _magFilter;
	}
};

#endif
