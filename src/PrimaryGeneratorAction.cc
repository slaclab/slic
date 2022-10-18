#include "PrimaryGeneratorAction.hh"

// SLIC
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "MCParticleManager.hh"
#include "PrimaryGenerator.hh"
#include "RunManager.hh"
#include "TrackManager.hh"

namespace slic {

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction() {}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  /* Reset the TrackManager's state from previous event. */
  TrackManager::instance()->reset();

  /* Reset the MCParticleManager's state from previous event. */
  MCParticleManager::instance()->reset();

  /* Check if run needs to be aborted before generating events. */
  if (RunManager::instance()->isRunAborted()) {
    G4RunManager::GetRunManager()->AbortRun();
  } else {
    // Loop over all generators and generate the primary vertices.
    PrimaryGenerator::Factory::get().apply([event](const auto& generator) {
      generator->GeneratePrimaryVertex(event);
    });

    // smear all primary vertices (if activated)
    auto npv{event->GetNumberOfPrimaryVertex()};
    if (npv > 0) {
      // loop over all vertices generated
      for (auto pv{0}; pv < npv; ++pv) {
        auto primary_vertex{event->GetPrimaryVertex(pv)};

        // Loop over all particle associated with the primary vertex and
        // set the generator status to 1.
        for (int iparticle = 0;
             iparticle < primary_vertex->GetNumberOfParticle(); ++iparticle) {
          auto primary{primary_vertex->GetPrimary(iparticle)};

          //auto primary_info{dynamic_cast<UserPrimaryParticleInformation*>(
          //    primary->GetUserInformation())};
          //if (!primary_info) {
            // no user info defined
            //  ==> make a new one
            //primary_info = new UserPrimaryParticleInformation;
            //primary->SetUserInformation(primary_info);
          //}  // check if primaryinfo is defined

          //auto hep_status{primary_info->getHepEvtStatus()};
          //if (hep_status <= 0) {
            // undefined hep_status ==> set to 1
            //primary_info->setHepEvtStatus(1);
          //}  // check if hep_status defined

        }  // iparticle - loop over primary particles from this vertex

        // include the weight of this primary vertex in the event weight
        //event_info->incWeight(primary_vertex->GetWeight());

        // smear beamspot if it is turned on
        if (smear_) {
          auto x0_i{primary_vertex->GetX0()};
          auto y0_i{primary_vertex->GetY0()};
          auto z0_i{primary_vertex->GetZ0()};
          /*
           * G4UniformRand returns a number in [0,1]
           *  - we shift this range so that it is [-0.5,0.5]
           *  - multiply by the width to get [-0.5*size,0.5*size]
           *  - add the initial point (in case its off center) to get
           *    [init-0.5*size, init+0.5*size]
           */
          auto x0_f{beamspot_dx_ * (G4UniformRand() - 0.5) + x0_i};
          auto y0_f{beamspot_dy_ * (G4UniformRand() - 0.5) + y0_i};
          auto z0_f{beamspot_dz_ * (G4UniformRand() - 0.5) + z0_i};
          primary_vertex->SetPosition(x0_f, y0_f, z0_f);
        }
      }  // pv - loop over primary vertices
    } else {
      //EXCEPTION_RAISE(
      //    "NoPrimaries",
      //    "No primary vertices were produced by any of the generators.");
    }

    /* Begin event hook of EventSourceManager which will read the next input
     * event if applicable. */
    //_manager->beginEvent(event);

    /* Generate primary vertices using the event generator's (previously read)
     * current event. */
    //_manager->generateNextEvent(event);
  }

  /* If the event source hit the end of file, then abort the run. */
  // if (_manager->isEOF()) {
  //    SlicApplication::instance()->setAborting(true);
  //}

  // m_pluginManager->generatePrimary(event);
}
}  // namespace slic
