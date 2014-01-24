// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/Trajectory.hh,v 1.19 2013-11-06 00:23:36 jeremy Exp $

#ifndef SLIC_TRAJECTORY_HH
#define SLIC_TRAJECTORY_HH 1

// geant4
#include "G4VTrajectory.hh"
#include "G4Allocator.hh"
#include "G4Track.hh"
#include "G4VTrajectoryPoint.hh"
#include "globals.hh"

namespace slic {

typedef std::vector<G4VTrajectoryPoint*> TrajectoryPointContainer;

/**
 * @class Trajectory
 * @brief Implementation of G4VTrajectory.
 */
class Trajectory: public G4VTrajectory {
public:

    /**
     * Final status flags.
     */
	enum EFinalStatus {
		eUnset = 0, eDecayedInTracker = 1, eLeftDetector = 2, eStopped = 3, eDecayedInCalorimeter = 4
	};

public:

	/**
	 * Class constructor.
	 */
	Trajectory();

	/**
	 * Class constructor.
	 * @param[in] aTrack An associated G4Track.
	 */
	Trajectory(const G4Track *aTrack);

	/**
	 * Copy constructor.
	 * @param[in] trajectory The Trajectory to copy.
	 */
	Trajectory(Trajectory& trajectory);

	/**
	 * Class destructor.
	 */
	virtual ~Trajectory();

public:

	/**
	 * Draw a Trajectory.
	 * @param[in] i_mode The draw mode.
	 */
	//virtual void DrawTrajectory(G4int i_mode) const {
	//
	virtual void DrawTrajectory() const {
		G4VTrajectory::DrawTrajectory();
	}

	/**
	 * Convert final status enum to string (static version).
	 * @param[in] fs The EFinalStatus.
	 * @return The status converted to a string.
	 */
	static const std::string& getFinalStatusString(EFinalStatus fs);

	/**
	 * Convert the current final status enum to string.
	 * @return The current final status converted to a string.
	 */
	inline const std::string& getFinalStatusString() const {
		return Trajectory::getFinalStatusString(m_finalStatus);
	}

	/**
	 * Specialized implementation of the new operator.
	 * @param[in] size The size to allocate.
	 */
	inline void* operator new(size_t size);

	/**
	 * Specialized implementation of the delete operator.
	 * @param[in] pointer Pointer to the object to delete.
	 */
	inline void operator delete(void* pointer);

	/**
	 * Equals operator.
	 * @param[in] right The comparison object.
	 */
	inline int operator ==(const Trajectory& right) const {
		return (this == &right);
	}

	/**
	 * Get the track ID of this Trajectory.
	 * @return The track ID.
	 */
	inline G4int GetTrackID() const {
		return m_trackID;
	}

	/**
	 * Get parent track ID.
	 * @return The parent track ID.
	 */
	inline G4int GetParentID() const {
		return m_parentID;
	}

	/**
	 * Get the name of the particle associated with the Trajectory.
	 * @return The name of the particle.
	 */
	inline G4String GetParticleName() const {
		return m_particleName;
	}

	/**
	 * Get the charge.
	 * @return The charge.
	 */
	inline G4double GetCharge() const {
		return m_PDGCharge;
	}

	/**
	 * Get the PDG encoding of this Trajectory.
	 * @return The PDG encoding.
	 */
	inline G4int GetPDGEncoding() const {
		return m_PDGEncoding;
	}

	/**
	 * Get the initial momentum.
	 * @return The initial momentum.
	 */
	inline G4ThreeVector GetInitialMomentum() const {
		return m_initialMomentum;
	}

	/**
	 * Get the particle definition.
	 * @return The particle definition.
	 */
	inline const G4ParticleDefinition* GetParticleDefinition() {
		return m_particleDefinition;
	}

	/**
	 * Get the creation time of the particle, measured from the start of the event.
	 * @return The creation time.
	 */
	inline G4double GetGlobalTime() {
		return m_globalTime;
	}

	/**
	 * Return true if particle has daughters produced at its endpoint.
	 * @return True if particle has endpoint daughters; false if not.
	 */
	inline G4bool getHasEndpointDaughters() const {
		return m_hasEndpointDaughters;
	}

	/**
	 * Set that this Trajectory has endpoint daughters.
	 * @param[in] d Set to true if has endpoint daughters; false if not.
	 */
	inline void setHasEndpointDaughters(G4bool d) {
		m_hasEndpointDaughters = d;
	}

