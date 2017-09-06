/*
 * Copyright (c) 2015 Michael Shafae
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
 *
 * $Id: GLFWApp.h 6554 2017-09-05 06:58:22Z mshafae $
 *
 * STUDENTS DO NOT NEED TO MAKE ANY CHANGES TO THIS FILE.
 *
 */

//#pragma clang diagnostic ignored "-Wunused-private-field"

#include <array>
#include <tuple>

#ifndef _MSGFX_GLFW3_APP_
#define _MSGFX_GLFW3_APP_

#ifdef WIN32
#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define WIN32_LEAN_AND_MEAN 1
#define _USE_MATH_DEFINES 1
#include <Windows.h>
#pragma warning(disable : 4305)
#endif

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>

#include <iostream>
#include <string>

#include <FreeImage.h>

#include <GL/glew.h>

//#define GLFW_INCLUDE_GLU
//#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

class GLFWApp{
 public:

  typedef enum{
    CORE = GLFW_OPENGL_CORE_PROFILE,
    COMPATIBILITY = GLFW_OPENGL_COMPAT_PROFILE
  }profile_t;

  typedef enum{
    VSYNC,
    ASYNC,
    TEARING
  }syncmode_t;

  typedef enum{
    MOUSE_BUTTON_NONE = 0,
    MOUSE_BUTTON_LEFT = 1,
    MOUSE_BUTTON_RIGHT = 2,
    MOUSE_BUTTON_MIDDLE = 4
  }mouseButton_t;

  typedef enum{
    KEY_PRESS = GLFW_PRESS,
    KEY_RELEASE = GLFW_RELEASE,
    KEY_REPEAT = GLFW_REPEAT
  }keyaction_t;

  GLFWApp(int argc, char* argv[], const char* windowTitle,
          int windowSize_X, int windowSize_Y,
          int major = 2, int minor = 1,
          std::tuple<int, int> const & position = std::make_tuple(100, 100)) :
  _window(nullptr),
    _windowTitle(windowTitle),
    _major(major),
    _minor(minor),
    _mouseButtonFlags(0) {
    _mousePreviousPosition = std::make_tuple(windowSize_X / 2.0, windowSize_Y / 2.0);
    _mouseCurrentPosition = _mousePreviousPosition;
    memset(&_keyPressed[0], 0, sizeof(_keyPressed));
    glfwInit( );
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _minor);
		
    /* When you want core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    */
    
