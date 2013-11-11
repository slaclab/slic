// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/Trajectory.cc,v 1.17 2013-06-26 01:57:02 jeremy Exp $
#include "Trajectory.hh"

// lcdd
#include "lcdd/hits/TrackInformation.hh"

// geant4
#include "G4TrajectoryPoint.hh"

namespace slic {

G4Allocator<Trajectory> TrajectoryAllocator;

Trajectory::Trajectory() :
		G4VTrajectory(), m_particleDefinition(0), m_particleName(""), m_PDGCharge(0), m_PDGEncoding(0), m_trackID(
				0), m_parentID(0), m_posRecord(0), m_globalTime(0), m_endpointEnergy(0), m_createdInSimulation(
				false), m_hasEndpointDaughters(false), m_backscatter(false), m_vertexIsNotEndpointOfParent(
				false) {
}

Trajectory::Trajectory(const G4Track *aTrack) :
		G4VTrajectory() {
	m_particleDefinition = aTrack->GetDefinition();
	m_particleName = m_particleDefinition->GetParticleName();
	m_PDGCharge = m_particleDefinition->GetPDGCharge();
	m_PDGEncoding = m_particleDefinition->GetPDGEncoding();
	m_trackID = aTrack->GetTrackID();
	m_parentID = aTrack->GetParentID();
	m_posRecord = new TrajectoryPointContainer();
	m_posRecord->push_back(new G4TrajectoryPoint(aTrack->GetPosition()));
	m_initialMomentum = aTrack->GetMomentum();

	// default flags to false
	m_createdInSimulation = false;
	m_hasEndpointDaughters = false;
	m_backscatter = false;
	m_vertexIsNotEndpointOfParent = false;

	m_globalTime = aTrack->GetGlobalTime();
}

void Trajectory::setupManuallyFromTrack(const G4Track* aTrack) {
	TrackInformation* trkInfo = static_cast<TrackInformation*>(aTrack->GetUserInformation());

	assert( trkInfo);

	m_parentID = trkInfo->getOriginalTrackID();

	// position already added in trajectory ctor
	m_posRecord->insert(m_posRecord->begin(), new G4TrajectoryPoint(aTrack->GetVertexPosition()));

	G4double vKE = aTrack->GetVertexKineticEnergy();

	G4double mass = m_particleDefinition->GetPDGMass();
	G4double magmom = sqrt(vKE * vKE + 2. * vKE * mass);
	m_initialMomentum = magmom * (aTrack->GetVertexMomentumDirection());

	m_globalTime = aTrack->GetGlobalTime();
}

Trajectory::Trajectory(Trajectory &right) :
		G4VTrajectory() {
	m_particleName = right.m_particleName;
	m_particleDefinition = right.m_particleDefinition;
	m_PDGCharge = right.m_PDGCharge;
	m_PDGEncoding = right.m_PDGEncoding;
	m_trackID = right.m_trackID;
	m_parentID = right.m_parentID;
	m_globalTime = right.m_globalTime;

	m_posRecord = new TrajectoryPointContainer();
	for (unsigned int i = 0; i < right.m_posRecord->size(); i++) {
		G4TrajectoryPoint *rightPoint = (G4TrajectoryPoint*) ((*(right.m_posRecord))[i]);
		m_posRecord->push_back(new G4TrajectoryPoint(*rightPoint));
	}
}

Trajectory::~Trajectory() {
	for (unsigned int i = 0; i < m_posRecord->size(); i++) {
		delete (*m_posRecord)[i];
	}

	m_posRecord->clear();
	delete m_posRecord;
}

void Trajectory::AppendStep(const G4Step *aStep) {
	m_posRecord->push_back(new G4TrajectoryPoint(aStep->GetPostStepPoint()->GetPosition()));
}

void Trajectory::MergeTrajectory(G4VTrajectory *secondTrajectory) {
	if (!secondTrajectory)
		return;

	Trajectory *seco = (Trajectory*) secondTrajectory;
	G4int pent = seco->GetPointEntries();
	for (int i = 1; i < pent; i++) {
		m_posRecord->push_back((*(seco->m_posRecord))[i]);
	}

	delete (*seco->m_posRecord)[0];
	seco->m_posRecord->clear();
}

const std::string& Trajectory::getFinalStatusString(EFinalStatus fs) {
	static const std::string str_unset = "Unset";
	static const std::string str_invalid = "Invalid";
	static const std::string str_decayed_trk = "DecayedInTracker";
	static const std::string str_left = "LeftDetector";
	static const std::string str_stopped = "Stopped";
	static const std::string str_decayed_cal = "DecayedInCalorimeter";

	if (fs == eDecayedInTracker) {
		return str_decayed_trk;
	} else if (fs == eLeftDetector) {
		return str_left;
	} else if (fs == eStopped) {
		return str_stopped;
	} else if (fs == eDecayedInCalorimeter) {
		return str_decayed_cal;
	} else if (fs == eUnset) {
		return str_unset;
	}

	return str_invalid;
}
}
