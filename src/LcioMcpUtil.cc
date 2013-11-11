// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMcpUtil.cc,v 1.4 2013-11-06 00:23:37 jeremy Exp $
#include "LcioMcpUtil.hh"

namespace slic {
// Mcp with no parents
bool LcioMcpUtil::isPrimary(IMPL::MCParticleImpl* mcp) {
	return (mcp->getParents().size() == 0);
}

G4double LcioMcpUtil::computeParticleDistance(IMPL::MCParticleImpl* mcp1, IMPL::MCParticleImpl* mcp2) {
	return computeDistance(mcp1->getVertex(), mcp2->getVertex());
}

G4double LcioMcpUtil::computeDistance(const double *v1, const double *v2) {
	// distance = sqrt ( x^2 + y^2 + z^2 )

	// x^2
	G4double x = (v1[0] - v2[0]);
	G4double x2 = pow(x, 2);

	// y^2
	G4double y = (v1[1] - v2[1]);
	G4double y2 = pow(y, 2);

	// z^2
	G4double z = (v1[2] - v2[2]);
	G4double z2 = pow(z, 2);

	return sqrt(x2 + y2 + z2);
}
}
