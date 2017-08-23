#include "MCParticleGenerator.hh"

// SLIC
#include "MCParticleManager.hh"

// LCIO
#include "IOIMPL/LCFactory.h"
#include "IMPL/MCParticleImpl.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCIO.h"

// Geant4
#include "G4SystemOfUnits.hh"

using EVENT::MCParticle;
using EVENT::LCCollection;
using EVENT::LCEvent;
using IMPL::MCParticleImpl;
using IOIMPL::LCFactory;

namespace slic {

MCParticleGenerator::MCParticleGenerator(G4String eventFile) :
        _lcevent(0) {

    _particleManager = MCParticleManager::instance();

    /**
     * Setup the reader.
     */
    LCFactory* factory = LCFactory::getInstance();
    _reader = factory->createLCReader();
    _reader->open(eventFile);
}

MCParticleGenerator::~MCParticleGenerator() {
    _reader->close();
}

void MCParticleGenerator::readNextEvent() {
    _lcevent = _reader->readNextEvent(EVENT::LCIO::UPDATE);
}

LCEvent* MCParticleGenerator::getCurrentEvent() {
    return _lcevent;
}

void MCParticleGenerator::generateEvent(G4Event* event) {

    /* Get collection of MCParticles. */
    LCCollectionVec* particles = (LCCollectionVec*) _lcevent->takeCollection("MCParticle");

    /* Generate the Geant4 event using the MCParticleManager. */
    _particleManager->generateEvent(particles, event);
}

} /* namespace slic */
