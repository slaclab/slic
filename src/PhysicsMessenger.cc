// SLIC
#include "PhysicsMessenger.hh"
#include "PhysicsListManager.hh"
#include "HepPDTManager.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// Geant4
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"

namespace slic {

PhysicsMessenger::PhysicsMessenger() :
		m_physicsListDir(0), m_selectCmd(0) {
	defineCommands();
}

PhysicsMessenger::~PhysicsMessenger() {
}

void PhysicsMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
	std::istringstream is((const char*) newVals);

	std::string s;
	is >> s;

	PhysicsListManager* mgr = PhysicsListManager::instance();

	// set physics list
	if (cmd == m_selectCmd) {
		mgr->setCurrentListName(s);
	}
	// print available lists
	else if (cmd == m_printListsCmd) {
		PhysicsListManager::instance()->printAvailablePhysicsLists();
	}
	// Print current list and whether initialized.
	else if (cmd == m_printCurrentCmd) {
		mgr->log() << LOG::okay << "Current physics list <" << mgr->getCurrentListName() << ">." << LOG::done;
		mgr->log() << LOG::okay << "Physics has been initialized <" << mgr->isInitialized() << ">."
				<< LOG::done;
	}
	// Enable optical physics processes.
	else if (cmd == m_enableOpticalCmd) {
		bool enableOptical = true;
		if (newVals != G4String(""))
			enableOptical = G4UIcmdWithABool::GetNewBoolValue(newVals);
		PhysicsListManager::instance()->enableOptical(enableOptical);
	}
	// Set location of particle.tbl for HepPDT.
	else if (cmd == m_pdgCmd) {
		HepPDTManager::instance()->setParticleDataFile(s);
	} else {
		mgr->log() << LOG::error << "Unknown command for PhysicsMessenger." << LOG::done;
	}
}

void PhysicsMessenger::defineCommands() {
	G4UIparameter* p;

	m_physicsListDir = new G4UIdirectory("/physics/");
	m_physicsListDir->SetGuidance("Physics list commands. [SLIC]");

	// select physics list
	m_selectCmd = new G4UIcommand("/physics/select", this);
	m_selectCmd->SetGuidance(
			"Select the Geant4 physics list.  This command is only available in the PreInit state.");
	p = new G4UIparameter("list", 's', false);
	m_selectCmd->SetParameter(p);
	m_selectCmd->AvailableForStates(G4State_PreInit);

	// print available lists
	m_printListsCmd = new G4UIcmdWithoutParameter("/physics/printLists", this);
	m_printListsCmd->SetGuidance("Print available physics lists.");

	// print current list
	m_printCurrentCmd = new G4UIcmdWithoutParameter("/physics/printCurrent", this);
	m_printCurrentCmd->SetGuidance("Print name of currently selected physics list.");

	// Enable optical physics processes for the current list when it is initialized.
	m_enableOpticalCmd = new G4UIcmdWithABool("/physics/enableOptical", this);
	m_enableOpticalCmd->SetGuidance("Enable optical physics processes.");
	m_enableOpticalCmd->SetParameterName("enable", 'b', true);
	m_enableOpticalCmd->SetDefaultValue(true);

	// Set location of particle.tbl for supplementary particle data.
	m_pdgCmd = new G4UIcommand("/physics/setPDGFile", this);
	m_pdgCmd->SetGuidance("Set location of particle data for HepPDT, probably called particle.tbl.");
	p = new G4UIparameter("file", 's', false);
	m_pdgCmd->SetParameter(p);
	m_pdgCmd->AvailableForStates(G4State_PreInit);
}
}
