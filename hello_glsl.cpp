// 
// Michael Shafae
// mshafae at fullerton.edu
// 
// A toy program which renders a teapot and two light sources. 
//
//

#include <tuple>
#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GLFWApp.h"
#include "GLSLShader.h"
#include "glut_teapot.h"

void msglVersion(void){
  fprintf(stderr, "OpenGL Version Information:\n");
  fprintf(stderr, "\tVendor: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "\tRenderer: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "\tOpenGL Version: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "\tGLSL Version: %s\n",
          glGetString(GL_SHADING_LANGUAGE_VERSION));
}

class SpinningLight{
public:
  SpinningLight( ){ }
  SpinningLight(glm::vec3& color, glm::vec3& position, glm::vec3& center) :
    _rotationDelta(0.05), _color(color), _position(position), _center(center), _savedColor(color), _isOn(true){
      glm::vec3 random_vector = glm::sphericalRand(1.0);
      glm::vec3 d = direction( );
      _tangent = glm::cross(d, random_vector);
    }

  glm::vec4 color( ){return glm::vec4(_color, 1.0);}
  glm::vec4 position( ){return glm::vec4(_position, 1.0);}
  
  void rotateUp( ){
    glm::vec3 f = direction( );
    glm::vec3 _up = glm::normalize(_tangent);
    glm::vec3 s = glm::normalize(glm::cross(f, _up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat3 m = glm::rotate(_rotationDelta, s);
    _tangent = m * u;
    _position = m * _position;
  }

  void rotateDown( ){
    glm::vec3 f = direction( );
    glm::vec3 _up = glm::normalize(_tangent);
    glm::vec3 s = glm::normalize(glm::cross(f, _up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat3 m = glm::rotate(_rotationDelta, s);
    _tangent = m * u;
    _position = m * _position;
  }

  void rotateLeft( ){
    glm::vec3 f = glm::normalize(direction( ));
    glm::vec3 _up = glm::normalize(_tangent);
    glm::vec3 s = glm::normalize(glm::cross(f, _up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat3 m = glm::rotate(_rotationDelta, u);
    _position = m * _position;
  }

  void rotateRight( ){
    glm::vec3 f = glm::normalize(direction( ));
    glm::vec3 _up = glm::normalize(_tangent);
    glm::vec3 s = glm::normalize(glm::cross(f, _up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat3 m = glm::rotate(_rotationDelta, u);
    _position = m * _position;
  }

  void roll( ){
    glm::mat3 m = glm::rotate(-_rotationDelta, direction( ));
    _tangent = m * _tangent;
  }
  
  void toggle( ){
    _isOn = !_isOn;
    if( _isOn ){
      _color = _savedColor;
    }else{
      _color = glm::vec3(0.0, 0.0, 0.0);
    }
  }

  void draw( ){

  }
private:
  float _rotationDelta;
  glm::vec3 _color;
  glm::vec3 _position;
  glm::vec3 _center;
  glm::vec3 _tangent;
  glm::vec3 _savedColor;
  bool _isOn;
  
  glm::vec3 direction( ){
    glm::vec3 d;
    d = glm::normalize(_center - _position);
    return d;
  }

  void debug( ){
    std::cerr << "position " << glm::to_string(_position) << "(" << glm::length(_position) << ")" << "\ncenter " << glm::to_string(_center) << "\ntangent " << glm::to_string(_tangent) << "(" << glm::length(_tangent) << ")" << std::endl << std::endl;
  }
};

class HelloGLSLApp : public GLFWApp{
private:
  float rotationDelta;

  glm::vec3 centerPosition;
  glm::vec3 eyePosition;
  glm::vec3 upVector;

  glm::mat4 modelViewMatrix;
  glm::mat4 projectionMatrix;
  glm::mat4 normalMatrix;
  
  GLSLProgram shaderProgram_A;

  SpinningLight light0;
  SpinningLight light1; 

  // Variables to set uniform params for lighting fragment shader 
  unsigned int uModelViewMatrix_A;
  unsigned int uProjectionMatrix_A;
  unsigned int uNormalMatrix_A;
  unsigned int uLight0_position_A;
  unsigned int uLight0_color_A;
  unsigned int uLight1_position_A;
  unsigned int uLight1_color_A;
  
public:
  HelloGLSLApp(int argc, char* argv[]) :
    GLFWApp(argc, argv, std::string("Camera Control").c_str( ), 
            600, 600){ }
  
  void initCenterPosition( ){
    centerPosition = glm::vec3(0.0, 0.0, 0.0);
  }
  
  void initEyePosition( ){
    eyePosition = glm::vec3(0.0, 0.0, 5.0);
  }

  void initUpVector( ){
    upVector = glm::vec3(0.0, 1.0, 0.0);
  }

  void initRotationDelta( ){
    rotationDelta = 0.05;
  }
   
  void initLights( ){
    glm::vec3 color0(1.0, 0.0, 0.0);
    glm::vec3 position0(0.0, 5.0, 10.0);
    glm::vec3 color1(0.0, 0.0, 1.0);
    glm::vec3 position1(0.0, 5.0, -10.0);
    light0 = SpinningLight(color0, position0, centerPosition);
    light1 = SpinningLight(color1, position1, centerPosition);
  }
    
  void rotateCameraLeft( ){
    glm::vec3 f = glm::normalize(-eyePosition);
    glm::vec3 _up = glm::normalize(upVector);
    glm::vec3 s = glm::normalize(glm::cross(f, _up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat3 m = glm::rotate(rotationDelta, u);
    eyePosition = m * eyePosition;
  }

  void rotateCameraRight( ){
    glm::vec3 f = glm::normalize(-eyePosition);
    glm::vec3 _up = glm::normalize(upVector);
    glm::vec3 s = glm::normalize(glm::cross(f, _up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat3 m = glm::rotate(-rotationDelta, u);
    eyePosition = m * eyePosition;
  }

  void rotateCameraUp( ){
    glm::vec3 f = glm::normalize(-eyePosition);
    glm::vec3 _up = glm::normalize(upVector);
    glm::vec3 s = glm::normalize(glm::cross(f, _up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat3 m = glm::rotate(-rotationDelta, s);
    upVector = m * u;
    eyePosition = m * eyePosition;
  }

  void rotateCameraDown( ){
    glm::vec3 f = glm::normalize(-eyePosition);
    glm::vec3 _up = glm::normalize(upVector);
    glm::vec3 s = glm::normalize(glm::cross(f, _up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat3 m = glm::rotate(rotationDelta, s);
    upVector = m * u;
    eyePosition = m * eyePosition;
  }


  bool begin( ){
    msglError( );
    initCenterPosition( );
    initEyePosition( );
    initUpVector( );
    initRotationDelta( );
    initLights( );
    
    // Load shader program A
    const char* vertexShaderSource_A = "blinn_phong.vert.glsl";
    const char* fragmentShaderSource_A = "blinn_phong.frag.glsl";
    FragmentShader fragmentShader_A(fragmentShaderSource_A);
    VertexShader vertexShader_A(vertexShaderSource_A);
    shaderProgram_A.attach(vertexShader_A);
    shaderProgram_A.attach(fragmentShader_A);
    shaderProgram_A.link( );
    shaderProgram_A.activate( );
    
    printf("Shader program A built from %s and %s.\n",
           vertexShaderSource_A, fragmentShaderSource_A);
    if( shaderProgram_A.isActive( ) ){
      printf("Shader program is loaded and active with id %d.\n", shaderProgram_A.id( ) );
    }else{
      printf("Shader program did not load and activate correctly. Exiting.");
      exit(1);
    }
    
        // Set up uniform variables for shader program A
    uModelViewMatrix_A = glGetUniformLocation(shaderProgram_A.id( ), "modelViewMatrix");
    uProjectionMatrix_A = glGetUniformLocation(shaderProgram_A.id( ), "projectionMatrix");
    uNormalMatrix_A = glGetUniformLocation(shaderProgram_A.id( ), "normalMatrix");
    uLight0_position_A = glGetUniformLocation(shaderProgram_A.id( ), "light0_position");
    uLight0_color_A = glGetUniformLocation(shaderProgram_A.id( ), "light0_color");
    uLight1_position_A = glGetUniformLocation(shaderProgram_A.id( ), "light1_position");
    uLight1_color_A = glGetUniformLocation(shaderProgram_A.id( ), "light1_color");

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    msglVersion( );
    
    return !msglError( );
  }
  
  bool end( ){
    windowShouldClose( );
    return true;
  }
  
  void activateUniforms_A(glm::vec4& _light0, glm::vec4& _light1){
    glUniformMatrix4fv(uModelViewMatrix_A, 1, false, glm::value_ptr(modelViewMatrix));    
    glUniformMatrix4fv(uProjectionMatrix_A, 1, false, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uNormalMatrix_A, 1, false, glm::value_ptr(normalMatrix));
    glUniform4fv(uLight0_position_A, 1, glm::value_ptr(_light0));
    glUniform4fv(uLight0_color_A, 1, glm::value_ptr(light0.color( )));
    glUniform4fv(uLight1_position_A, 1, glm::value_ptr(_light1));
    glUniform4fv(uLight1_color_A, 1, glm::value_ptr(light1.color( )));
  }
  
  bool render( ){
    glm::vec4 _light0;
    glm::vec4 _light1;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::tuple<int, int> w = windowSize( );
    double ratio = double(std::get<0>(w)) / double(std::get<1>(w));

    projectionMatrix = glm::perspective(90.0, ratio, 1.0, 25.0);

    glm::mat4 lookAtMatrix = glm::lookAt(eyePosition, centerPosition, upVector);

    // Set light & material properties for the teapot;
    // lights are transformed by current modelview matrix
    // such that they are positioned correctly in the scene.
    _light0 = lookAtMatrix * light0.position( );
    _light1 = lookAtMatrix * light1.position( );
    
    modelViewMatrix = lookAtMatrix;
    normalMatrix = glm::inverseTranspose(modelViewMatrix);
    shaderProgram_A.activate( );
    activateUniforms_A(_light0, _light1);
    _glutSolidTeapot(1.0);
    light0.draw( );
    
    if(isKeyPressed('Q')){
      end( );      
    }else if(isKeyPressed(GLFW_KEY_EQUAL)){

    }else if(isKeyPressed(GLFW_KEY_MINUS)){

    }else if(isKeyPressed('R')){
      initEyePosition( );
      initUpVector( );
      initRotationDelta( );
      initLights( );  
      printf("Eye position, up vector and rotation delta reset.\n");
    }else if(isKeyPressed(GLFW_KEY_LEFT)){
      rotateCameraRight( );
    }else if(isKeyPressed(GLFW_KEY_RIGHT)){
      rotateCameraLeft( );
    }else if(isKeyPressed(GLFW_KEY_UP)){
      rotateCameraDown( );
    }else if(isKeyPressed(GLFW_KEY_DOWN)){
      rotateCameraUp( );
    }else if(isKeyPressed('W')){
      light0.rotateUp( );
    }else if(isKeyPressed('S')){
      light0.rotateUp( );
    }else if(isKeyPressed('A')){
      light0.rotateLeft( );
    }else if(isKeyPressed('D')){
      light0.rotateLeft( );
    }else if(isKeyPressed('X')){
      light0.roll( );
    }else if(isKeyPressed('Y')){
      light1.rotateUp( );
    }else if(isKeyPressed('H')){
      light1.rotateUp( );
    }else if(isKeyPressed('G')){
      light1.rotateLeft( );
    }else if(isKeyPressed('J')){
      light1.rotateLeft( );
    }else if(isKeyPressed('N')){
      light1.roll( );
    }else if(isKeyPressed('1')){
      light0.toggle( );
    }else if(isKeyPressed('2')){
      light1.toggle( );
    }
    return !msglError( );
  }
    
};


int main(int argc, char* argv[]){
  HelloGLSLApp app(argc, argv);
  return app();
}

