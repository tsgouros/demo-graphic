
#ifndef MVRDEMO_H_
#define MVRDEMO_H_

#include "main/VRMain.h"

/// MVRDemo is a simple way to create a graphics VR application.  Developers
/// need only to subclass MVRDemo and override the onVREvent(...),
/// onVRRender(...), and/or onVRRenderContext(...).  To run the application,
/// developers can call run.  To shut down the application, call the
/// shutdown() method.
///
/// A simple MinVR Graphics application:
///
///  ------------------------------------------------------------------------
/// #include <MVRDemo.h>
///
/// class MyMVRDemo : public MVRDemo {
/// public:
/// MyMVRDemo(int argc, char** argv) : MVRDemo(argc, argv) {}
///
///   virtual void onVRRenderScene(VRState& renderState) {
///       // draw graphics
///   }
/// };
///
/// int main(int argc, char **argv) {
///   MyMVRDemo app(argc, argv);
///   app.run();
///   return 0;
/// }

class MVRDemo : public MinVR::VREventHandler, public MinVR::VRRenderHandler {
 public:

  typedef MinVR::VRDataIndex VRState;

	///  MVRDemo expects command line parameters using the MinVR command line
	///  convention.  The options that MinVR does not use will be available
  /// through the getLeftoverArg* methods.
	MVRDemo(int argc, char** argv) {

    _main = new MinVR::VRMain();

    _main->addEventHandler(this);
    _main->addRenderHandler(this);
    _main->initialize(argc, argv);
  }


	/// Default destructor shuts down the application and the interface with
	/// MinVR.
	virtual ~MVRDemo() { delete _main; }

  // Toy versions of the three important functions are provided for
  // debugging and learning.  You will need to override all three
  // methods to make your application work.
  
	/// This function is called once for each time a display node requires the
  ///	scene -- whatever it is -- to be rendered.  For example, a stereo display
  ///	node will require the scene to be drawn twice (once per eye).  A stereo
  ///	audio thing might require it to be rendered once for each ear.  A
  ///	multiple-viewer VR thing might require it to be rendered once for each eye
  ///	for each viewer.
  ///
  /// In other words, this is where the *scene* is assembled and loaded into
  /// whatever buffers it needs to be loaded into.
	virtual void onRenderContext(const VRState& stateData) {
      std::cout << stateData;
  };

  /// Within a context, a scene must be rendered for each of the "viewers" in
  /// that scene, be they an eyeball or an ear.  For a stereo graphics
  /// application, this would be once for the left eye and once for the right.
  /// For an audio application, once for each ear.  For a multiple-viewer VR
  /// application, once for each eye for each viewer.  For a haptic
  /// installation, once for each finger or hand or limb or whatever.
	virtual void onRenderScene(const VRState& stateData) {
      std::cout << stateData;
  };

  /// This handler is used to accept and handle event data from the
  /// various parts that issue events. 
  virtual void onVREvent(const MinVR::VREvent &eventData) {
      std::cout << eventData;
  };
  
	/// Starts the application and does not return until the application exits.
	void run() { while (_main->mainloop()) {} };

	/// Frees memory and other resources.  Typically called after run() completes.
  /// If shutdown() is called while the app is still running, it will cause the
  /// app to exit from run() the next time through the mainloop. 
	void shutdown() { _main->shutdown(); };

  /// After parsing the command line, the number of arguments unused by MinVR
  /// can be retrieved with this function. 
  int getLeftoverArgc() { return _main->getLeftoverArgc(); };

  /// After parsing the command line, the values of arguments unused by MinVR
  /// can be retrieved with this function.  
  char** getLeftoverArgv() { return _main->getLeftoverArgv(); };

 private:

  MinVR::VRMain* _main;
  
};

#endif //MVRDEMO_H_
