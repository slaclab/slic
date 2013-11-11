// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMcpMessenger.cc,v 1.14 2013-06-26 01:57:02 jeremy Exp $

// SLIC
#include "LcioMcpMessenger.hh"
#include "LcioManager.hh"
#include "LcioMcpManager.hh"
#include "TrajectoryManager.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// Geant4
#include "G4UIparameter.hh"

namespace slic {

LcioMcpMessenger::LcioMcpMessenger() {
	defineCommands();
}

LcioMcpMessenger::~LcioMcpMessenger() {
}

void LcioMcpMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
	std::istringstream is((const char*) newVals);
	std::string s;
	is >> s;

	LcioMcpManager* mgr = LcioMcpManager::instance();

	// set minimum tracking distance
	if (cmd == m_minTrackingDistanceCmd) {
		mgr->setMinimumTrackingDistance(m_minTrackingDistanceCmd->GetNewDoubleValue(newVals));
	} else if (cmd == m_printFinalCollection) {
		mgr->enablePrintFinal(m_printFinalCollection->GetNewBoolValue(newVals.c_str()));
	} else if (cmd == m_printInitialCollection) {
		mgr->enablePrintInitial(m_printInitialCollection->GetNewBoolValue(newVals.c_str()));
	}
	// Enable writing of initial MCParticle collection to event.
	else if (cmd == m_writeInitialCollection) {
		mgr->setWriteInitialMCParticleCollection(m_writeInitialCollection->GetNewBoolValue(newVals.c_str()));
	} else {
		G4Exception("LcioMessenger::setNewValue()", "", JustWarning, "Unknown command");
	}

	return;
}

void LcioMcpMessenger::defineCommands() {
	// Mcp dir
	m_mcpDir = new G4UIdirectory("/mcp/");
	m_mcpDir->SetGuidance("LCIO Monte Carlo Particle commands. [SLIC]");

	// minimumTrackingDistance
	m_minTrackingDistanceCmd = new G4UIcmdWithADoubleAndUnit("/mcp/minimumTrackingDistance", this);
	m_minTrackingDistanceCmd->SetGuidance(
			"Set the minimum travel distance for an input particle to be tracked in Geant4.");
	m_minTrackingDistanceCmd->SetParameterName("minimumTrackingDistance", false, true);
	m_minTrackingDistanceCmd->SetDefaultUnit("mm");
	m_minTrackingDistanceCmd->SetDefaultValue(0.1);
	m_minTrackingDistanceCmd->SetRange("minimumTrackingDistance > 0.");

	// Enable printing of MCParticle collections during the event. 
	m_printInitialCollection = new G4UIcmdWithABool("/mcp/printInitialCollection", this);
	m_printInitialCollection->SetGuidance(
			"Enable printing of the initial MCParticle collection during event processing.");
	m_printInitialCollection->SetParameterName("enable", true);
	m_printInitialCollection->SetDefaultValue(true);

	// Enable printing of MCParticle collections during the event. 
	m_printFinalCollection = new G4UIcmdWithABool("/mcp/printFinalCollection", this);
	m_printFinalCollection->SetGuidance(
			"Enable printing of the final MCParticle collection during event processing.");
	m_printFinalCollection->SetParameterName("enable", true);
	m_printFinalCollection->SetDefaultValue(true);

	m_writeInitialCollection = new G4UIcmdWithABool("/mcp/writeInitialCollection", this);
	m_writeInitialCollection->SetGuidance(
			"Write the initial MCParticle collection from StdHep conversion to the LCEvent.");
	m_writeInitialCollection->SetParameterName("enable", true);
	m_writeInitialCollection->SetDefaultValue(false);
}
}
