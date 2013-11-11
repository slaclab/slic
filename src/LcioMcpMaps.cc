// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMcpMaps.cc,v 1.8 2013-11-06 00:23:37 jeremy Exp $
#include "LcioMcpMaps.hh"

// slic
#include "LcioMcpManager.hh"
#include "Module.hh"
#include "Trajectory.hh"

using EVENT::MCParticle;
using IMPL::MCParticleImpl;

namespace slic {

LcioMcpMaps::LcioMcpMaps(LcioMcpManager* manager) :
		Module("LcioMcpMaps") {
	m_manager = manager;
}

LcioMcpMaps::~LcioMcpMaps() {
}

void LcioMcpMaps::addTrackToMCParticle(G4int trackID, MCParticleImpl* particle) {
	assert(particle);
	assert(trackID > 0);

	m_trackToMcp[trackID] = particle;
}

MCParticleImpl* LcioMcpMaps::findParticle(G4int trkID) const {
	MCParticleImpl* mcp = 0;
	for (TrackToMcpMap::const_iterator iter = m_trackToMcp.begin(); iter != m_trackToMcp.end(); iter++) {
		if (iter->first == trkID) {
			mcp = iter->second;
			break;
		}
	}
	return mcp;
}

/* Find the trackID for the final MCParticle. */
G4int LcioMcpMaps::findTrack(MCParticle* mcp) const {
	G4int trkID = -1;
	for (TrackToMcpMap::const_iterator iter = m_trackToMcp.begin(); iter != m_trackToMcp.end(); iter++) {
		if (iter->second == mcp) {
			trkID = iter->first;
			break;
		}
	}
	return trkID;
}

MCParticle* LcioMcpMaps::findInitialParticle(Trajectory* trj) const {
	MCParticle* mcpPrim = 0;

	G4int trjTrkID = trj->GetTrackID();

	/* Get initial collection. */
	EVENT::LCCollection* initMcpColl = m_manager->getInitialMcpCollection();

	// loop over input collection
	int numPrePart = initMcpColl->getNumberOfElements();
	for (int k = 0; k < numPrePart; k++) {

		// current MCP
		MCParticle* mcpPre = static_cast<MCParticle*>(initMcpColl->getElementAt(k));

		// do we have matching primary for this Mcp?
		G4PrimaryParticle* primaryMatch = findPrimary(mcpPre);
		if (primaryMatch) {

			// found matching primary to this input trajectory
			if (primaryMatch->GetTrackID() == trjTrkID) {
				mcpPrim = mcpPre;
				break;
			}

		}
	}

	return mcpPrim;
}

MCParticle* LcioMcpMaps::findDaughterParticle(MCParticle* mcpInit, G4PrimaryParticle* primDau) const {
	// loop over Mcp input particle's daughters
	MCParticle* mcpDau = 0;
	int numMcpDau = mcpInit->getDaughters().size();
	for (int i = 0; i < numMcpDau; i++) {
		// does ith Mcp dau match this primary?
		if (findPrimary(mcpInit->getDaughters()[i]) == primDau) {
			// found it
			mcpDau = mcpInit->getDaughters()[i];
			break;
		}
	}

	// return dau
	return mcpDau;
}

MCParticleImpl* LcioMcpMaps::findFinalParticle(MCParticle* mcpInit) const {
	MCParticleImpl* mcpFinal = 0;
	for (InitMcpToMcpMap::const_iterator iter = m_initMcpToMcp.begin(); iter != m_initMcpToMcp.end();
			iter++) {
		if (iter->first == mcpInit) {
			mcpFinal = iter->second;
			break;
		}
	}

	return mcpFinal;
}

G4PrimaryParticle* LcioMcpMaps::findPrimary(MCParticle* mcp) const {
	G4PrimaryParticle *pp = 0;
	for (McpToPrimaryMap::const_iterator iter = m_McpToPrimary.begin(); iter != m_McpToPrimary.end();
			iter++) {
		if (iter->first == mcp) {
			pp = iter->second;
			break;
		}
	}

	return pp;
}

void LcioMcpMaps::addParticleToPrimary(MCParticle* mcp, G4PrimaryParticle* primary) {
	m_McpToPrimary[mcp] = primary;
}

void LcioMcpMaps::addInitialParticleToFinal(MCParticle* initial, MCParticleImpl* final) {
	m_initMcpToMcp[initial] = final;
}

void LcioMcpMaps::clear() {
	m_trackToMcp.clear();
	m_McpToPrimary.clear();
	m_initMcpToMcp.clear();
}

void LcioMcpMaps::printMaps() {
	log() << LOG::debug << "Printing maps..." << LOG::done;

	printTracksToParticles();
	printParticlesToPrimaries();
	printInitialParticleToFinal();
	//    TrajectoryManager::instance()->printTrackIDToTrajectoryMap();
}

void LcioMcpMaps::printTracksToParticles() {
	log() << LOG::debug << "Printing track to Mcp map..." << LOG::done;

	log() << LOG::debug << "*** TrackToMcpMap ***" << LOG::done;
	log() << LOG::debug << "trkID | McpPtr" << LOG::done;
	for (TrackToMcpMap::iterator iter = m_trackToMcp.begin(); iter != m_trackToMcp.end(); iter++) {
		log() << LOG::debug << iter->first << " " << iter->second << LOG::done;
	}
	log() << LOG::debug << LOG::done;
}

void LcioMcpMaps::printParticlesToPrimaries() {
	log() << LOG::debug << "Printing Mcp to primary map..." << LOG::done;

	log() << LOG::debug << LOG::done;
	log() << LOG::debug << "*** McpToPrimaryMap ***" << LOG::done;
	log() << LOG::debug << "McpPtr | PrimaryPtr" << LOG::done;

	for (McpToPrimaryMap::iterator iter = m_McpToPrimary.begin(); iter != m_McpToPrimary.end(); iter++) {
		log() << LOG::debug << iter->first << " " << iter->second << LOG::done;
	}
	log() << LOG::debug << LOG::done;
}

void LcioMcpMaps::printInitialParticleToFinal() {
	log() << LOG::debug << "Printing InitMcp to Mcp map..." << LOG::done;

	log() << LOG::debug << LOG::done;
	log() << LOG::debug << "*** McpToMcpMap ***" << LOG::done;
	log() << LOG::debug << "McpPtrInit | McpPtrFinal" << LOG::done;

	for (InitMcpToMcpMap::iterator iter = m_initMcpToMcp.begin(); iter != m_initMcpToMcp.end(); iter++) {
		log() << LOG::debug << iter->first << " " << iter->second << LOG::done;
	}
	log() << LOG::debug << LOG::done;
}
}