	/**
	 * Return true if this Trajectory is backscattered from a non-tracking region
	 * back into the tracking region.
	 * @return True if Trajectory is backscattered; false if not.
	 */
	inline G4bool getBackscatter() const {
		return m_backscatter;
	}

	/**
	 * Set backscatter.
	 * @param[in] b Set to true if backscatter; false if not.
	 */
	inline void setBackscatter(G4bool b) {
		m_backscatter = b;
	}

	/**
	 * Get vertex is not endpoint of parent flag.
	 * @return The vertix is not endpoint of parent flag.
	 */
	inline G4bool getVertexIsNotEndpointOfParent() const {
		return m_vertexIsNotEndpointOfParent;
	}

	/**
	 * Set the vertex is not endpoint of parent flag.
	 * @param[in] v The vertex is not endpoint of parent flag value.
	 */
	inline void setVertexIsNotEndpointOfParent(G4bool v = true) {
		m_vertexIsNotEndpointOfParent = true;
	}

	/**
	 * Set the final status.
	 * @param[in] fstatus The final status.
	 */
	inline void setFinalStatus(EFinalStatus fstatus) {
		m_finalStatus = fstatus;
	}

	/**
	 * Get the final status.
	 * @return The final status.
	 */
	inline EFinalStatus getFinalStatus() const {
		return m_finalStatus;
	}

	/**
	 * Set the created in simulation flag.
	 * @param[in] c The created in simulation flag.
	 */
	inline void setCreatedInSimulation(G4bool c) {
		m_createdInSimulation = c;
	}

	/**
	 * Get the created in simulation flag.
	 * @return The created in simulation flag.
	 */
	inline G4bool getCreatedInSimulation() const {
		return m_createdInSimulation;
	}

	/**
	 * Setup this Trajectory from a G4Track.
	 * @param[in] aTrack The G4Track.
	 */
	void setupManuallyFromTrack(const G4Track* aTrack);

	/**
	 * Set the endpoint energy.
	 * @param[in] e The endpoint energy.
	 */
	inline void setEndpointEnergy(G4double e) {
		m_endpointEnergy = e;
	}

	/**
	 * Get the endpoint energy.
	 * @return The endpoint energy in GeV.
	 */
	G4double getEndPointEnergy() {
		return m_endpointEnergy;
	}

public:

	/**
	 * Append the information from a G4Step.
	 * @param[in] aStep The G4Step.
	 */
	void AppendStep(const G4Step *aStep);

	/**
	 * Merge a trajectory into this one.
	 * @param[in] secondTrajectory The Trajectory to be merged.
	 */
	void MergeTrajectory(G4VTrajectory *secondTrajectory);

	/**
	 * Get the number of point entries.
	 * @return The number of point entries.
	 */
	virtual G4int GetPointEntries() const {
		return m_posRecord->size();
	}

	/**
	 * Get a G4VTrajectoryPoint by index.
	 * @param[in] i The index.
	 * @return The trajectory point at index.
	 */
	virtual G4VTrajectoryPoint* GetPoint(G4int i) const {
		return (*m_posRecord)[i];
	}

private:

	// track info
	G4ParticleDefinition* m_particleDefinition;
	G4String m_particleName;
	G4double m_PDGCharge;
	G4int m_PDGEncoding;
	G4int m_trackID;
	G4int m_parentID;
	TrajectoryPointContainer* m_posRecord;
	G4ThreeVector m_initialMomentum;
	G4double m_globalTime;
	G4double m_endpointEnergy;

	// non-exclusive sim statuses
	G4bool m_createdInSimulation;
	G4bool m_hasEndpointDaughters;
	G4bool m_backscatter;
	G4bool m_vertexIsNotEndpointOfParent;

	// mutually exclusive final states: see enums
	EFinalStatus m_finalStatus;
};

extern G4Allocator<Trajectory> TrajectoryAllocator;

inline void* Trajectory::operator new(size_t) {
	void *aTrajectory;
	aTrajectory = (void*) TrajectoryAllocator.MallocSingle();
	return aTrajectory;
}

inline void Trajectory::operator delete(void* aTrajectory) {
	TrajectoryAllocator.FreeSingle((Trajectory*) aTrajectory);
}
}

#endif
