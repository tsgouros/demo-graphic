# demo-graphic

A fairly simple 3D set of example graphics programs that use OpenGL
shaders and the MinVR virtual reality library.  It's meant to provide,
if not an easy introduction to programming VR with OpenGL, at least a
less painful one.

You'll find example code in the examples directory, starting with the
basics of OpenGL buffer objects and shaders.  There are some simple
shaders in the shaders directory that are used by the example
applications.

You'll also find code for a "Baby Scene Graph" library in the src
directory, and the example code relies on it.  This is a lightweight
scene graph library that is built to illustrate how to do matrix
manipulation and OpenGL buffer management.  The goal is to have
something with which one can develop VR programs on a laptop and have
them run equally well on a head-mounted display and an immersive
environment, such as Brown University's YURT.  The MinVR SDK provides
this kind of functionality, and the BSG library here will help you
stay in-bounds of acceptable usage.

The BSG code has lots of comments, and where it seemed appropriate, I
chose clarity over optimization, so I make no promises about
performance and features.  That is, if you find it's not fast enough,
or missing reflection maps or something, you've outgrown the training
wheels, and should be using a real scene graph package, like OSG.

 -Tom Sgouros
  April, 2017

  Center for Computation and Visualization,
  Brown University


The build instructions that follow work on Mac (command line) and
Linux.  Please submit additions to this file for builds.  Current
wishlist:

  - Windows
  - Mac XCode
  - QtCreator


## FreeGLUT

This uses FreeGLUT, which can be installed on a Mac using Homebrew.
It is, at least on my machine, limited to OpenGL 2.1 and GLSL 120.  It
also uses GLEW, and includes an updated cmake Find module for both of
these libraries.

To install FreeGLUT, if needed, using Homebrew:

    $ brew install freeglut

I am using 2.8.1. Version 3.0.0 should work, but I haven't tested
that.  The older (2.8.1) version is the one supported in the CCV YURT.

## GLEW

To install GLEW, if needed, using Homebrew:

    $ brew install glew

## Build instructions

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

If cmake finds libPNG it will build a demo2 binary, and if it also
finds MinVR, it will create a demo3.  You will find them in the bin
subdirectory of your build directory.

### Documentation

There is a significant amount of documentation in the code, and you
can get a doxygen dump of the API comments by adding
'-DBUILD_DOCUMENTATION=ON' to the cmake invocation.


## Downloading and installing MinVR

Try this.  It should look slightly familiar, but we are selecting only
some features of MinVR to use for now, and are "installing" MinVR
somewhere so it can be available to other programs that want to link
to it.

    $ git clone http://github.com/MinVR/MinVR.git
    $ cd MinVR
    $ mkdir build
    $ cd build
    $ cmake .. -DBASE_PLUGINS=OFF -DMINVR_OPENGL_PLUGIN=ON -DMINVR_FREEGLUT_PLUGIN=ON
    $ make
    $ make install       #<-- don't forget this step.  You'll get a
                              "can't find MinVR/api.h" error if you do.

This will install MinVR in an "install" directory inside the "build"
directory you just created.  You can put it somewhere else by adding
"-DCMAKE_INSTALL_PREFIX=/my/path/to/install" to the cmake line above.

After successfully building the MinVR package and getting past the
"make install" step (don't forget that part), you will need to rebuild
the demo-graphic repo, by going over to its build directory and
issuing a command like this:

    $ cmake .. -DMINVR_INSTALL_DIR=/my/path/to/MinVR/build/install
    $ make

The "make" command will only rebuild programs it perceives to have
changed since the last time you built them.  In this case, since what
has changed is something outside the demo-graphic realm, you might have
to use the '-B' option of make:

    $ make -B

This should rebuild everything.

Alternatively, you can go edit some of the files to let make know they
need to be rebuilt.  Add a space and remove it or something like that.
Dumb, but effective, like a lot of computing practice.

To run a MinVR application, you'll need to set the MINVR_ROOT
environment variable, and specify a configuration file.  There are
some MinVR configuration files in the config directory.

    $ cd /path/to/my/MinVR
    $ export MINVR_ROOT=$(pwd)
    $ cd /path/to/my/demo-graphic/build
    $ bin/demo3 ../config/desktop-freeglut.xml ../src/shader2.vp ../src/shader.fp

Note that MINVR_ROOT points to the root of the MinVR *installation*,
not the MinVR *repo*.


### Running on the CCV YURT.

On the OSCAR machines that run the YURT, you will not have to compile
MinVR or set environment variables, but you will have to do these:

    $ module load centos-updates
    $ module load centos-libs
    $ module load nvidia-driver
    $ module load vrpn
    $ module load cave
    $ module load freeglut
    $ module load libpng
    $ module load MinVR

These commands can be put in a file called ~/.modules.  You may find
it convenient to copy my .modules file:

    $ cp /gpfs/home/tsgouros/.modules ~/

More to come...



