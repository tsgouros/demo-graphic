# demo-graphic

A fairly simple 3D graphics program that use OpenGL shaders.

This uses FreeGLUT, which can be installed on a Mac using Homebrew.  It is, at least on my machine, limited to OpenGL 2.1 and GLSL 120.

To build on a Mac or on Linux:

    $ git clone http://github.com:tsgouros/demo-graphic.git
    $ cd demo-graphic
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ bin/demo

If you're using XCode or Windows, this will probably work, too, but
someone else has to make the instructions.