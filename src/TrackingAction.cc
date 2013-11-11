// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/TrackingAction.cc,v 1.14 2012-11-27 19:32:19 jeremy Exp $
#include "TrackingAction.hh"

// slic
#include "Trajectory.hh"
#include "TrajectoryManager.hh"

namespace slic {

TrackingAction::TrackingAction() {
	// set TrajectoryManager links
	m_trjMgr = TrajectoryManager::instance();
}

TrackingAction::~TrackingAction() {
}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
	// TrajectoryManager's action; also requires setup of trkMgr
	m_trjMgr->setTrackingManager(fpTrackingManager); // needs to be called every time?
	m_trjMgr->preTracking(aTrack);
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
	// TrajectoryManager's action
	m_trjMgr->postTracking(aTrack);
}
}
