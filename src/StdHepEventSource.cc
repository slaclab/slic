// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/StdHepEventSource.cc,v 1.10 2013-11-06 00:23:37 jeremy Exp $
#include "StdHepEventSource.hh"

// slic
#include "LcioManager.hh"
#include "LcioMcpManager.hh"

namespace slic {
StdHepEventSource::StdHepEventSource(const std::string& fname) :
		EventSourceWithInputFile("StdHepEventSource", fname), m_convertor(0) {
	// create loader with internal reader
	m_loader = new StdHepLoader();

	// create new convertor
	m_convertor = new StdHepToLcioConvertor();
}

void StdHepEventSource::generate(G4Event* anEvent) {
	LcioManager::instance()->getGenerator()->generatePrimaryVertexFromMcpCollection(
			LcioMcpManager::instance()->getInitialMcpCollection(), anEvent);
}

// open the current file
void StdHepEventSource::open() {
	m_loader->openStdHepFile(m_filename);
	m_fileIsOpen = true;
}

// close the current file
void StdHepEventSource::close() {
	m_loader->closeStdHepFile();
	m_fileIsOpen = false;
}

// read the next event
void StdHepEventSource::readNextEvent() {
	m_loader->readNextEvent();

	if (m_loader->isEndOfInput()) {
		m_eof = true;
	}
}

void StdHepEventSource::dumpCurrentEvent() {
	log() << LOG::okay << "Dumping StdHep event info ..." << LOG::done;

	lStdHep* rdr = m_loader->getStdHepReader();

	if (rdr) {
		rdr->printEventTable();
	} else {
		log() << LOG::error << "No current StdHep reader." << LOG::done;
	}

	log() << LOG::okay << LOG::endl;
}

void StdHepEventSource::beginRun(const G4Run* aRun) {
	// do superclass setup
	EventSourceWithInputFile::beginRun(aRun);

	// setup convertor
	assert( m_loader->getStdHepReader());
	m_convertor->setStdHepReader(m_loader->getStdHepReader());
}

void StdHepEventSource::beginEvent(const G4Event* anEvent) {
	// read an event
	EventSourceWithInputFile::beginEvent(anEvent);

	// not EOF?
	if (!isEOF()) {

		// set convertor's coll
		m_convertor->setParticleCollection(LcioMcpManager::instance()->getInitialMcpCollection());

		// run convertor
		m_convertor->fillParticleCollection();
	}
}

lStdHep* StdHepEventSource::getStdHepReader() {
	return m_loader->getStdHepReader();
}
}
