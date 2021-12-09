#include "PairConvFilter.hh"

//------------//
//   Geant4   //
//------------//
#include "G4RunManager.hh"

namespace slic {

void PairConvFilter::stepping(const G4Step *step) {

  if (hasPairConv_)
    return;

  // Get the track associated with this step.
  auto track{step->GetTrack()};

  // Get the PDG ID of the track and make sure it's a photon. If another
  // particle type is found, return.
  auto pdgID{track->GetParticleDefinition()->GetPDGEncoding()};
  if (pdgID != 22)
    return;

  // Get the volume the particle is in.
  auto volume{track->GetVolume()};
  auto volumeName{volume->GetName()};

  // Get the particle type.
  auto particleName{track->GetParticleDefinition()->GetParticleName()};

  // Get the kinetic energy of the particle.
  auto incidentParticleEnergy{step->GetPostStepPoint()->GetTotalEnergy()};

  /*
  std::cout << "*******************************" << std::endl;
  std::cout << "*   Step " << track->GetCurrentStepNumber() << std::endl;
  std::cout << "********************************" << std::endl;
  std::cout << "[ TargetBremFilter ]: " << "\n"
              << "\tTotal energy of " << particleName
             // << " ( PDG ID: " << pdgID
              << " ) : " << incidentParticleEnergy       << "\n"
              << "\tTrack ID: " << track->GetTrackID()     << "\n"
              << "\tStep #: " << track->GetCurrentStepNumber() << "\n"
              << "\tParticle currently in " << volumeName
              << "\tPost step process: " <<
  step->GetPostStepPoint()->GetStepStatus()
              << std::endl;
  */
  // Only conversions that happen in the target, first or second layers
  // of the tracker are of interest.  If the photon has propagated past
  // the second layer and didn't convert, kill the event.
  if (volumeName.find("module_L3") != std::string::npos) {
    /*std::cout << "[ PairConvFilter ]: Photon is beyond the sensitive"
              << " detectors of interest. Killing event." << std::endl;*/
    track->SetTrackStatus(fKillTrackAndSecondaries);
    G4RunManager::GetRunManager()->AbortEvent();
    return;
  } else if ((volumeName.find("module_L1") == std::string::npos) &&
             (volumeName.find("module_L2") == std::string::npos)) {
    /*std::cout << "[ PairConvFilter ]: Photon is not within sensitive "
              << " detectors of interest." << std::endl;*/
    return;
  }

  // Check if any secondaries were produced in the volume.
  const std::vector<const G4Track *> *secondaries =
      step->GetSecondaryInCurrentStep();

  /*std::cout << "[ PairConvFilter ]: "
            << particleName  << " produced " << secondaries->size()
            << " secondaries." << std::endl;*/

  // If the particle didn't produce any secondaries, stop processing
  // the event.
  if (secondaries->size() == 0) {
    /*std::cout << "[ PairConvFilter ]: "
                << "Primary did not produce secondaries!"
                << std::endl;*/
    return;
  }

  auto processName{(*secondaries)[0]->GetCreatorProcess()->GetProcessName()};
  if (processName.compareTo("conv") == 0) {
    hasPairConv_ = true;
    std::cout << "[ PairConvFilter ]: "
              << "WAB converted in " << volumeName << std::endl;

  } else {
    track->SetTrackStatus(fKillTrackAndSecondaries);
    G4RunManager::GetRunManager()->AbortEvent();
    return;
  }
}

void PairConvFilter::endEvent(const G4Event *) { hasPairConv_ = false; }

} // namespace slic

DECLARE_PLUGIN(slic, PairConvFilter)
