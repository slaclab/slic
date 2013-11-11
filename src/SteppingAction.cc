// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/SteppingAction.cc,v 1.16 2013-07-01 21:01:31 jeremy Exp $

// SLIC
#include "SteppingAction.hh"
#include "TrajectoryManager.hh"

// LCDD
#include "lcdd/geant4/UserRegionInformation.hh"
#include "lcdd/hits/TrackInformation.hh"

// Geant4
#include "G4UserSteppingAction.hh"
#include "G4Track.hh"

namespace slic {

SteppingAction::SteppingAction() : Module("SteppingAction", false, true) {
}

SteppingAction::~SteppingAction() {
}

void SteppingAction::UserSteppingAction(const G4Step* aStep) {
	/* TrajectoryManager's stepping action */
	TrajectoryManager::instance()->stepping(aStep);

	/* Check if the track should be killed. */
	checkKillTrack(aStep);
}

void SteppingAction::checkKillTrack(const G4Step* aStep) {
	/* Check if the preStepPoint region has tracking killing enabled. */
	UserRegionInformation* regionInfo = UserRegionInformation::getRegionInformation(aStep->GetPreStepPoint());
	if (regionInfo->getKillTracks()) {
		/* Kill the track if region track kill flag is set to true. */
		log() << LOG::always << "Killing track: " << aStep->GetTrack()->GetTrackID() << LOG::done;
		aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	}
}

} // namespace slic
