#include "MCParticleManager.hh"

// SLIC
#include "EventSourceManager.hh"

// Geant4
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace slic {

void MCParticleManager::generateEvent(LCCollectionVec* particles, G4Event* event) {

    /* Apply Z smearing to input particles. */
    G4double z = smearZPosition(EventSourceManager::instance()->getZSmearing());

    /* Apply the Lorentz Transformation to input particles. */
    applyLorentzTransformation(particles, EventSourceManager::instance()->getLorentzTransformationAngle());

    /*
     * Process particles and add them to the primary vertex of the event.
     */
    G4ThreeVector particlePosition(0, 0, z);
    G4double particleTime;
    G4PrimaryVertex* vertex = new G4PrimaryVertex(particlePosition, particleTime);
    for(size_t i=0; i < particles->size(); i++) {
        MCParticle* mcp = (MCParticle*)(*particles)[i];
        if (mcp->getParents().size()==0) {
            //std::cout << "construct tree of relevant G4PrimaryParticles for initial MCParticle " << i << " " << std::flush;
            std::set<G4PrimaryParticle*> primaries = getRelevantParticles(mcp);
            //std::cout << "... finished" << std::endl;
            std::set<G4PrimaryParticle*>::iterator primaryIt;
            for (primaryIt = primaries.begin(); primaryIt != primaries.end(); primaryIt++) {
                vertex->SetPrimary(*primaryIt);
                //std::cout << "G4PrimaryParticle ("<< (*primaryIt)->GetPDGcode() << ") added to G4PrimaryVertex." << std::endl;
            }
        }
    }

    /* Add the primary vertex to Geant4. */
    event->AddPrimaryVertex(vertex);

    /* Register the collection of MCParticles that was created. */
    setMCParticleCollection(particles);
}


std::set<G4PrimaryParticle*> MCParticleManager::getRelevantParticles(MCParticle* p) {

    //log each particle which has been called, to avoid double counting and increase efficiency
    _visitedParticles.insert(p);
    PrimaryParticleMap::iterator mcpIT;
    std::set<G4PrimaryParticle*> relevantParticlesSet; //holds all relevant decay particles of p

    /*
     * Case #1:
     *
     * If p is a stable particle: search for it in LCIO2Geant4Map
     *
     * If exists in map: get the G4PrimaryParticle version of p from LCIO2Geant4Map
     * Else: create G4PrimaryParticle version write it to LCIO2Geant4Map
     *
     * Finally: insert the G4PrimaryParticle verion of p into the relevant particle list
     * and return the list.
     */
    if (p->getGeneratorStatus() == 1) {
        G4PrimaryParticle* g4p;
        mcpIT = _primaryMap.find(p);
        if (mcpIT != _primaryMap.end()) {
            g4p = mcpIT->second;
        } else {
            G4int IDHEP = p->getPDG();
            G4double PHEP1 = p->getMomentum()[0];
            G4double PHEP2 = p->getMomentum()[1];
            G4double PHEP3 = p->getMomentum()[2];
            G4double PHEP5 = p->getMass();
            // create G4PrimaryParticle object
            g4p = new G4PrimaryParticle(IDHEP, PHEP1 * GeV, PHEP2 * GeV, PHEP3 * GeV);
            g4p->SetMass(PHEP5 * GeV);
            _primaryMap[p] = g4p;
            //std::cout << "*" << std::flush;
        }
        //std::cout << g4p->GetPDGcode() << std::flush;
        relevantParticlesSet.insert(g4p);
        return relevantParticlesSet;
    }

    /*
     * CASE #2:
     *
     * If p is not stable: get first decay daughter and calculate the proper time of p.
     *
     * If proper time is not zero: particle is "relevant", since it carries vertex information.
     *
     * If p is already in LCIO2Geant4Map: take it.
     * Else: create G4PrimaryParticle version of p and write it to LCIO2Geant4Map
     *
     * Collect all relevant particles of all daughters and setup "relevant mother-
     * daughter-relations" between relevant decay particles and G4PrimaryParticle version of p
     *
     * Finally: insert only the G4PrimaryParticle version of p to the relevant particle list and return this "list".
     * The added particle has now the correct pre-assigned decay products and (hopefully) the right lifetime.
     */
    else if (p->getDaughters().size() > 0) {  //fg: require that there is at least one daughter - otherwise forget the particle
        // calculate proper time
        MCParticle* dp = p->getDaughters()[0];

        double proper_time = fabs((dp->getTime() - p->getTime()) * p->getMass()) / p->getEnergy();
        //  fix by S.Morozov for real != 0
        double aPrecision = dp->getTime() * p->getMass() / p->getEnergy();
        double bPrecision = p->getTime() * p->getMass() / p->getEnergy();

        double proper_time_Precision = pow(10, -DBL_DIG)*fmax(fabs(aPrecision),fabs(bPrecision));

        bool isProperTimeZero = (proper_time <= proper_time_Precision);

        // -- remove original --- if (proper_time != 0) {
        if (isProperTimeZero == false) {

            G4PrimaryParticle* g4p;
            mcpIT = _primaryMap.find(p);
            if (mcpIT != _primaryMap.end()) {
                g4p = mcpIT->second;
            } else {
                G4int IDHEP = p->getPDG();
                G4double PHEP1 = p->getMomentum()[0];
                G4double PHEP2 = p->getMomentum()[1];
                G4double PHEP3 = p->getMomentum()[2];
                G4double PHEP5 = p->getMass();
                // create G4PrimaryParticle object
                g4p = new G4PrimaryParticle(IDHEP, PHEP1 * GeV, PHEP2 * GeV, PHEP3 * GeV);
                g4p->SetMass(PHEP5 * GeV);
                g4p->SetProperTime(proper_time * ns);
                _primaryMap[p] = g4p;
                std::set<G4PrimaryParticle*> vec3;
                for (size_t i = 0; i < p->getDaughters().size(); i++) {
                    if (_visitedParticles.count(p->getDaughters()[i]) == 0) {
                        std::set<G4PrimaryParticle*> vec2 = getRelevantParticles(p->getDaughters()[i]);
                        std::set<G4PrimaryParticle*>::iterator setit;
                        for (setit = vec2.begin(); setit != vec2.end(); setit++) {
                            vec3.insert(*setit);
                        }
                    }
                }
                std::set<G4PrimaryParticle*>::iterator setit;
                for (setit = vec3.begin(); setit != vec3.end(); setit++) {
                    g4p->SetDaughter(*setit);
                }
                //std::cout << "*" << std::flush;
            }
            //std::cout << g4p->GetPDGcode() << std::flush;
            relevantParticlesSet.insert(g4p);
            return relevantParticlesSet;
        }
        /* CASE #3:
         *
         * If p is not stable AND has zero lifetime: forget about p and retrieve all relevant
         * decay particles of all daughters of p.
         *
         * In this case this step of recursion is just there for collecting all relevant decay
         * products of daughters and returning them.
         */
        else {
            for (size_t i = 0; i < p->getDaughters().size(); i++) {
                if (_visitedParticles.count(p->getDaughters()[i]) == 0) {
                    std::set<G4PrimaryParticle*> vec2 = getRelevantParticles(p->getDaughters()[i]);
                    std::set<G4PrimaryParticle*>::iterator setit;
                    for (setit = vec2.begin(); setit != vec2.end(); setit++) {
                        relevantParticlesSet.insert(*setit);
                    }
                }
            }
            return relevantParticlesSet;
        }
    }
    return relevantParticlesSet;
}

void MCParticleManager::applyLorentzTransformation(LCCollection* particles, const G4double alpha) {

    if (alpha == 0)
        return; // nothing to do

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
        G4cout << "Smeared Z position: " << z << G4endl;
    }
    return z;
}

};
