// SLIC
#include "LcioFileNamer.hh"
#include "EventSourceManager.hh"
#include "FileUtil.hh"
#include "SlicApplication.hh"
#include "PackageInfo.hh"
#include "RunManager.hh"
#include "TimeUtil.hh"
#include "PhysicsListManager.hh"
#include "Geant4VersionInfo.hh"

// LCDD
#include "lcdd/core/LCDDProcessor.hh"
#include "lcdd/util/StringUtil.hh"

// Geant4
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

// stl
#include <sstream>

using namespace std;

namespace slic {

std::string LcioFileNamer::m_sep = "_";

LcioFileNamer::LcioFileNamer() :
		Module("LcioFileNamer") {
	m_defaultFields.push_back("event");
	m_defaultFields.push_back("application");
	m_defaultFields.push_back("geant4");
	m_defaultFields.push_back("physics");
	m_defaultFields.push_back("geometry");
}

LcioFileNamer::~LcioFileNamer() {
}

std::string LcioFileNamer::getFieldValue(std::string field) {
	std::string value;

	static std::string sep("-");

	if (field == "application" || field == "app") {
		value = PackageInfo::getShortName() + "-v" + PackageInfo::getVersion(sep);
	} else if (field == "geometry" || field == "geo") {
		value = LCDDProcessor::instance()->getDetectorName();
	} else if (field == "date") {
		value = TimeUtil::getDate();
	} else if (field == "event" || field == "evt") {
		value = LcioFileNamer::makeEventName();
	} else if (field == "eventNumber" || field == "evtNum") {
		value = LcioFileNamer::makeEventNumberString();
	} else if (field == "run" || field == "runNumber" || field == "runNum") {
		value = LcioFileNamer::makeRunNumberString();
	} else if (field == "binary") {
		value = SlicApplication::instance()->getBinaryBasename();
	} else if (field == "physics") {
		value = PhysicsListManager::instance()->getCurrentListName();
	} else if (field == "geant4") {
		value = "Geant4-v" + Geant4VersionInfo::getVersion(sep);
	} else {
#ifdef SLIC_LOG      
		log() << LOG::error << "Ignoring unknown autoname field <" << field << ">." << LOG::done;
#endif
		value = "";
	}

	return value;
}

std::string LcioFileNamer::makeFileName(std::vector<std::string> fieldList) {
	std::string filename = "";
	if (fieldList.size() != 0) {
		for (std::vector<std::string>::const_iterator it = fieldList.begin(); it != fieldList.end(); it++) {
			std::string value = getFieldValue(*it);
			if (value != "") {
				filename = filename + value + m_sep;
			}
		}
		if (filename != "") {
			filename.erase(filename.end() - 1, filename.end());
		}
	} else {
		filename = makeDefaultFileName();
	}
	return filename;
}

std::string LcioFileNamer::makeDefaultFileName() {
	return makeFileName(m_defaultFields);
}

std::string LcioFileNamer::makeEventName() {
	std::string evt;

	EventSourceManager::ESourceType est = EventSourceManager::instance()->getCurrentSourceType();

	if (EventSourceManager::instance()->isFileSource()) {
		evt = makeFileBasedName();
	} else if (est == EventSourceManager::eGPS) {
		evt = makeGPSName();
	} else if (est == EventSourceManager::eParticleGun) {
		evt = makeGunName();
	} else {
		evt = "events";
	}
	return evt;
}

std::string LcioFileNamer::makeFileBasedName() {
	std::string fname = EventSourceManager::instance()->getFilename();

	// basename and remove file extension
	fname = FileUtil::removeExtension(FileUtil::basename(fname));

	return fname;
}

std::string LcioFileNamer::makeGPSName() {
	G4GeneralParticleSource* gps = EventSourceManager::instance()->getGPS();

	// particle name
	std::string pname = gps->GetParticleDefinition()->GetParticleName();

	// energy
	double ene = gps->GetCurrentSource()->GetEneDist()->GenerateOne(gps->GetParticleDefinition());
	std::string eneStr = StringUtil::toString(ene / GeV) + "GeV";

	// full string
	std::string evtname = pname + m_sep + eneStr;

	return evtname;
}

std::string LcioFileNamer::makeGunName() {
	G4ParticleGun* gun = EventSourceManager::instance()->getParticleGun();

	// particle name
	std::string pname = gun->GetParticleDefinition()->GetParticleName();

	// energy
	std::string pE = StringUtil::toString(gun->GetParticleEnergy() / GeV) + "GeV";

	// full string
	std::string evtname = pname + m_sep + pE;

	return evtname;
}

std::string LcioFileNamer::makeRunNumberString() {
	return StringUtil::toString(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
}

std::string LcioFileNamer::makeEventNumberString() {
	return StringUtil::toString(SlicApplication::instance()->getRunManager()->getNumberOfEventsToRun());
}
}
