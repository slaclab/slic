// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/TrajectoryManager.cc,v 1.54 2013-11-06 00:23:37 jeremy Exp $

// slic
#include "TrajectoryManager.hh"
#include "Trajectory.hh"

// geant4
#include "G4TrackVector.hh"
#include "G4Event.hh"

namespace slic {

TrajectoryManager::~TrajectoryManager() {
}

TrajectoryManager::TrajectoryManager() :
		Module("TrajectoryManager"), m_trackingManager(0), m_currentTrajectory(0), m_currentTrack(0), m_currentTrackID(
				-1) {
}

void TrajectoryManager::preTracking(const G4Track* aTrack) {
	m_currentTrack = aTrack;
	m_currentTrackID = aTrack->GetTrackID();

#ifdef SLIC_LOG
	log() << LOG::verbose
	<< "TrajectoryManager::preTracking()" << LOG::endl
	<< "trackID <" << aTrack->GetTrackID() << ">" << LOG::endl
	<< "particle <" << aTrack->GetDefinition()->GetParticleName() << ">" << LOG::endl
	<< "volume <" << aTrack->GetVolume()->GetName() << ">"
	<< LOG::done;
	printStatusFlags(aTrack);
#endif

	// default trajectory storage to OFF
	m_trackingManager->SetStoreTrajectory(false);

	// handle primary or secondary track
	if (isPrimaryTrack(aTrack)) {
		handlePrimaryTrack(aTrack);
	} else {
		handleSecondaryTrack(aTrack);
	}
}

void TrajectoryManager::printStatusFlags(const G4Track* trk) {
	log() << LOG::debug << "status flags for trackID <" << trk->GetTrackID() << ">" << LOG::done;
	printStatusFlags(TrackInformation::getTrackInformation(trk));
}

void TrajectoryManager::printStatusFlags(TrackInformation* trkInfo) {
	if (trkInfo) {
		log() << LOG::verbose << "tracking status <"
				<< TrackInformation::getTrackingStatusString(trkInfo->getTrackingStatus()) << ">" << LOG::endl
				<< "orig tracking status <"
				<< TrackInformation::getTrackingStatusString(trkInfo->getOriginalTrackingStatus()) << ">"
				<< LOG::endl << "backscatter flag <" << trkInfo->getBackscatter() << ">" << LOG::endl
				<< "belowThreshold flag <" << trkInfo->getBelowThreshold() << ">" << LOG::endl
				<< "tracker hit flag <" << trkInfo->hasTrackerHit() << ">" << LOG::endl << "vtxnotendp flag <"
				<< trkInfo->getVertexIsNotEndpointOfParent() << ">" << LOG::done;
	}
#ifdef SLIC_LOG
	else {
		log() << LOG::debug << "Track info NULL in printStatusFlags()." << LOG::done;
	}
#endif
}

void TrajectoryManager::printTrajectory(const Trajectory* trj) {
	log() << LOG::debug << "trajectory info" << LOG::endl << "trackID <" << trj->GetTrackID() << ">"
			<< LOG::endl << "final status: " << trj->getFinalStatusString() << LOG::endl << "endp dau: "
			<< trj->getHasEndpointDaughters() << LOG::endl << "backscatter: " << trj->getBackscatter()
			<< LOG::endl << "vtxnotendp: " << trj->getVertexIsNotEndpointOfParent() << LOG::endl
			<< "created in sim: " << trj->getCreatedInSimulation() << LOG::endl << "vtx: "
			<< trj->GetPoint(0)->GetPosition() << LOG::endl << "endp: "
			<< trj->GetPoint(trj->GetPointEntries() - 1)->GetPosition() << LOG::endl << LOG::done;
}

void TrajectoryManager::postTracking(const G4Track* aTrack) {
#ifdef SLIC_LOG
	log() << LOG::verbose << "TrajectoryManager::postTracking()" << LOG::endl;
#endif

	// check if needs manual setup
	if (needsManualTrajectorySetup(aTrack)) {

#ifdef SLIC_LOG
		log() << LOG::verbose << "needs manual trajectory setup" << LOG::done;
#endif

		// setup trajectory in default way
		Trajectory* trj = createTrajectoryFromTrack(aTrack);

		// do additional, manual setup using this track
		trj->setupManuallyFromTrack(aTrack);
	}

	// trk info to secondaries
	copySecondaryTrackInformationFromParent(aTrack);

	// set final status codes in current trajectory
	setTrajectoryFinalStatus(aTrack);

	// print if store traj on/off
#ifdef SLIC_LOG
	log() << LOG::verbose << "store trajectory <" << m_trackingManager->GetStoreTrajectory() << ">" << LOG::done;
#endif

	// print status flags
#ifdef SLIC_LOG
	printStatusFlags(aTrack);
#endif

	Trajectory* trj = static_cast<Trajectory*>(m_trackingManager->GimmeTrajectory());
	if (0 != trj) {
		trj->setEndpointEnergy(aTrack->GetTotalEnergy());
	}
}

void TrajectoryManager::handlePrimaryTrack(const G4Track* aTrack) {
	// set track info
	setPrimaryTrackInformation(aTrack);

	// create the trajectory
	createTrajectoryFromTrack(aTrack);
}

Trajectory* TrajectoryManager::createTrajectoryFromTrack(const G4Track* aTrack) {
#ifdef SLIC_LOG
	log() << LOG::verbose << "createTrajectoryFromTrack()" << LOG::done;
#endif
	Trajectory* trj = new Trajectory(aTrack);
	m_trackingManager->SetStoreTrajectory(true);
	m_trackingManager->SetTrajectory(trj);
	addTrackIDToTrajectoryLink(trj);
	m_currentTrajectory = trj;
	return trj;
}

void TrajectoryManager::setPrimaryTrackInformation(const G4Track* aTrack) {
	TrackInformation* trkInfo = new TrackInformation(aTrack);

	// set tracking flag using reg info
	setTrackingFlagFromRegionInformation(trkInfo, UserRegionInformation::getRegionInformation(aTrack));

	G4Track* nconstTrack = const_cast<G4Track*>(aTrack);
	nconstTrack->SetUserInformation(trkInfo);
}

void TrajectoryManager::setTrackingFlagFromRegionInformation(TrackInformation* trk_info,
		UserRegionInformation* reg_info) {
	if (reg_info->getStoreSecondaries() == true) {
		trk_info->setTrackingStatus(TrackInformation::eInTrackingRegion);
	} else {
		trk_info->setTrackingStatus(TrackInformation::eInNontrackingRegion);
	}
}

/*
 Called in PreTracking to set status flags and
 decide whether or not to create a trj for
 this secondary particle.
 */
void TrajectoryManager::handleSecondaryTrack(const G4Track* aTrack) {
	// get track information
	TrackInformation* trkInfo = TrackInformation::getTrackInformation(aTrack);

	// get region information
	UserRegionInformation* regInfo = UserRegionInformation::getRegionInformation(aTrack);

	// must have track and region info to proceed
	if (regInfo && trkInfo) {

		// check if in tracking region
		if (regInfo->getStoreSecondaries()) {

			// check if backscatter
			if (isBackscatter(aTrack)) {
				trkInfo->setBackscatter(true);
			}

			/*
			 Check if below threshold and continuous process creation,
			 i.e. not an endp dau.
			 */
			else if (isBelowThreshold(aTrack) && trkInfo->getVertexIsNotEndpointOfParent()) {
				trkInfo->setBelowThreshold(true);
			}

			/*
			 Process of elimination: above threshold and not backscatter,
			 so requires a new trajectory and trkInfo.
			 */
			else {

				// setup secondary track info
				setupSecondaryTrackInformation(aTrack);

				// create the secondary trajectory
				createSecondaryTrajectory(aTrack);
			}

			/*
			 // DEBUG
			 if ( trkInfo != TrackInformation::getTrackInformation( aTrack ) ) {
			 log() << "WARNING: track info was reset" << LOG::endl;

			 log() << LOG::endl;
			 log() << "old flags" << LOG::endl;
			 printStatusFlags( trkInfo );
			 log() << LOG::endl;

			 log() << "new flags" << LOG::endl;
			 printStatusFlags( TrackInformation::getTrackInformation( aTrack ) );
			 log() << LOG::endl;
			 }
			 */

			/*
			 Set flag for in tracking region, re-getting track info,
			 because may have been recreated above.
			 */
			TrackInformation::getTrackInformation(aTrack)->setTrackingStatus(
					TrackInformation::eInTrackingRegion);
		}
		// in calorimeter, i.e. non-tracking
		else {

			// set non tracking status
			trkInfo->setTrackingStatus(TrackInformation::eInNontrackingRegion);
			trkInfo->setOriginalTrackingStatus(TrackInformation::eInNontrackingRegion);
		}
	}
	// fatal error: regInfo or trkInfo were null
	else {
		log() << LOG::fatal << "FATAL ERROR: region info or track info were null in handleSecondaryTrack()."
				<< LOG::endl << "region info <" << regInfo << ">" << LOG::endl << "track info <" << trkInfo
				<< ">" << LOG::done;
		G4Exception("", "", FatalException, "Track info or region info was null.");
	}
}

void TrajectoryManager::setupSecondaryTrackInformation(const G4Track* aTrack) {
	// old track info
	TrackInformation* oldTrkInfo = TrackInformation::getTrackInformation(aTrack);

	// create new track info
	TrackInformation* newTrkInfo = new TrackInformation(aTrack);

	// copy vtxnotendp flag from old trkInfo
	newTrkInfo->setVertexIsNotEndpointOfParent(oldTrkInfo->getVertexIsNotEndpointOfParent());

	/*
	 Delete old track info to avoid mem leak.
	 */
	delete oldTrkInfo;
	oldTrkInfo = 0;

	/*
	 Set original tracking status; it must be tracking, because otherwise
	 would not be calling this function.  (By definition, secondary tracks
	 do not get trajectories in non-tracking regions.)
	 */
	newTrkInfo->setOriginalTrackingStatus(TrackInformation::eInTrackingRegion);

	/*
	 Setup new track info in this track.
	 */
	G4Track* nconstTrk = const_cast<G4Track*>(aTrack);
	nconstTrk->SetUserInformation(newTrkInfo);
}

Trajectory* TrajectoryManager::createSecondaryTrajectory(const G4Track* aTrack) {
	// create the trajectory
	Trajectory* trj = createTrajectoryFromTrack(aTrack);

	// set CreatedInSimulation flag
	trj->setCreatedInSimulation(true);

	return trj;
}

// called in PostTracking to pass trk info to the track's secondaries
void TrajectoryManager::copySecondaryTrackInformationFromParent(const G4Track* aTrack) {
	// get track info for parent
	TrackInformation* parTrkInfo = TrackInformation::getTrackInformation(aTrack);

	// get current trajectory from tracking manager
	Trajectory* parTrj = getCurrentTrajectory();

	// get secondaries container
	G4TrackVector* secondaries = m_trackingManager->GimmeSecondaries();

	// loop over secondaries to 1) create trk info and 2) set vertexIsNotEndpointOfParent flag
	if (secondaries) {
		size_t nseco = secondaries->size();
		for (size_t i = 0; i < nseco; i++) {

			// set current seco track ptr
			G4Track* seco = (*secondaries)[i];

			// setup new trk info for seco
			TrackInformation* secoTrkInfo = new TrackInformation(parTrkInfo);
			seco->SetUserInformation(secoTrkInfo);

			// set vtxnotendp flag in trk info
			if (parTrj) {

				// get parent's endpoint
				G4ThreeVector parEndpoint = getTrajectoryEndpoint(parTrj);

				// check if direct parent (flag always on if not) OR endp not near
				if (secoTrkInfo->getOriginalTrackID() != aTrack->GetTrackID()
						|| vertexIsNotEndpointOfParent(parEndpoint, seco)) {

					// set vertexIsNotEndpointOfParent flag in trk info
					secoTrkInfo->setVertexIsNotEndpointOfParent(true);
				}
			}
			// vtxnotendp always set, because parent is not stored
			else {
				secoTrkInfo->setVertexIsNotEndpointOfParent(true);
			}
		}

#ifdef SLIC_LOG
		log() << LOG::verbose << "track info passed to <" << nseco << "> secondaries" << LOG::endl;
#endif
	}
}

bool TrajectoryManager::needsManualTrajectorySetup(const G4Track* aTrack) {
	// store != ON and has a corresponding tracker hit
	return (!m_trackingManager->GetStoreTrajectory())
			&& (TrackInformation::getTrackInformation(aTrack)->hasTrackerHit());
}

void TrajectoryManager::stepping(const G4Step* aStep) {
	const G4Track* theTrack = aStep->GetTrack();

	// step has a live track
	if (isAlive(aStep)) {

		TrackInformation* trkInfo = TrackInformation::getTrackInformation(theTrack);

		UserRegionInformation* preReg = UserRegionInformation::getRegionInformation(
				aStep->GetPreStepPoint());
		UserRegionInformation* postReg = UserRegionInformation::getRegionInformation(
				aStep->GetPostStepPoint());

		// set tracking status based on region info
		if (preReg && postReg) {

			// tracking to non-tracking
			if (preReg->getStoreSecondaries() && (!postReg->getStoreSecondaries())) {
				trkInfo->setTrackingStatus(TrackInformation::eInNontrackingRegion);
			}
			// non-tracking to tracking
			else if (!preReg->getStoreSecondaries() && postReg->getStoreSecondaries()) {
				TrackInformation::ETrackingStatus origStatus = trkInfo->getOriginalTrackingStatus();

				// if looped like tracking -> nontracking -> tracking without dying
				if (!(origStatus == TrackInformation::eInTrackingRegion)) {
					trkInfo->setBackscatter(true);
				}

				// set tracking status to tracking
				trkInfo->setTrackingStatus(TrackInformation::eInTrackingRegion);
			}
		}
	}
}

void TrajectoryManager::setTrajectoryFinalStatus(const G4Track* aTrack) {
	// get current traj
	Trajectory* trj = getCurrentTrajectory();

	// must have trajectory to set status
	if (trj) {

		G4StepPoint* postStep = aTrack->GetStep()->GetPostStepPoint();

		// get track info
		TrackInformation* trkInfo = TrackInformation::getTrackInformation(aTrack);

		// has endpoint daughters
		G4int nSecoAtEnd = getNumberOfSecondariesAtEnd(aTrack);
		if (nSecoAtEnd > 0) {
			trj->setHasEndpointDaughters(true);
		}

		// default final status to unset
		Trajectory::EFinalStatus finalStatus = Trajectory::eUnset;

		// backscatter from track info
		trj->setBackscatter(trkInfo->getBackscatter());

		// vertexIsNotEndpointOfParent
		if (trkInfo->getVertexIsNotEndpointOfParent()) {
			trj->setVertexIsNotEndpointOfParent(true);
		}

		// no decay -> no secondaries at end
		if (nSecoAtEnd <= 0) {
			// left detector
			if (!postStep->GetMaterial()) {
				finalStatus = Trajectory::eLeftDetector;
			}

			// stopped
			else if (aTrack->GetKineticEnergy() <= 0.) {
				finalStatus = Trajectory::eStopped;
			}
			// unflagged loopers killed by G4 
#ifdef SLIC_DEBUG
			else {
				log() << LOG::debug << "Unknown decay case (probably a looper)." << LOG::done;
			}
#endif
		}
		// decayed
		else {

			// decayed in tracker
			if (trkInfo->getTrackingStatus() == TrackInformation::eInTrackingRegion) {
				finalStatus = Trajectory::eDecayedInTracker;
			}
			// process of elimination: decayed in calorimeter
			else {
				finalStatus = Trajectory::eDecayedInCalorimeter;
			}
		}

		// set final status in trajectory
		trj->setFinalStatus(finalStatus);
	}
}

void TrajectoryManager::printTrack(const G4Track* trk, bool isSecondary) {
	log() << LOG::endl;
	log() << "track ptr <" << trk << ">" << LOG::endl;
	log() << "trackID <" << trk->GetTrackID() << ">" << LOG::endl;
	log() << "parent trackID <" << trk->GetParentID() << ">" << LOG::endl;
	log() << "PDG <" << trk->GetDefinition()->GetPDGEncoding() << ">" << LOG::endl;
	log() << "position <" << trk->GetPosition() << ">" << LOG::endl;

	if (!isSecondary) {
		if (trk->GetMaterial()) {
			log() << "material <" << trk->GetMaterial()->GetName() << ">" << LOG::endl;
		} else {
			log() << "NO material" << LOG::endl;
		}

		if (trk->GetVolume()) {
			log() << "volume <" << trk->GetVolume()->GetName() << ">" << LOG::endl;
		} else {
			log() << "NO volume" << LOG::endl;
		}
	}

	if (!isSecondary) {
		log() << "numSecoAtEnd <" << getNumberOfSecondariesAtEnd(trk) << ">" << LOG::endl;
	}

	log() << "kE <" << trk->GetKineticEnergy() << ">" << LOG::endl;
	log() << "mom <" << trk->GetMomentum() << ">" << LOG::endl;

	printStatusFlags(trk);

	log() << LOG::endl;
}

void TrajectoryManager::printPhysVolDbg(G4StepPoint* stepPnt) {
	if (stepPnt) {
		G4VPhysicalVolume* pv = stepPnt->GetPhysicalVolume();

		if (pv) {
			log() << LOG::debug << "PV name <" << pv->GetName() << ">" << LOG::endl;
		} else {
			log() << LOG::debug << "PV is NULL!" << LOG::endl;
		}
	}
}

G4int TrajectoryManager::getNumberOfSecondariesAtEnd(const G4Track* aTrack) {
	G4int numSecoAtEnd = 0;
	G4TrackVector* seco = m_trackingManager->GimmeSecondaries();
	G4ThreeVector endPnt = aTrack->GetPosition();

	if (seco) {
		size_t numSeco = seco->size();

#ifdef SLIC_LOG
		log() << LOG::debug << "numSeco <" << numSeco << ">" << LOG::endl;
#endif

		for (size_t i = 0; i < numSeco; i++) {
			// if seco endpoint near track's endpoint
			if ((*seco)[i]->GetPosition().isNear(endPnt)) {
				++numSecoAtEnd;
			}
		}
	}

#ifdef SLIC_LOG
	log() << LOG::debug << "found numSecoAtEnd <" << numSecoAtEnd << ">" << LOG::endl;
#endif

	return numSecoAtEnd;
}

void TrajectoryManager::fillTrajectoryMap(const G4Event* anEvent, bool clearIt) {
#ifdef SLIC_LOG
	log() << LOG::verbose << "fillTrackIDToTrajectoryMap()" << LOG::endl;
#endif
	if (clearIt) {
		clearTrackIDToTrajectoryMap();
	}

	G4TrajectoryContainer* trajCont = anEvent->GetTrajectoryContainer();

	if (trajCont) {
		G4int n_traj = trajCont->entries();

		Trajectory* trj = 0;
		for (int i = 0; i < n_traj; i++) {
			trj = static_cast<Trajectory*>((*trajCont)[i]);
			addTrackIDToTrajectoryLink(trj);
		}
	} else {
		log() << LOG::error << "Cannot fill map without a trajectory container!" << LOG::endl;
	}
}

void TrajectoryManager::addTrackIDToTrajectoryLink(Trajectory* trj) {
	if (trj) {
		m_trackIDToTrajectory[trj->GetTrackID()] = trj;
	} else {
		log() << LOG::error << "Trajectory is null!" << LOG::endl;
	}
}

void TrajectoryManager::printSecondaries() {
	log() << LOG::debug << "secondaries for trackID <" << m_trackingManager->GetTrack()->GetTrackID() << ">"
			<< LOG::done;

	G4TrackVector* secondaries = m_trackingManager->GimmeSecondaries();
	size_t nseco = secondaries->size();
	for (size_t i = 0; i < nseco; i++) {

		// set current seco track ptr
		G4Track* seco = (*secondaries)[i];
		log() << LOG::debug << "secondary track <" << i << ">" << LOG::endl;
		printTrack(seco, true);
	}
}

// find trajectory by track ID
Trajectory* TrajectoryManager::findTrajectory(G4int trkID) {
#ifdef SLIC_LOG
	log() << LOG::debug << "findTrajectory() - trkID <" << trkID << ">" << LOG::endl;
#endif

	Trajectory* trj = 0;
	TrackIDToTrajectoryMap::iterator iter;

	iter = m_trackIDToTrajectory.find(trkID);

	if (iter != m_trackIDToTrajectory.end()) {
		trj = iter->second;
	}

#ifdef SLIC_LOG
	if ( trj ) {
		log() << LOG::debug << "found trajectory" << LOG::endl;
	}
	else {
		log() << LOG::debug << "no trajectory found" << LOG::endl;
	}
#endif

	return trj;
}
}
