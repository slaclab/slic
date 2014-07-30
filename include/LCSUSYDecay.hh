#ifndef SLIC_LCSUSYDECAY_HH
#define SLIC_LCSUSYDECAY_HH 1

// Geant4
#include "G4UnknownDecay.hh"

namespace slic {

/**
 * @class LCSUSYDecay
 * @brief This class triggers decays for SUSY particles.
 */
class LCSUSYDecay: public G4UnknownDecay {
public:

    /**
     * Class constructor.
     */
	LCSUSYDecay() :
			G4UnknownDecay("LCSUSYDecay") {
		;
	}

	/**
	 * Make this class applicable to SUSY particles.
	 * @return True if process is applicable; false if not.
	 */
	G4bool IsApplicable(const G4ParticleDefinition &pdef) {
		return (pdef.GetParticleType() == "susy");
	}
};
}

#endif
