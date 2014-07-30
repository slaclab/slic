#ifndef SLIC_LCEXTENDEDDECAY_HH_
#define SLIC_LCEXTENDEDDECAY_HH_ 1

#include "G4UnknownDecay.hh"

namespace slic {

/**
 * @class LCExtendedDecay
 * @brief This class triggers decays for the extended particle set from HepPDT.
 */
class LCExtendedDecay: public G4UnknownDecay {
public:

    /**
     * Class constructor.
     */
    LCExtendedDecay() :
            G4UnknownDecay("LCExtendedDecay") {
        ;
    }

    /**
     * Determine if the extended decay process is applicable to the given particle.
     * @param[in] pdef The particle definition.
     */
    G4bool IsApplicable(const G4ParticleDefinition &pdef) {
        return (pdef.GetParticleType() == "extended");
    }
};
}

#endif
