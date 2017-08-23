#include "EventSource.hh"

// Geant4
#include "G4Run.hh"
#include "G4Event.hh"

namespace slic {

EventSource::~EventSource() {
}

EventSource::EventSource(const std::string& name) :
        Module(name), m_name(name), nEventsGenerated(0) {
}

void EventSource::reset() {
    // no-op -- implement if generator can be reset to event 0 or new file/source
}

unsigned int EventSource::skipEvents(unsigned int) {
    // no-op -- implement if generator can "skip" an event
    return 0;
}

void EventSource::beginEvent(const G4Event*) {
    // no-op -- implement if generator has an action at beginning of event
}

void EventSource::endEvent(const G4Event*) {
    // no-op -- implement if generator has an action at end of event
}

void EventSource::beginRun(const G4Run*) {
    // no-op -- implement if generator has an action at beginning of run
}

void EventSource::endRun(const G4Run*) {
    // no-op -- implement if generator has an action at end of run
}
}
