// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMcpStatusSetter.cc,v 1.7 2013-11-06 00:23:37 jeremy Exp $
#include "LcioMcpStatusSetter.hh"

// slic
#include "Trajectory.hh"
#include "EventSourceManager.hh"

using IMPL::MCParticleImpl;
using EVENT::MCParticle;

namespace slic {
IMPL::MCParticleImpl* LcioMcpStatusSetter::setStatusCodesFromTrajectory(Trajectory* trj,
		MCParticleImpl* mcp) {
	// set vertexIsNotEndOfParent flag
	LcioMcpStatusSetter::setVertexIsNotEndpointOfParentFlag(trj, mcp);

	// set final status flags
	LcioMcpStatusSetter::setFinalStatusFlags(trj, mcp);

	// set backscatter flag
	LcioMcpStatusSetter::setBackscatterFlag(trj, mcp);

	// For GPS source, set generator status to FS for trajectories without parents,
	// i.e. the parent track ID is 0.
	if (EventSourceManager::instance()->getCurrentSourceType() == EventSourceManager::eGPS) {
		if (trj->GetParentID() == 0) {
			mcp->setGeneratorStatus(1);
		}
	}

	// return the MCP
	return mcp;
}

void LcioMcpStatusSetter::setBackscatterFlag(Trajectory* trj, IMPL::MCParticleImpl* mcp) {
	if (trj->getBackscatter()) {
		mcp->setBackscatter(true);
	}
}

void LcioMcpStatusSetter::setVertexIsNotEndpointOfParentFlag(Trajectory* trj, MCParticleImpl* mcp) {
	mcp->setVertexIsNotEndpointOfParent(trj->getVertexIsNotEndpointOfParent());
}

void LcioMcpStatusSetter::setCreatedInSimulationFlag(Trajectory* trj, IMPL::MCParticleImpl* mcp) {
	if (trj->getCreatedInSimulation()) {
		mcp->setGeneratorStatus(0);
		mcp->setCreatedInSimulation(true);
	} else {
		mcp->setCreatedInSimulation(false);
	}
}

void LcioMcpStatusSetter::setFinalStatusFlags(Trajectory* trj, IMPL::MCParticleImpl* mcp) {
	// set final status flags
	Trajectory::EFinalStatus trjFinalStatus = trj->getFinalStatus();
	switch (trjFinalStatus) {

	case Trajectory::eDecayedInTracker:
		mcp->setDecayedInTracker(true);
		break;

	case Trajectory::eLeftDetector:
		mcp->setHasLeftDetector(true);
		break;

	case Trajectory::eStopped:
		mcp->setStopped(true);
		break;

	case Trajectory::eDecayedInCalorimeter:
		mcp->setDecayedInCalorimeter(true);
		break;

	case Trajectory::eUnset:
		break;

	default:
		break;
	}
}

void LcioMcpStatusSetter::setGeneratorStatus(MCParticle* mcpInit, MCParticleImpl* mcp) {
	// set generator status if initial exists
	if (mcpInit) {

		// if init particle, this was not created in sim
		mcp->setCreatedInSimulation(false);

		// copy gen status from initial
		mcp->setGeneratorStatus(mcpInit->getGeneratorStatus());
	}
	// no initial -> created in sim
	else {
		mcp->setCreatedInSimulation(true);
		mcp->setGeneratorStatus(0);
	}
}
}
