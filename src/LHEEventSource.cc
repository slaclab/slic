#include "LHEEventSource.hh"

#include "G4Event.hh"

namespace slic {

LHEEventSource::LHEEventSource(const std::string& fname) : EventSourceWithInputFile(fname) {
}

LHEEventSource::~LHEEventSource() {
    delete m_eventGenerator;
}

void LHEEventSource::generate(G4Event* anEvent) {
    std::cout << "LHEEventSource: generate event - " << anEvent->GetEventID() << std::endl;
    m_eventGenerator->GeneratePrimaryVertex(anEvent);
}

void LHEEventSource::open() {
    std::cout << "LHEEventSource: opening " << getFilename() << std::endl;
    m_reader = new LHEReader(getFilename());
    m_eventGenerator = new LHEPrimaryGenerator(m_reader);
    m_fileIsOpen = true;
}

void LHEEventSource::close() {
    std::cout << "LHEEventSource: closing " << getFilename() << std::endl;
    delete m_eventGenerator;
    m_fileIsOpen = false;
}

void LHEEventSource::readNextEvent() {
    // Don't do anything here.  The LHEEventGenerator calls this method instead.
    std::cout << "LHEEventSource: read next event (no-op)" << std::endl;
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
