// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/StdHepLoader.cc,v 1.23 2013-06-26 01:57:02 jeremy Exp $

#include "StdHepLoader.hh"

// lcdd
#include "lcdd/util/StringUtil.hh"

// geant4
#include "G4PrimaryParticle.hh"
#include "G4Event.hh"

// std
#include <assert.h>

// lcio
#include "UTIL/lStdHep.hh"

using UTIL::lStdHep;

namespace slic {

StdHepLoader::~StdHepLoader() {
	;
}

StdHepLoader::StdHepLoader() :
		Module("StdHepLoader"), m_filename(""), m_reader(0), m_isEndOfInput(false) {
	;
}

void StdHepLoader::openStdHepFile(const std::string& filename) {
	openStdHepFile(filename.c_str());
}

void StdHepLoader::openStdHepFile(const char* filename) {
	// set filename
	m_filename = filename;

	// close if currently open
	closeStdHepFile();

	// new lStdHep interface
	m_reader = new lStdHep(filename);

	// if error, assume fatal
	if (m_reader->getError()) {
		G4Exception("StdHepLoader::openStdHepFile()", "", FatalException, "Error opening input file.");
	}

	// print open message
	log().okay(
			"StdHep file <" + std::string(filename) + "> opened with <"
					+ StringUtil::toString((int) m_reader->numEvents()) + "> events.");
}

void StdHepLoader::closeStdHepFile() {
	if (0 != m_reader) {
		delete m_reader;
		m_reader = 0;
	}
}

void StdHepLoader::readNextEvent() {
	/* Read the next event and check the error code. */
	if (m_reader->readEvent() != LSH_SUCCESS) {
		/* The error code was not EOF.  Assume this means a fatal error occurred. */
		if (m_reader->getError() != LSH_ENDOFFILE) {
			log().fatal("StdHep errorcode <" + StringUtil::toString((int) m_reader->getError()) + ">.");
			G4Exception("", "", FatalException, "lStdHep reported an unrecoverable error.");
		}
		/* EOF */
		else {
			m_isEndOfInput = true;
		}
	}
}

void StdHepLoader::dumpCurrentEvent() {
	if (m_reader) {
		m_reader->printEventHeader();
		m_reader->printEvent();
		m_reader->printEventTable();
	} else {
		log().error("Cannot run StdHepLoader::dumpCurrentEvent.  StdHep reader is not initialized.");
	}
}
}
