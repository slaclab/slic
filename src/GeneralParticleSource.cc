#include "GeneralParticleSource.hh"

//~~ Geant4 ~~//
#include "G4Event.hh"

namespace slic {

void GeneralParticleSource::GeneratePrimaryVertex(G4Event* event) {
  gps_.GeneratePrimaryVertex(event);
}

}  // namespace slic

DECLARE_GENERATOR(slic::GeneralParticleSource)
