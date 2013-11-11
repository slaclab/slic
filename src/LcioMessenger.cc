// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMessenger.cc,v 1.38 2013-06-26 01:57:02 jeremy Exp $

// SLIC
#include "LcioMessenger.hh"
#include "LcioManager.hh"
#include "LcioMcpManager.hh"
#include "LcioFileNamer.hh"
#include "TrajectoryManager.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// Geant4
#include "G4UIcommand.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"
#include "G4UImanager.hh"

// STL
#include <sstream>

namespace slic {

LcioMessenger::LcioMessenger(LcioManager* mgr) :
		m_mgr(mgr) {
	defineCommands();
}

LcioMessenger::~LcioMessenger() {
	delete m_lcioDir;
	delete m_pathCmd;
	delete m_filenameCmd;
}

void LcioMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
	std::istringstream is((const char*) newVals);

	std::string s;
	is >> s;

	// set output path
	if (cmd == m_pathCmd) {
		m_mgr->setPath(s);
	}
	// set filename
	else if (cmd == m_filenameCmd) {
		m_mgr->setFilename(s);
	}
	// fileExistsAction
	else if (cmd == m_fileExistsActionCmd) {
		m_mgr->setFileExistsAction(LcioManager::getFileExistsActionFromString(s));
	}
	// setRunNumber
	else if (cmd == m_setRunNumberCmd) {
		m_mgr->setRunNumber(StringUtil::toInt(s));
	}
	// autoname
	else if (cmd == m_autonameCmd) {
		std::vector<std::string> fields;
		StringUtil::split(newVals, " ", fields);
		m_mgr->setAutonameFields(fields);
	}
	/* dump event */
	else if (cmd == m_dumpEventCmd) {
		m_mgr->enableDumpEvent(m_dumpEventCmd->GetNewBoolValue(newVals.c_str()));
	}
	// flags
	else if (cmd == m_setLongFlagCmd || cmd == m_setPDGFlagCmd) {

		// no args then true
		bool flag_set = true;

		// get arg val if exists
		if (s != std::string("")) {
			flag_set = StringUtil::toBool(s);
		}

		LcioHitsCollectionBuilder* hcb = m_mgr->getHCBuilder();

		if (hcb) {

			// CHBIT_LONG for positions in hits
			if (cmd == m_setLongFlagCmd) {
				hcb->setLongFlag(flag_set);
			}
			// CHBIT_PDG for every PDG contribution in hits
			else if (cmd == m_setPDGFlagCmd) {
				hcb->setPDGFlag(flag_set);
			}
		} else {
			G4Exception("", "", FatalException, "LcioHitsCollectionBuilder is null.");
		}
	}
	// bad command; shouldn't happen
	else {
		G4Exception("LcioMessenger::setNewValue()", "", JustWarning, "Unknown command.");
	}
	return;
}

void LcioMessenger::defineCommands() {
	G4UIparameter* p;

	// Lcio dir
	m_lcioDir = new G4UIdirectory("/lcio/");
	m_lcioDir->SetGuidance("LCIO output commands. [SLIC]");

	// path
	m_pathCmd = new G4UIcommand("/lcio/path", this);
	m_pathCmd->SetGuidance("Set Lcio output path.");
	p = new G4UIparameter("path", 's', false);
	m_pathCmd->SetParameter(p);

	// filename
	m_filenameCmd = new G4UIcommand("/lcio/filename", this);
	m_filenameCmd->SetGuidance("Set Lcio output filename.");
	p = new G4UIparameter("filename", 's', false);
	m_filenameCmd->SetParameter(p);

	// CHBIT_LONG
	m_setLongFlagCmd = new G4UIcommand("/lcio/longFlag", this);
	m_setLongFlagCmd->SetGuidance("Set CHBIT_LONG flag for hit positions in output (default = on).");
	p = new G4UIparameter("Value", 'b', true);
	m_setLongFlagCmd->SetParameter(p);
	m_setLongFlagCmd->AvailableForStates(G4State_PreInit);

	// CHBIT_PDG
	m_setPDGFlagCmd = new G4UIcommand("/lcio/PDGFlag", this);
	m_setPDGFlagCmd->SetGuidance(
			"Set CHBIT_PDG flag for super-long output with all Mcp contribs (default is aggregated).");
	p = new G4UIparameter("Value", 'b', true);
	m_setPDGFlagCmd->SetParameter(p);
	m_setLongFlagCmd->AvailableForStates(G4State_PreInit);

	// fileExistsAction
	m_fileExistsActionCmd = new G4UIcommand("/lcio/fileExists", this);
	m_fileExistsActionCmd->SetGuidance(
			"Set action when an Lcio output file exists: fail, overwrite/delete or append.");
	p = new G4UIparameter("mode", 's', false);
	m_fileExistsActionCmd->SetParameter(p);

	// set the starting run number
	m_setRunNumberCmd = new G4UIcmdWithAnInteger("/lcio/runNumber", this);
	m_setRunNumberCmd->SetGuidance("Set starting run number for LCIO event.");
	m_setRunNumberCmd->SetParameterName("RunNumber", false);

	// enable autonaming
	m_autonameCmd = new G4UIcommand("/lcio/autoname", this);
	m_autonameCmd->SetGuidance("Set autonaming parameters.");
	m_autonameCmd->SetGuidance(
			"Valid autoname fields: application geometry event eventNumber run binary physics date geant4");
	p = new G4UIparameter("autoname", 's', true);
	m_autonameCmd->SetParameter(p);

	/* dumping of event data */
	m_dumpEventCmd = new G4UIcmdWithABool("/lcio/dumpEvent", this);
	m_dumpEventCmd->SetGuidance("Dump information about collections in the event");
	m_dumpEventCmd->SetParameterName("enable", true);
	m_dumpEventCmd->SetDefaultValue(true);
}
}
