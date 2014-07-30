#include "StdHepEventSource.hh"

// SLIC
#include "LcioManager.hh"
#include "MCParticleManager.hh"

// LCIO
#include "UTIL/LCTOOLS.h"

namespace slic {
StdHepEventSource::StdHepEventSource(const std::string& fname) :
		EventSourceWithInputFile("StdHepEventSource", fname),
		m_eventGenerator(0) {
}

void StdHepEventSource::generate(G4Event* anEvent) {
    //std::cout << "StdHepEventSource::generate" << std::endl;

    m_eventGenerator->generateEvent(anEvent);
}

// open the current file
void StdHepEventSource::open() {

    /* Initialize the event generator using the current file name. */
    m_eventGenerator = new StdHepGenerator(getFilename());

	m_fileIsOpen = true;
}

// close the current file
void StdHepEventSource::close() {
    delete m_eventGenerator;
	m_fileIsOpen = false;
}

// read the next event
void StdHepEventSource::readNextEvent() {

	m_eventGenerator->readNextEvent();
	if (m_eventGenerator->getCurrentParticleCollection() == 0) {
	    m_eof = true;
	}
}

void StdHepEventSource::printCurrentEvent() {
    if (MCParticleManager::instance()->getMCParticleCollection() != NULL)
        UTIL::LCTOOLS::printMCParticles(MCParticleManager::instance()->getMCParticleCollection());
}

void StdHepEventSource::beginRun(const G4Run* aRun) {

	// do superclass setup
	EventSourceWithInputFile::beginRun(aRun);
}

void StdHepEventSource::beginEvent(const G4Event* anEvent) {

    //std::cout << "StdHepEventSource::beginEvent" << std::endl;

	// read an event
	EventSourceWithInputFile::beginEvent(anEvent);
}

}
