// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioMcpStatusSetter.hh,v 1.7 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMCPSTATUSSETTER_HH
#define SLIC_LCIOMCPSTATUSSETTER_HH 1

// lcio
#include "EVENT/MCParticle.h"
#include "IMPL/MCParticleImpl.h"

namespace slic {
class Trajectory;

/**
 * @class LcioMcpStatusSetter
 * @brief Static utility functions for setting the status of MCParticles
 *        from associated Trajectory, initial MCParticle, and TrackInformation
 *        objects.
 */
class LcioMcpStatusSetter {

private:

    /**
     * Class constructor, which is private because this class is all static utility methods.
     */
	LcioMcpStatusSetter() {
	}

public:

	/**
	 * Set the status codes on an MCParticle from a Trajectory.
	 * @param[in] trajectory The particle Trajectory.
	 * @param[in] particle The MCParticle on which to set status codes.
	 * @return The same MCParticle with status codes set.
	 */
	static IMPL::MCParticleImpl* setStatusCodesFromTrajectory(Trajectory* trajectory, IMPL::MCParticleImpl* particle);

	/**
	 * Set the generator status on a final MCParticle from the associated initial.
	 * @param[in] initialParticle The initial MCParticle.
	 * @param[in,out] finalParticle The output MCParticle with generator status set.
	 */
	static void setGeneratorStatus(EVENT::MCParticle* initialParticle, IMPL::MCParticleImpl* finalParticle);

private:

    /**
     * Set the final flag values on an
     * @param[in] trajectory The Trajectory.
     * @param[in,out] particle The MCParticle on which statuses will be set.
     */
    static void setFinalStatusFlags(Trajectory* trajectory, IMPL::MCParticleImpl* particle);

	/**
	 * Set the vertex is not endpoint of parent flag on an MCParticle from Trajectory data.
	 * @param[in] trajectory The Trajectory.
	 * @param[in,out] particle The MCParticle.
	 */
	static void setVertexIsNotEndpointOfParentFlag(Trajectory* trajectory, IMPL::MCParticleImpl* particle);

	/**
	 * Set the created in simulation flag on an MCParticle from Trajectory data.
	 * @param[in] trajectory The Trajectory.
	 * @param[in,out] particle The MCParticle.
	 */
	static void setCreatedInSimulationFlag(Trajectory* trajectory, IMPL::MCParticleImpl* particle);

	/**
	 * Set the backscatter flag on an MCParticle from Trajectory data.
	 * @param[in] trajectory The Trajectory.
	 * @param[in] particle The MCParticle.
	 */
	static void setBackscatterFlag(Trajectory* trajectory, IMPL::MCParticleImpl* particle);
};
}

#endif
