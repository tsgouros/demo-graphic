#version 330
// GLSL Version 1.2, as in the vertex shader.

// Like the vertex shader (read that one if you haven't yet), this is
// about the simplest possible fragment shader.  It just takes a color
// value from the vertex shader, and adds an alpha value to it, before
// assigning it to the pre-defined output name.

// Here's our input from the output of the vertex shader.
in vec4 colorFrag;
out vec4 color;

void main()
{
  // This is a predefined output value -- spelled in exactly this way
  // -- to be the color assigned to the given vertex.  All we're doing
  // here is taking the RGB value in Color and making it into an RGBA
  // value.
  color = colorFrag;
}
