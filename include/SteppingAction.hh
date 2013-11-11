// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/SteppingAction.hh,v 1.17 2013-11-06 00:23:36 jeremy Exp $

#ifndef SLIC_STEPPINGACTION_HH
#define SLIC_STEPPINGACTION_HH 1

// SLIC
#include "Module.hh"

// Geant4
#include "G4UserSteppingAction.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"

class UserRegionInformation;

namespace slic {

/**
 * @class SteppingAction
 * @brief Implementation of G4UserSteppingAction.
 */
class SteppingAction: public G4UserSteppingAction, Module {

public:

    /**
     * Class constructor.
     */
	SteppingAction();

	/**
	 * Class destructor.
	 */
	virtual ~SteppingAction();

	/**
	 * Implementation of user stepping action.
	 * @param[in] step The G4Step.
	 */
	void UserSteppingAction(const G4Step* step);

private:

	/**
	 * Kill a G4Track if it is inside a track-killing region.
	 * @param[in] step The G4Step.
	 */
	void checkKillTrack(const G4Step* step);
};
}

#endif
