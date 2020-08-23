#include "StdHepGenerator.hh"

// SLIC
#include "MCParticleManager.hh"

// LCIO
#include "IMPL/MCParticleImpl.h"

// Geant4
#include "G4SystemOfUnits.hh"

using EVENT::MCParticle;
using EVENT::LCCollection;
using IMPL::MCParticleImpl;

namespace slic {

StdHepGenerator::StdHepGenerator(G4String eventFile) :
        _particles(0) {
    _reader = new LCStdHepRdrNew(eventFile.data());
}

StdHepGenerator::~StdHepGenerator() {
    delete _reader;
}

void StdHepGenerator::readNextEvent() {
    /* Read in the next StdHep event which is converted to an MCParticle collection. */
    _particles = _reader->readEvent();
}

void StdHepGenerator::generateEvent(G4Event* event) {
    /*
     * Use the MCParticleManager utility method to create the Geant4 event using the current
     * collection of MCParticles.
     */
    MCParticleManager::instance()->generateEvent(_particles, event);
}

LCCollectionVec* StdHepGenerator::getCurrentParticleCollection() {
    return _particles;
}

} /* namespace slic */
