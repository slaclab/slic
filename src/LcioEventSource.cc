#include "LcioEventSource.hh"

// SLIC
#include "MCParticleManager.hh"

// LCIO
#include "UTIL/LCTOOLS.h"

namespace slic {

LcioEventSource::LcioEventSource(const std::string& fname) :
        EventSourceWithInputFile("LcioEventSource", fname), m_eventGenerator(0) {
}

LcioEventSource::~LcioEventSource() {
    delete m_eventGenerator;
}

// open the current file
void LcioEventSource::open() {
    m_eventGenerator = new MCParticleGenerator(m_filename);
    m_fileIsOpen = true;
}

// close the current file
void LcioEventSource::close() {
    delete m_eventGenerator;
    m_eventGenerator = 0;
    m_fileIsOpen = false;
}

// read the next event
void LcioEventSource::readNextEvent() {
    m_eventGenerator->readNextEvent();
    if (m_eventGenerator->getCurrentEvent() == 0) {
        m_eof = true;
    }
}

void LcioEventSource::printCurrentEvent() {
    if (MCParticleManager::instance()->getMCParticleCollection() != NULL)
        UTIL::LCTOOLS::printMCParticles(MCParticleManager::instance()->getMCParticleCollection());
}

void LcioEventSource::generate(G4Event* anEvent) {
    m_eventGenerator->generateEvent(anEvent);
}

void LcioEventSource::beginRun(const G4Run* aRun) {
    // do superclass action
    EventSourceWithInputFile::beginRun(aRun);
}

void LcioEventSource::beginEvent(const G4Event* anEvent) {
    EventSourceWithInputFile::beginEvent(anEvent);
}

}
