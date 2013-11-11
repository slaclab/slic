// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/VRML2WriterMessenger.cc,v 1.2 2012-11-27 19:32:19 jeremy Exp $

#include "VRML2WriterMessenger.hh"

// slic
#include "VRML2Writer.hh"

// geant4
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"

namespace slic {

VRML2WriterMessenger::VRML2WriterMessenger() {
	m_vrmlDir = new G4UIdirectory("/vrml/");
	m_vrmlDir->SetGuidance("Test VRML2Writer. [SLIC]");

	m_writeVrmlCmd = new G4UIcmdWithAString("/vrml/writeVRML", this);
	m_writeVrmlCmd->SetGuidance("Write VRML of current geometry to a file.");

}

VRML2WriterMessenger::~VRML2WriterMessenger() {
	;
}

void VRML2WriterMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
	G4String fname;
	if (newVals == "")
		fname = "geometry.wrl";
	else
		fname = newVals;
	VRML2Writer* writer = new VRML2Writer(fname);
	writer->write();
	delete writer;
	writer = 0;
}
}
