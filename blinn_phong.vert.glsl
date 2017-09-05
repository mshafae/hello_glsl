# version 120 
/*
 * Michael Shafae
 * mshafae at fullerton.edu
 * 
 * A simple Blinn-Phong shader with two light sources.
 * This file is the vertex shader which transforms the
 * input vertex into eye coordinates and passes along
 * the vertex's color and normal to the fragment shader.
 *
 * For more information see:
 *     <http://en.wikipedia.org/wiki/Blinn–Phong_shading_model>
 *
 * $Id: blinn_phong.vert.glsl 4891 2014-04-05 08:36:23Z mshafae $
 *
 * Be aware that for this course, we are limiting ourselves to
 * GLSL v.1.2. This is not at all the contemporary shading
 * programming environment, but it offers the greatest degree
 * of compatability.
 *
 * Please do not use syntax from GLSL > 1.2 for any homework
 * submission.
 *
 */

// These are passed in from the CPU program, camera_control_*.cpp
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;


// These are variables that we wish to send to our fragment shader
// In later versions of GLSL, these are 'out' variables.
varying vec3 myNormal;
varying vec4 myVertex;

void main() {
  gl_Position = projectionMatrix * modelViewMatrix * gl_Vertex;
  myNormal = gl_Normal;
  myVertex = gl_Vertex;
}
