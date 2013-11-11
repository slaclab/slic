// $Id: SlicMain.cc,v 1.7 2012-11-28 01:40:48 jeremy Exp $

#include "SlicMain.hh"

// slic
#include "SlicApplication.hh"

// geant4
#include "G4RunManager.hh"

namespace slic {

SlicMain::SlicMain() :
		Module("SlicMain") {
	;
}

SlicMain::~SlicMain() {
	;
}

int SlicMain::main(int argc, char** argv) {
	// Default return code.
	int returnCode = 0;

	// Application instance.
	SlicApplication* slicApp = 0;

	// Top-level try block.
#ifdef SLIC_CATCH_STD_EXCEPTIONS
	try {
		std::cout << "SLIC is enabled to catch std exceptions." << std::endl;
#endif
		// Create the SLIC application using the singleton function.
		slicApp = SlicApplication::instance();

		// Setup the command line arguments.
		slicApp->initialize(argc, argv);

		// Run the app.
		slicApp->run();

		// Get the application return code.
		returnCode = slicApp->getReturnCode();

		// Delete the application.
		if (slicApp) {
			delete slicApp;
		}

#ifdef SLIC_CATCH_STD_EXCEPTIONS
	// Catch a generic std::exception.
    } catch(std::exception& stde) {
	    log() << LOG::error << LOG::head << stde.what() << LOG::done;
	    returnCode = SlicApplication::STD_EXCEPTION;
    }
    // Catch an unknown exception.
    catch(...) {
	    log() << LOG::error << LOG::head << "Top-level catch block caught an unknown exception!" << LOG::done;
	    returnCode = SlicApplication::UNKNOWN_EXCEPTION;
    }
#endif

	return returnCode;
}
} // namespace slic