    _window = glfwCreateWindow(windowSize_X, windowSize_Y, windowTitle, nullptr, nullptr);
    if(_window){
      glfwSetWindowPos(_window, std::get<0>(position), std::get<1>(position));
      glfwSetWindowUserPointer(_window, this);
      glfwSetMouseButtonCallback(_window, GLFWApp::_mouseButtonCallback);
      glfwSetCursorPosCallback(_window, GLFWApp::_cursorPositionCallback);
      glfwSetKeyCallback(_window, GLFWApp::_keyCallback);
      glfwMakeContextCurrent(_window);
      glewExperimental = GL_TRUE;
      glewInit( );
      sync(VSYNC);
    	FreeImage_Initialise( );
      assert(checkGLError("Constructor"));
    }else{
      fprintf(stderr, "Failed constructor (%d, %d)\n", _myGLVersion( ), _reportedGLVersion( ));
      assert(_window);
    }
  }

  virtual ~GLFWApp( ){
    if(_window){
      glfwDestroyWindow(_window);
    }
  	FreeImage_DeInitialise( );
    glfwTerminate( );
  }

  void sync(syncmode_t const & sync){
    switch(sync){
    case ASYNC:
      glfwSwapInterval(0);
      break;
    case VSYNC:
      glfwSwapInterval(1);
      break;
    case TEARING:
      glfwSwapInterval(-1);
      break;
    default:
      fprintf(stderr, "No such syncmode. (%d)\n", sync);
      break;
    }
  }

  void swap( ){
    glfwSwapBuffers(_window);
  }

  virtual bool begin( ) = 0;
  virtual bool render( ) = 0;
  virtual bool end( ) = 0;

  void windowShouldClose( ){
    glfwSetWindowShouldClose(_window, GL_TRUE);
  }
  
  int operator( )( ){
    int rv = EXIT_FAILURE;
    if(_window != 0){
      rv = this->begin() ? EXIT_SUCCESS : EXIT_FAILURE;
      while(rv == EXIT_SUCCESS){
        rv = this->render() ? EXIT_SUCCESS : EXIT_FAILURE;
        rv = rv && this->checkGLError("Render");
        glfwPollEvents( );
        if(glfwWindowShouldClose(_window)){
          break;
        }
        swap( );
      }
      rv = rv && this->end();
    }
    return rv;
  }

  std::tuple<int, int> windowSize( ) const{
    int x, y;
    glfwGetFramebufferSize(_window, &x, &y);
    return std::make_tuple(x, y);
  }
  
  int windowWidth( ) const{
    std::tuple<int, int> w = windowSize( );
    return std::get<0>(w);
  }
  
  int windowHeight( ) const{
    std::tuple<int, int> w = windowSize( );
    return std::get<1>(w);
  }
  

  GLFWwindow* window( ){
    return _window;
  }

  bool isKeyPressed(int key) const{
    return _keyPressed[key];
  }

  void keyUp(int key){
    _keyPressed[key] = false;
  }

 protected:   
  bool checkGLError(const char *msg){
    bool ret = true;
    GLenum err = glGetError( );
    std::string errorString;
    while(err != GL_NO_ERROR){
      ret = false;
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
      fprintf(stderr, "GL ERROR(%s): %s\n", msg, errorString.c_str( ));
      err = glGetError( );
    }
    return( ret );
  }

  std::tuple<int, int> mouseCurrentPosition( ){
    return _mouseCurrentPosition;
  }
  
  int mouseButtonFlags( ){
    return _mouseButtonFlags;
  }

 private:
  GLFWwindow* _window;
  std::string _windowTitle;
  int _major;
  int _minor;
  std::array<bool, 512> _keyPressed;
  int _mouseButtonFlags;
  std::tuple<float, float> _mousePreviousPosition;
  std::tuple<float, float> _mouseCurrentPosition;

  static void _mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    GLFWApp *app = reinterpret_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    assert(app != nullptr);
    switch(action){
    case GLFW_PRESS:
      {
        app->_mousePreviousPosition = app->_mouseCurrentPosition;
        switch(button)
          {
          case GLFW_MOUSE_BUTTON_LEFT:
            {
              app->_mouseButtonFlags |= GLFWApp::MOUSE_BUTTON_LEFT;
            }
            break;
          case GLFW_MOUSE_BUTTON_MIDDLE:
            {
              app->_mouseButtonFlags |= GLFWApp::MOUSE_BUTTON_MIDDLE;
            }
            break;
          case GLFW_MOUSE_BUTTON_RIGHT:
            {
              app->_mouseButtonFlags |= GLFWApp::MOUSE_BUTTON_RIGHT;
            }
            break;
          }
      }
      break;
    case GLFW_RELEASE:
      {
        switch(button)
          {
          case GLFW_MOUSE_BUTTON_LEFT:
            {
              app->_mouseButtonFlags &= ~GLFWApp::MOUSE_BUTTON_LEFT;
            }
            break;
          case GLFW_MOUSE_BUTTON_MIDDLE:
            {
              app->_mouseButtonFlags &= ~GLFWApp::MOUSE_BUTTON_MIDDLE;
            }	
            break;
          case GLFW_MOUSE_BUTTON_RIGHT:
            {
              app->_mouseButtonFlags &= ~GLFWApp::MOUSE_BUTTON_RIGHT;
            }
            break;
          }
      }
      break;
      }
  }
  
  static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key < 0){
      return;
    }
    GLFWApp *app = reinterpret_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    assert(app != nullptr);
    app->_keyPressed[key] = (action == KEY_PRESS || action == GLFW_REPEAT);
    if(app->isKeyPressed(GLFW_KEY_ESCAPE)){
      app->end( );
    }
  }
  
  static void _cursorPositionCallback(GLFWwindow* window, double x, double y){
    GLFWApp *app = reinterpret_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    assert(app != nullptr);
    app->_mouseCurrentPosition = std::make_tuple(int(floor(x)), int(floor(y)));
  }
  
 	int _version(int major, int minor) const{
    return major * 100 + minor * 10;
  }

  int _myGLVersion( ) const{
    return _major * 100 + _minor * 10;
  }

  int _reportedGLVersion( ) const{
  	GLint major = 0;
  	GLint minor = 0;
  	glGetIntegerv(GL_MAJOR_VERSION, &major);
  	glGetIntegerv(GL_MINOR_VERSION, &minor);
    return _version(major, minor);
  }
  
  bool _checkGLVersion( ) const{
    return _reportedGLVersion( ) >= _myGLVersion( );
  }

  static void _glfwSimpleErrorCallback(int error, const char* description){
    fprintf(stderr, "GLFW Error: %s\n", description);
  }


}; // end GLFWApp

#endif
