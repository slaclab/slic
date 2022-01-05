#include "PairConvFilter.hh"

//------------//
//   Geant4   //
//------------//
#include "G4RunManager.hh"

namespace slic {

G4ClassificationOfNewTrack PairConvFilter::stackingClassifyNewTrack(
    const G4Track *track, const G4ClassificationOfNewTrack &currentTrackClass) {

  if (track == currentTrack_) {
    currentTrack_ = nullptr;
    //std::cout << "[ PairConvFilter ]: Pushing track to waiting stack."
    //          << std::endl;
    return fWaiting;
  }

  // Use current classification by default so values from other plugins are
  // not overridden.
  return currentTrackClass;
}

void PairConvFilter::stepping(const G4Step *step) {

  if (hasPairConv_) return;

  // Get the track associated with this step.
  auto track{step->GetTrack()};

  // Get the particle type.
  auto particleName{track->GetParticleDefinition()->GetParticleName()};

  // Get the kinetic energy of the particle.
  auto incidentParticleEnergy{step->GetPostStepPoint()->GetTotalEnergy()};

  auto pdgID{track->GetParticleDefinition()->GetPDGEncoding()};

  // Get the volume the particle is in.
  auto volume{track->GetVolume()};
  auto volumeName{volume->GetName()};

  /*
  std::cout << "*******************************" << std::endl;
  std::cout << "*   Step " << track->GetCurrentStepNumber() << std::endl;
  std::cout << "********************************\n"
            << "\tTotal energy of " << particleName << " ( PDG ID: " << pdgID
            << " ) : " << incidentParticleEnergy << " MeV\n"
            << "\tTrack ID: " << track->GetTrackID() << "\n"
            << "\tParticle currently in " << volumeName << "\n"
            << "\tPost step process: "
            << step->GetPostStepPoint()->GetStepStatus() << std::endl;*/

  // Get the PDG ID of the track and make sure it's a photon. If another
  // particle type is found, push it to the waiting stack until the photon has
  // been processed.
  if (pdgID != 22) {
    currentTrack_ = track;
    track->SetTrackStatus(fSuspend);
    return;
  }

  // Only conversions that happen in the target, and layers 1-3 
  // of the tracker are of interest.  If the photon has propagated past
  // the second layer and didn't convert, kill the event.
  // TODO: OM: This really should be done with regions.
  if (volumeName.find("module_L4") != std::string::npos) {
    //std::cout << "[ PairConvFilter ]: Photon is beyond the sensitive"
    //          << " detectors of interest. Killing event." << std::endl;
    track->SetTrackStatus(fKillTrackAndSecondaries);
    G4RunManager::GetRunManager()->AbortEvent();
    return;
  } else if ((volumeName.find("module_L1") == std::string::npos) &&
             (volumeName.find("module_L2") == std::string::npos) && 
             (volumeName.find("module_L3") == std::string::npos)) {
    //std::cout << "[ PairConvFilter ]: Photon is not within sensitive "
    //          << " detectors of interest." << std::endl;
    return;
  }

  // Check if any secondaries were produced in the volume.
  const std::vector<const G4Track *> *secondaries =
      step->GetSecondaryInCurrentStep();

  //std::cout << "[ PairConvFilter ]: "
  //  << particleName  << " produced " << secondaries->size()
  //  << " secondaries." << std::endl;

  // If the particle didn't produce any secondaries, stop processing
  // the event.
  if (secondaries->size() == 0) {
    //std::cout << "[ PairConvFilter ]: "
    //  << "Primary did not produce secondaries!"
    //  << std::endl;
    return;
  }

  auto processName{(*secondaries)[0]->GetCreatorProcess()->GetProcessName()};
  if (processName.compareTo("conv") == 0) {
    hasPairConv_ = true;
    ++pairConvCount_; 
    std::cout << "[ PairConvFilter ]: "
              << "WAB converted in " << volumeName << std::endl;

  } else {
    track->SetTrackStatus(fKillTrackAndSecondaries);
    G4RunManager::GetRunManager()->AbortEvent();
    return;
  }
}

void PairConvFilter::endEvent(const G4Event *) { hasPairConv_ = false; }

void PairConvFilter::endRun(const G4Run* run) { 
  std::cout << "[ PairConvFilter ]: " 
            << "Total number of pair conversions: " << pairConvCount_ 
            << std::endl;
}

} // namespace slic

DECLARE_PLUGIN(slic, PairConvFilter)
