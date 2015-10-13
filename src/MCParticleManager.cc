#include "MCParticleManager.hh"

// SLIC
#include "EventSourceManager.hh"

// Geant4
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "Randomize.hh"

namespace slic {

void MCParticleManager::generateEvent(LCCollectionVec* particles, G4Event* event) {

#if SLIC_LOG
    log() << LOG::debug << LOG::head << "generating event " << event->GetEventID() << " from " << particles->size() << " particles" << LOG::endl << LOG::done;
#endif

    // Apply Z smearing to input particles.
#if SLIC_LOG
    log() << LOG::debug << "applying Z smearing: " << EventSourceManager::instance()->getZSmearing() << LOG::done;
#endif
    G4double z = smearZPosition(EventSourceManager::instance()->getZSmearing());

    /// Apply the Lorentz Transformation to input particles.
#if SLIC_LOG
    log() << LOG::debug << "applying Lorentz transform: " << EventSourceManager::instance()->getLorentzTransformationAngle() << LOG::done;
#endif
    applyLorentzTransformation(particles, EventSourceManager::instance()->getLorentzTransformationAngle());

    log() << LOG::endl;

    // Loop over MCParticle input list.
    for (size_t i = 0; i < particles->size(); i++) {

        // Get the next MCParticle.
        MCParticle* mcp = (MCParticle*) (*particles)[i];

        // Debug print information about the particle.
#ifdef SLIC_LOG
        log() <<  LOG::debug << "processing particle " << i << LOG::endl
                << "  PDG: " << mcp->getPDG() << LOG::endl
                << "  gen status: " << mcp->getGeneratorStatus() << LOG::endl
                << "  sim status: " << mcp->getSimulatorStatus() << LOG::endl
                << "  energy: " << mcp->getEnergy() << LOG::endl
                << "  time: " << mcp->getTime() << LOG::endl
                << "  daughters: " << mcp->getDaughters().size() << LOG::endl
                << "  vertex: ( " << mcp->getVertex()[0] << " " << mcp->getVertex()[1] << " " << mcp->getVertex()[2] << " )" << LOG::endl
                << "  p: ( " << mcp->getMomentum()[0] << ", " << mcp->getMomentum()[1] << ", " << mcp->getMomentum()[2] << " )" << LOG::endl
                << "  charge: " << mcp->getCharge() << LOG::endl
                << "  mass: " << mcp->getMass() << LOG::endl
                << "  daughters: " << mcp->getDaughters().size() << LOG::endl
                << "  parents: " << mcp->getParents().size() << LOG::endl
                << LOG::done;
#endif

        // Only process particles without parents; daughters will be handled recursively.
        if (mcp->getParents().size() == 0) {

#ifdef SLIC_LOG
            log() << LOG::debug << "creating new vertex for MCParticle " << i << LOG::done;
#endif

            // Create a new primary vertex.
            G4PrimaryVertex* vertex = createVertex(mcp);

            if (vertex == 0) {
#ifdef SLIC_LOG
                log() << LOG::debug << "no vertex created for particle " << i << LOG::done;
#endif
            }

#ifdef SLIC_LOG
            log() << LOG::debug << "creating primary for MCParticle " << i << LOG::done;
#endif

            // Create a primary particle and recursively create primaries for its daughters.
            const std::set<G4PrimaryParticle*> primaries = createPrimary(0, mcp);

            // Were any primaries created?
            if (primaries.size() > 0) {

                // Loop over returned set of primary particles.
                for (std::set<G4PrimaryParticle*>::const_iterator it = primaries.begin(); it != primaries.end(); it++) {

                    // Is this a top-level particle with no daughters?
                    if (!MCParticleManager::isDaughter((*it), primaries)) {

                        // Create a new vertex if necessary.
                        if (vertex == 0) {
#ifdef SLIC_LOG
                            log() << "creating vertex from primary " << *it << LOG::done;
#endif
                            // Create vertex by looking up MCParticle from the primary.
                            vertex = createVertex(_mcpMap[(*it)]);
                        }

                        // Add the primary particle to the vertex.
#ifdef SLIC_LOG
                        log() << "adding primary daughter particle " << *it << " to vertex" << LOG::done;
#endif
                        vertex->SetPrimary(*it);
                    }
                }

                // Add the vertex to the event.
                event->AddPrimaryVertex(vertex);

            } else {
#ifdef SLIC_LOG
                log() << LOG::warning << "skipping particle " << i << " with no primaries" << LOG::done;
#endif
                // No primaries created so this record and its daughters are skipped!
                if (vertex != 0) {
#ifdef SLIC_LOG
                    log() << LOG::debug << "deleting empty vertex for particle " << i << LOG::done;
#endif
                    delete vertex;
                }
            }
        }
#ifdef SLIC_LOG
        log() << LOG::debug << LOG::endl << "-------------------------------------------------" << LOG::endl << LOG::done;
#endif
    }

    // Register the collection of MCParticles that was created.
    setMCParticleCollection(particles);
}

std::set<G4PrimaryParticle*> MCParticleManager::createPrimary(G4PrimaryParticle* parent, MCParticle* mcp) {

    // The MCParticle argument should not be null.
    if (mcp == 0) {
        G4Exception("", "", FatalException, "MCParticle is null.");
    }

    // Set of primaries that will be returned.
    std::set<G4PrimaryParticle*> primaries;

    G4PrimaryParticle* primary = 0;

    // Is this primary not created yet and does it have a valid generator status?
    if (_primaryMap.count(mcp) == 0 && (mcp->getGeneratorStatus() == 1 || mcp->getGeneratorStatus() == 2)) {

        // Create the primary particle from the MCParticle's information.
        primary = new G4PrimaryParticle();
        primary->SetPDGcode(mcp->getPDG());
        primary->SetCharge(mcp->getCharge());
        primary->SetMass(mcp->getMass() * GeV);
        const double* p = mcp->getMomentum();
        primary->SetMomentum(p[0] * GeV, p[1] * GeV, p[2] * GeV);

        // Set the proper time by subtracting from daughter's time.
        if (mcp->getDaughters().size() > 0) {
            MCParticle* dp = mcp->getDaughters()[0];
            double properTime = fabs((dp->getTime() - mcp->getTime()) * mcp->getMass()) / mcp->getEnergy();
#ifdef SLIC_LOG
            log() << LOG::debug << "proper time set to " << properTime << LOG::done;
#endif
            primary->SetProperTime(mcp->getTime() * ns);
        }

        // Map primary to MCParticle.
#ifdef SLIC_LOG
            log() << LOG::debug << "mapping particle " << mcp << " to primary " << primary << LOG::done;
#endif
        _primaryMap[mcp] = primary;

        // Map MCParticle to primary.
        _mcpMap[primary] = mcp;

        if (parent != 0) {
#ifdef SLIC_LOG
            log() << LOG::debug << "setting parent primary on daughter" << LOG::done;
#endif
            parent->SetDaughter(primary);
        }

        primaries.insert(primary);
    }

    for (int i = 0; i < mcp->getDaughters().size(); i++) {
#ifdef SLIC_LOG
        log() << LOG::debug << "creating primary for daughter " << i << LOG::done;
#endif
        std::set<G4PrimaryParticle*> daughters = createPrimary(primary, mcp->getDaughters()[i]);
#ifdef SLIC_LOG
        log() << LOG::debug << "created " << daughters.size() << " daughter primaries" << LOG::done;
#endif

        primaries.insert(daughters.begin(), daughters.end());
    }
#ifdef SLIC_LOG
        log().getOutputStream().flush();
#endif
    return primaries;
}

G4PrimaryVertex* MCParticleManager::createVertex(MCParticle* mcp) {

    G4PrimaryVertex* vertex = 0;

    if (mcp->getGeneratorStatus() == 1 || mcp->getGeneratorStatus() == 2) {

        // Get the particle's origin.
        const double* particleVertex = mcp->getVertex();
        G4ThreeVector particlePosition(particleVertex[0], particleVertex[1], particleVertex[2]);

        // Get the particle's time.
        G4double particleTime = mcp->getTime();

        // Create a new vertex for this particle.
#ifdef SLIC_LOG
        log() << LOG::debug << "creating vertex at position " << particlePosition << " with time " << particleTime << LOG::done;
#endif
        vertex = new G4PrimaryVertex(particlePosition, particleTime);
    }

    return vertex;
}

void MCParticleManager::applyLorentzTransformation(LCCollection* particles, const G4double alpha) {

    if (alpha == 0) {
        return; // nothing to do
    }

    //G4cout << "Applying Lorentz Transformation angle: " << alpha << G4endl;

    // parameters of the Lorentz transformation matrix
    const G4double gamma = sqrt(1 + sqr(tan(alpha)));
    const G4double betagamma = tan(alpha);

    if (particles != 0) {

        int nMCP = particles->getNumberOfElements();

        for (int i = 0; i < nMCP; ++i) {

            IMPL::MCParticleImpl* mcp = dynamic_cast<IMPL::MCParticleImpl*>(particles->getElementAt(i));

            const double* p = mcp->getMomentum();

            // before the transformation
            double pPrime[3];

            const G4double m = mcp->getMass();

            //G4cout << "pre pX: " << p[0] << G4endl;

            // after the transformation (boost in x-direction)
            pPrime[0] = betagamma * sqrt(sqr(p[0]) + sqr(p[1]) + sqr(p[2]) + sqr(m)) + gamma * p[0];
            pPrime[1] = p[1];
            pPrime[2] = p[2];

            //G4cout << "transformed pX: " << pPrime[0] << G4endl;

            // py and pz remain the same, E changes implicitly with px
            mcp->setMomentum(pPrime);
        }
    }
}

G4double MCParticleManager::smearZPosition(const G4double rms) {
    G4double z = 0;
    if (rms != 0) {
        // Generate smeared Z position.
        z = G4RandGauss::shoot(0, rms/mm);
        //G4cout << "Smeared Z position: " << z << G4endl;
    }
    return z;
}

bool MCParticleManager::isDaughter(G4PrimaryParticle* particle, const std::set<G4PrimaryParticle*>& primaries) {
    for (std::set<G4PrimaryParticle*>::iterator it = primaries.begin(); it != primaries.end(); it++) {
        G4PrimaryParticle* dau = (*it)->GetDaughter();
        while (dau != 0) {
            if (dau == particle) {
                return true;
            }
            dau = dau->GetNext();
        }
    }
    return false;
}

}
;
