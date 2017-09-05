/*
 * Copyright (c) 2005-2015 Michael Shafae.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: GLSLShader.h 5720 2015-04-15 22:08:59Z mshafae $
 *
 * Utility functions for loading a shader.
 *
 * STUDENTS DO NOT NEED TO MAKE ANY CHANGES TO THIS FILE.
 *
 */

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>

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

#ifndef NOTEXTURE
#include "GLTexture.h"
#else
#define Texture2D void*
#endif

#ifndef _GLSLSHADER_H_
#define _GLSLSHADER_H_

#define msglError( ) _msglError( stderr, __FILE__, __LINE__ )

bool _msglError( FILE *out, const char *filename, int line ){
  bool ret = false;
  std::string errorString;
  GLenum err = glGetError( );
  while( err != GL_NO_ERROR ) {
    ret = true;
    switch(err){
    case GL_INVALID_ENUM:
      errorString = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      errorString = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      errorString = "GL_INVALID_OPERATION";
      break;
    case GL_STACK_OVERFLOW:
      errorString = "GL_STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      errorString = "GL_STACK_UNDERFLOW";
      break;
    case GL_TABLE_TOO_LARGE:
      errorString = "GL_TABLE_TOO_LARGE";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      // OpenGL 3.0 and later
      errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      errorString = "GL_OUT_OF_MEMORY";
      break;
    }
    fprintf(stderr, "%s:%d:GL ERROR(%d): %s\n", filename, line, err, errorString.c_str( ));
    err = glGetError( );
  }
  return( ret );
}

char* file2strings( const char *filename ){
  long fileLength, numberOfCharsRead;
  char *strings;
  FILE *fhandle = fopen( filename, "r" );
  if( !fhandle ){
    fprintf( stderr, "%s: Can't open file %s for reading\n", __FUNCTION__, filename );
    return NULL;
  }
  fseek( fhandle, 0, SEEK_END );
  fileLength = ftell( fhandle );

  fseek( fhandle, 0, SEEK_SET );

  if( !(strings = (char*)calloc( (fileLength+1), sizeof(char) )) ){
    fprintf( stderr, "%s: Can't malloc space for string buffer %s\n", __FUNCTION__, filename );
    return NULL;
  }

  numberOfCharsRead = (long)fread( strings, sizeof(char), fileLength, fhandle );

  strings[numberOfCharsRead] = '\0';

  fclose( fhandle );

  return( strings );
}

class Shader{
public:
  GLuint _object;
  char *_srcFileName;
  
  Shader( const char *srcFileName ){
    _srcFileName = strdup(srcFileName);
  };

  ~Shader( ){
    glDeleteShader( _object );
    free(_srcFileName);
    msglError( );
  }

  bool compileShader( const GLchar *src ){
    GLint compiled_ok;
    char *msg;
    GLint length = (GLint)strlen(src);
    glShaderSource( _object, 1, &src, &length );
    glCompileShader( _object );
    msglError( );
    glGetShaderiv( _object, GL_COMPILE_STATUS, &compiled_ok );
    msglError( );
    if( !compiled_ok ){
      fprintf( stderr, "Compilation failed for shader %s\n",
        _srcFileName );
      msg = getInfoLog( );
      fprintf( stderr, "Info Log:\n%s\n", msg );
      free( msg );
    }
    return( compiled_ok == 1 );
  }
  
  char* getInfoLog( ){
    GLint info_log_length;
    glGetShaderiv( _object, GL_INFO_LOG_LENGTH, &info_log_length );
    char *info_log = (char*)malloc( info_log_length );
    glGetShaderInfoLog( _object, info_log_length, NULL, info_log);
    return( info_log );
  }
};

class VertexShader : public Shader{

public:
  VertexShader( const char *srcFileName ) : Shader(srcFileName){
    char *src = file2strings( _srcFileName );
    assert(src);
    if( (Shader::_object = glCreateShader( GL_VERTEX_SHADER )) == 0 ){
      fprintf( stderr, "Can't generate vertex shader name\n" );
    }
    msglError( );
    compileShader( src );
    msglError( );
    free( src );
  }

  GLuint object( ){
    return Shader::_object;
  }

};


class FragmentShader : public Shader{

public:
  FragmentShader( const char *srcFileName ) : Shader(srcFileName){
    char *src = file2strings( _srcFileName );
    assert(src);
    if( (Shader::_object = glCreateShader( GL_FRAGMENT_SHADER )) == 0 ){
      fprintf( stderr, "Can't generate fragment shader name\n" );
      exit(1);
    }
      compileShader( src );
      free( src );
    }

    GLuint object( ){
      return Shader::_object;
    }
};


