/**
 * @file LHEPrimaryGenerator.h
 * @brief Class for generating a Geant4 event from LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef SLIC_LHEPRIMARYGENERATOR_HH_
#define SLIC_LHEPRIMARYGENERATOR_HH_

// Geant4
#include "G4VPrimaryGenerator.hh"

// LDMX
#include "LHEReader.hh"

namespace slic {

/**
 * @class LHEPrimaryGenerator
 * @brief Generates a Geant4 event from an LHEEvent
 */
class LHEPrimaryGenerator: public G4VPrimaryGenerator {

    public:

        /**
         * Class constructor.
         * @param reader The LHE reader with the event data.
         */
        LHEPrimaryGenerator(LHEReader* reader);

        /**
         * Class destructor.
         */
        virtual ~LHEPrimaryGenerator();

        /**
         * Generate vertices in the Geant4 event.
         * @param anEvent The Geant4 event.
         */
        void GeneratePrimaryVertex(G4Event* anEvent);

    private:

        /**
         * The LHE reader with the event data.
         */
        LHEReader* reader_;
};

}

#endif
