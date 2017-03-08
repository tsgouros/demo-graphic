# demo-graphic

A fairly simple 3D graphics program that use OpenGL shaders.

This uses FreeGLUT, which can be installed on a Mac using Homebrew.
It is, at least on my machine, limited to OpenGL 2.1 and GLSL 120.  It
also uses GLEW, and includes an updated cmake Find module for both of
these libraries.

To install FreeGLUT, if needed, using Homebrew:

    $ brew install freeglut

I am using 2.8.1. Version 3.0.0 should work, but I haven't tested
that.  The older (2.8.1) version is the one supported in the yurt.

To install GLEW, if needed, using Homebrew:

    $ brew install glew

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
finds MinVR, it will create a demo3.

## Downloading and installing MinVR

Try this.  It should look slightly familiar, but we are selecting only
some features of MinVR to use for now, and are "installing" MinVR
somewhere so it can be available to other programs that want to link
to it.

    $ git clone http://github.com/MinVR/MinVR.git
    $ cd MinVR
    $ git checkout beta  #<-- tested with commit 376569e
    $ mkdir build
    $ cd build
 *  $ cmake .. -DBASE_PLUGINS=OFF -DMINVR_OPENGL_PLUGIN=ON -DMINVR_FREEGLUT_PLUGIN=ON
    $ make
    $ make install       #<-- don't forget this step.  You'll get a
                              "can't find MinVR/api.h" error if you do.

This will install MinVR in an "install" directory inside the "build"
directory you just created.  You can put it somewhere else by adding
"-DCMAKE_INSTALL_PREFIX=/my/path/to/install" to the cmake line above.

    Note: Use the "master" branch on MinVR.  This is the default when
          you clone the repo, so you shouldn't have to worry about it.

* If this fails and the cmake command reports that it cannot find
  FreeGLUT, try this hack.  The cmake program uses a file with hints
  in it to try to figure out where to find your freeglut installation.
  The hints in the demo-graphic FindFreeGLUT file are better than in
  the MinVR version.  While we wait for the better version to become
  part of the MinVR release, go to your MinVR repo and do this:

    $ cd MinVR   # <-- you are at the root of the MinVR tree.
    $ cp /path/to/my/demo-graphic/cmake/FindFreeGLUT.cmake plugins/FREEGLUT/cmake/

  Then try the build again from the cmake step above.

After successfully building the MinVR package and getting past the
"make install" step, you will need to rebuild the demo-graphic repo,
by going over to its build directory and issuing a command like this:

    $ cmake .. -DMINVR_INSTALL_DIR=/my/path/to/MinVR/build/install
    $ make

The "make" command will only rebuild programs it perceives to have
changed since the last time you built them.  In this case, since what
has changed is something outside the demo-graphic repo, you might have
to go edit some of the files to let make know they need to be rebuilt.
Add a space and remove it or something like that.  Dumb, but
effective, like a lot of computing practice.

To run a MinVR application, you'll need to set the MINVR_ROOT
environment variable, and specify a configuration file.

    $ cd /path/to/my/MinVR
    $ export MINVR_ROOT=$(pwd)
    $ cd /path/to/my/demo-graphic/build
    $ bin/demo3 ../config/desktop-freeglut.xml ../src/shader2.vp ../src/shader.fp


### Running on the yurt.

On the yurt, you'll have to do these:

    $ module load centos-updates
    $ module load centos-libs
    $ module load nvidia-driiver
    $ module load vrpn
    $ module load cave
    $ module load freeglut
    $ module load libpng

These commands can be put in a file called ~/.modules.

More to come...



