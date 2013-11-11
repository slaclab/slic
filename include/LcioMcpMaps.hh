// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioMcpMaps.hh,v 1.8 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMCPMAPS_HH
#define SLIC_LCIOMCPMAPS_HH 1

// slic
#include "Module.hh"

// lcio
#include "IMPL/MCParticleImpl.h"
#include "EVENT/MCParticle.h"

// geant4
#include "globals.hh"

// stl
#include <map>

// geant4
class G4PrimaryParticle;

namespace slic {

// slic
class LcioMcpManager;
class Trajectory;

/**
 * @class LcioMcpMaps
 * @brief Data maps that relate MCParticles to various other runtime objects.
 */
class LcioMcpMaps: public Module {

public:

    /**
     * Class constructor.
     * @param[in] particleManager The MCParticle Manager.
     */
	LcioMcpMaps(LcioMcpManager* particleManager);

	/**
	 * Class destructor.
	 */
	virtual ~LcioMcpMaps();

public:

	/**
	 * Map of track IDs to MCParticle.
	 */
	typedef std::map<G4int, IMPL::MCParticleImpl*> TrackToMcpMap;

	/**
	 * Map of MCParticle to G4PrimaryParticle.
	 */
	typedef std::map<EVENT::MCParticle*, G4PrimaryParticle*> McpToPrimaryMap;

	/**
	 * Map of initial MCParticle from StdHep to final MCParticle.
	 */
	typedef std::map<EVENT::MCParticle*, IMPL::MCParticleImpl*> InitMcpToMcpMap;

public:

	/**
	 * Map an initial MCParticle to a G4PrimaryParticle.
	 * @param[in] particle The MCParticle.
	 * @param[in] primary The G4PrimaryParticle.
	 */
	void addParticleToPrimary(EVENT::MCParticle* particle, G4PrimaryParticle* primary);

	/**
	 * Map a track ID to an MCParticle.
	 * @param[in] trackID The track ID.
	 * @param[in] particle The MCParticle.
	 */
	void addTrackToMCParticle(G4int trackID, IMPL::MCParticleImpl* particle);

	/**
	 * Map an initial MCParticle to a final output MCParticle.
	 * @param[in] initial The initial MCParticle.
	 * @param[in] final The final MCParticle.
	 */
	void addInitialParticleToFinal(EVENT::MCParticle* initial, IMPL::MCParticleImpl* final);

	/**
	 * Find an MCParticle from the associated trackID.
	 * @param[in] trackID The track ID.
	 * @return The MCParticle or null if not found.
	 */
	IMPL::MCParticleImpl* findParticle(G4int trackID) const;

	/**
	 * Find a trackID from a final MCParticle.
	 * @param[in] particle The final MCParticle.
	 * @return The track ID or -1 if not found.
	 */
	G4int findTrack(EVENT::MCParticle* particle) const;

	/**
	 * Find a G4PrimaryParticle from an MCParticle.
	 * @param[in] particle The MCParticle.
	 * @return The G4PrimaryParticle or null if not found.
	 */
	G4PrimaryParticle* findPrimary(EVENT::MCParticle* particle) const;

	/**
	 * Find an MCParticle from a Trajectory.
	 * @param[in] trajectory The Trajectory.
	 * @return The MCParticle or null if not found.
	 */
	EVENT::MCParticle* findInitialParticle(Trajectory* trajectory) const;

	/**
	 * Find a daughter MCParticle given a G4PrimaryParticle daughter.
	 * @param[in] initial The parent particle.
	 * @param[in] primary The G4PrimaryParticle daughter.
	 * @return The primary daughter or null if does not exist.
	 */
	EVENT::MCParticle* findDaughterParticle(EVENT::MCParticle* initial, G4PrimaryParticle* primary) const;

	/**
	 * Find a final, output MCParticle given an initial, input MCParticle.
	 * @param[in] initialParticle The intitial MCParticle.
	 */
	IMPL::MCParticleImpl* findFinalParticle(EVENT::MCParticle* initialParticle) const;

	/**
	 * Print out all the maps.
	 */
	void printMaps();

	/**
	 * Print the track to particle map.
	 */
	void printTracksToParticles();

	/**
	 * Print the particle to primary map.
	 */
	void printParticlesToPrimaries();

	/**
	 * Print the intial to final MCParticle map.
	 */
	void printInitialParticleToFinal();

	/**
	 * Clear the maps.
	 */
	void clear();

private:

	// trackID -> MCP
	TrackToMcpMap m_trackToMcp;

	// MCP -> G4PrimaryParticle
	McpToPrimaryMap m_McpToPrimary;

	// initial Mcp -> final Mcp
	InitMcpToMcpMap m_initMcpToMcp;

	// back pointer to LcioMcpManager
	LcioMcpManager* m_manager;
};
}

#endif
