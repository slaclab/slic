#include "LHEEventSource.hh"

#include "G4Event.hh"

namespace slic {

LHEEventSource::LHEEventSource(const std::string& fname) : EventSourceWithInputFile(fname) {
}

LHEEventSource::~LHEEventSource() {
    delete m_eventGenerator;
}

void LHEEventSource::generate(G4Event* anEvent) {
    m_eventGenerator->GeneratePrimaryVertex(anEvent);
}

void LHEEventSource::open() {
    m_reader = new LHEReader(getFilename());
    m_eventGenerator = new LHEPrimaryGenerator(m_reader);
    m_fileIsOpen = true;
}

void LHEEventSource::close() {
    delete m_eventGenerator;
    m_fileIsOpen = false;
}

void LHEEventSource::readNextEvent() {
    // The LHEEventGenerator will read the next event.
}

void LHEEventSource::printCurrentEvent() {
}

void LHEEventSource::beginRun(const G4Run* aRun) {
    EventSourceWithInputFile::beginRun(aRun);
}

void LHEEventSource::beginEvent(const G4Event* anEvent) {
    EventSourceWithInputFile::beginEvent(anEvent);
}

}
