// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioPrimaryGenerator.hh,v 1.18 2013-11-06 00:23:35 jeremy Exp $
#ifndef SLIC_LCIOPRIMARYGENERATOR_HH
#define SLIC_LCIOPRIMARYGENERATOR_HH

// slic
#include "Module.hh"

// lcio
#include "EVENT/LCCollection.h"
#include "IMPL/MCParticleImpl.h"

// geant4
#include "G4VPrimaryGenerator.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"

namespace slic {

class LcioManager;
class LcioMcpManager;

/**
 @class LcioPrimaryGenerator
 @brief Generates G4PrimaryParticles from an LCIO MCParticle tree.
 */
class LcioPrimaryGenerator: public Module {

public:
    typedef std::map<EVENT::MCParticle*, G4PrimaryParticle*> ParticleToPrimaryMap;

public:

    /**
     * Class constructor.
     * @param[in] manager The LcioManager.
     */
    LcioPrimaryGenerator(LcioManager* manager);

    /**
     * Class destructor.
     */
    virtual ~LcioPrimaryGenerator();

public:

    /**
     * Create the G4Event structure from a collection of MCParticles.
     * @param[in] particles The list of MCParticles.
     * @param[in,out] anEvent The G4Event.
     */
    void generatePrimaryVertexFromMcpCollection(EVENT::LCCollection* particles, G4Event* anEvent);

private:

    /**
     * Create the G4PrimaryParticle based on an input MCParticle.
     * @param[in] particle The input MCParticle.
     * @return The G4PrimaryParticle created from the MCParticle.
     */
    G4PrimaryParticle* createPrimaryParticle(IMPL::MCParticleImpl* particle);

    /**
     * Create a G4PrimaryVertex based on an itpu MCParticle.
     * @param[in] particle The MCParticle.
     * @return The G4PrimaryVertex created from the MCParticle.
     */
    G4PrimaryVertex* createPrimaryVertex(IMPL::MCParticleImpl* particle);

private:
    LcioManager* m_mgr;
    LcioMcpManager* m_mcpManager;
};
}

#endif
