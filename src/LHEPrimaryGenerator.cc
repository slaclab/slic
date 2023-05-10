#include "LHEPrimaryGenerator.hh"

//~~ Geant4 ~~//
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

//#include "SimApplication/UserPrimaryParticleInformation.h"

namespace slic {

LHEPrimaryGenerator::LHEPrimaryGenerator(const std::string& name)
    : PrimaryGenerator(name) {
  reader_ = std::make_unique<LHEReader>();
}

void LHEPrimaryGenerator::GeneratePrimaryVertex(G4Event* event) {
  // Only get the next event if the regeneration flag hasn't been set. By
  // default, regenEvent will return false.
  if (!regenEvent()) {
    lhe_event_ = reader_->nextEvent();
  }

  // If the reader returns a nullptr, then end of the file has been reached.
  // At this point, the run should be aborted.
  if (lhe_event_ == nullptr) {
    // TODO Use a logger instead.
    std::cout << "[ LHEPrimaryGenerator ] : Ran out of input events so run "
                 "will be aborted!"
              << std::endl;
    G4RunManager::GetRunManager()->AbortRun(true);
    event->SetEventAborted();
  }

  auto vertex{new G4PrimaryVertex()};
  vertex->SetPosition(0, 0, 0);
  vertex->SetWeight(lhe_event_->getXWGTUP());

  std::map<LHEParticle*, G4PrimaryParticle*> particle_map;

  auto particles{lhe_event_->getParticles()};
  for (const auto& particle : particles) {
    if (particle->getISTUP() > 0) {
      auto primary{new G4PrimaryParticle()};
      if (particle->getIDUP() == -623) { // Tungsten ion 
        auto tungsten_ion{G4IonTable::GetIonTable()->GetIon(74, 184, 0.)};
        if (tungsten_ion != nullptr) {
          primary->SetParticleDefinition(tungsten_ion);
        } else {
          G4Exception("LHEPrimaryGenerator::GeneratePrimaryVertex",
                      "EventGenerationError", FatalException,
                      "Failed to find particle definition for W ion.");
        }
      } else {
        primary->SetPDGcode(particle->getIDUP());
      }

      primary->Set4Momentum(
          particle->getPUP(0) * GeV, particle->getPUP(1) * GeV,
          particle->getPUP(2) * GeV, particle->getPUP(3) * GeV);
      primary->SetProperTime(particle->getVTIMUP() * nanosecond);

      // UserPrimaryParticleInformation* primaryInfo = new
      // UserPrimaryParticleInformation();
      // primaryInfo->setHepEvtStatus(particle->getISTUP());
      // primary->SetUserInformation(primaryInfo);

      particle_map[particle] = primary;

      // Assign primary as daughter but only if the mother is not a DOC
      // particle.
      if (particle->getMother(0) != NULL &&
          particle->getMother(0)->getISTUP() > 0) {
        auto primary_mom{particle_map[particle->getMother(0)]};
        if (primary_mom != NULL) {
          primary_mom->SetDaughter(primary);
        }
      } else {
        vertex->SetPrimary(primary);
      }
    }
  }
  event->AddPrimaryVertex(vertex);
}
}  // namespace slic

DECLARE_GENERATOR(slic::LHEPrimaryGenerator)
