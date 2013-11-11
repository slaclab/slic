// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioMcpUtil.hh,v 1.7 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMCPUTIL_HH
#define SLIC_LCIOMCPUTIL_HH

// lcio
#include "IMPL/MCParticleImpl.h"

// g4
#include "globals.hh"

namespace slic {

/**
 * @class LcioMcpUtil
 * @brief Static MCParticle utility functions.
 */
class LcioMcpUtil {

private:

    /**
     * Class constructor, which is private because this class is all static utility functions.
     */
	LcioMcpUtil() {
	}

public:

	/**
	 * Compute the distance between the starting vertices of two MCParticle objects.
	 * @param[in] particle1 The first MCParticle.
	 * @param[in] particle2 The second MCParticle.
	 * @return The distance in millimeters between the two particle's starting vertices.
	 */
	static G4double computeParticleDistance(IMPL::MCParticleImpl* particle1, IMPL::MCParticleImpl* particle2);

	/**
	 * Compute the distance between two points represented as double arrays.
	 * @param[in] v1 The first vertex.
	 * @param[in] v2 The second vertex.
	 * @return The distance in millimeters between the two vertices.
	 */
	static G4double computeDistance(const double *v1, const double *v2);

	/**
	 * Check if an MCParticle is a primary, e.g. it has no parent particle.
	 * @return True if MCParticle is primary; false if not.
	 */
	static bool isPrimary(IMPL::MCParticleImpl* mcp);
};
}

#endif