class GLSLProgram{

private:
  GLuint _object;
  Texture2D *_texture;

public: 
  GLSLProgram( ){
    if( (_object = glCreateProgram( )) == 0 ){
      fprintf( stderr, "Can't generate program name\n" );
      exit(1);
    }
    _texture = NULL;
  }

  ~GLSLProgram( ){
    detachAll( );
    glDeleteProgram( _object );
  }

  GLuint id( ){
    return _object;
  }
  
  bool attach( FragmentShader &fs ){
    glAttachShader( _object, fs.object( ) );
    return( !msglError( ) );
  }

  bool attach( VertexShader &vs ){
    glAttachShader( _object, vs.object( ) );
    return( !msglError( ) );
  }

  bool detachAll( ){
    bool ret = false;
    GLsizei const maxCount = 32;
    GLsizei count;
    GLuint shaders[32];
    glGetAttachedShaders( _object, maxCount, &count, shaders );
    for(int i = 0; i < count; i++ ){
      if( !(detach( shaders[i] )) ){
        ret = true;
      }
    }
    return( ret );
  }

  bool detach( GLuint shaderName ){
    msglError( );
    glDetachShader( _object, shaderName );
    return( !msglError( ) );
  }

  bool detach( FragmentShader &fs ){
    glDetachShader( _object, fs.object( ) );
    return( !msglError( ) );
  }

  bool detach( VertexShader &vs ){
    glDetachShader( _object, vs.object( ) );
    return( !msglError( ) );
  }

  bool link( ){
    GLint linked_ok;
    char *msg;
    bool ret = true;
  
    glLinkProgram( _object );

    glGetProgramiv( _object, GL_LINK_STATUS, &linked_ok );
    if( !linked_ok ){
      ret = false;
      fprintf( stderr, "Linking failed.\n" );
      msg = getInfoLog( );
      fprintf( stderr, "%s\n", msg );
      free(msg );
    }
    return ret;
  }
  
  char* getInfoLog( ){
    GLint info_log_length;
    glGetProgramiv( _object, GL_INFO_LOG_LENGTH, &info_log_length );
    char *info_log = (char*)malloc( info_log_length );
    glGetProgramInfoLog( _object, info_log_length, NULL, info_log);
    return( info_log );
  }
  
  virtual bool activateUniforms( ){
    return false;
  }
  
  bool activate( ){
    activateUniforms( );
    msglError( );
    glUseProgram( _object );
#ifndef NOTEXTURE
    if(_texture){
      _texture->bind( );
    }
#endif
    return( !msglError( ) );
  }
  
  bool deactivate( ){
    glUseProgram( 0 );
    return( !msglError( ) );
  }
  
  void attachTexture(Texture2D* texture){
      _texture = texture;
  }

  Texture2D* texture( ){
    return _texture;
  }
  
  bool isActive( ){
    int id;
    // slow!
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);
    return( id == _object );
  }
  
  bool isHardwareAccelerated( ){
    bool ret = true;
     #if defined(__APPLE__)
     /*
      * This checks to make sure that the shaders are going to
      * be hardware accelerated.
      * If they aren't, a message is printed.
      *
      * Something similar can be done on Unix as well as MS Windows but
      * it is left up to the student to look up the neccessary API calls.
      */
    GLint hardwareAccelerated;

    CGLGetParameter( CGLGetCurrentContext( ), kCGLCPGPUVertexProcessing, &hardwareAccelerated );
    if( !hardwareAccelerated ){
      ret = false;
      fprintf( stderr,
        "Warning: Vertex shader is NOT being hardware-accelerated\n" );
    }
    CGLGetParameter( CGLGetCurrentContext( ), kCGLCPGPUFragmentProcessing, &hardwareAccelerated );
    if( !hardwareAccelerated ){
      ret = false;
      fprintf( stderr,
        "Warning: Fragment shader is NOT being hardware-accelerated\n" );
    }
     #endif
    return ret;
  }
};

bool loadShaderProgram(GLSLProgram& shaderProgram, const char* vertexShaderSource, const char* fragmentShaderSource){
  bool rv = true;
  FragmentShader fragmentShader(fragmentShaderSource);
  VertexShader vertexShader(vertexShaderSource);
  shaderProgram.attach(vertexShader);
  shaderProgram.attach(fragmentShader);
  shaderProgram.link( );
  shaderProgram.activate( );
  
  fprintf(stderr, "Shader program built from %s and %s.\n",
         vertexShaderSource, fragmentShaderSource);

  if( shaderProgram.isActive( ) ){
    fprintf(stderr, "Shader program is loaded and active with id %d.\n", shaderProgram.id( ) );
  }else{
    fprintf(stderr, "Shader program did not load and activate correctly. Exiting.");
    rv = false;
  }
  return rv;
}

#endif
