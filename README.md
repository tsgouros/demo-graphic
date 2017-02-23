# demo-graphic

A fairly simple 3D graphics program that use OpenGL shaders.

This uses FreeGLUT, which can be installed on a Mac using Homebrew.
It is, at least on my machine, limited to OpenGL 2.1 and GLSL 120.  It
also uses GLEW, and includes an updated cmake Find module for both of
these libraries.

To build on a Mac or on Linux:

    $ git clone http://github.com/tsgouros/demo-graphic.git
    $ cd demo-graphic
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ bin/demo

It's a cmake build, so if you're using XCode on Mac, or Windows, this
will probably work, too, but someone else has to make the instructions
for those, and I will happily include them here.