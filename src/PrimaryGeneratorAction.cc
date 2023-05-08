// SLIC
#include "PrimaryGeneratorAction.hh"

#include "EventSourceManager.hh"
#include "G4SystemOfUnits.hh"
#include "LcioManager.hh"
#include "MCParticleManager.hh"
#include "RunManager.hh"
#include "SlicApplication.hh"
#include "TrackManager.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// LCIO
#include "EVENT/LCCollection.h"
#include "IMPL/MCParticleImpl.h"

namespace slic {

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : Module("PrimaryGeneratorAction", false) {
  _manager = EventSourceManager::instance();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  /* Print begin of event message. */
  // printBeginEventMessage(anEvent);

  /* Reset the TrackManager's state from previous event. */
  TrackManager::instance()->reset();

  /* Reset the MCParticleManager's state from previous event. */
  MCParticleManager::instance()->reset();

  /* Check if run needs to be aborted before generating events. */
  if (RunManager::instance()->isRunAborted()) {
    G4RunManager::GetRunManager()->AbortRun();
    log() << LOG::warning << LOG::name << LOG::sep
          << "Run was aborted.  Events will not be generated." << LOG::done;
  } else {
    /* Begin event hook of EventSourceManager which will read the next input
     * event if applicable. */
    _manager->beginEvent(anEvent);

    /* Generate primary vertices using the event generator's (previously read)
     * current event. */
    _manager->generateNextEvent(anEvent);

    // smear all primary vertices (if activated)
    auto nvtx{anEvent->GetNumberOfPrimaryVertex()};
    if (nvtx > 0) {
      // loop over all vertices generated
      for (int ivtx{0}; ivtx < nvtx; ++ivtx) {
        auto primary_vertex{anEvent->GetPrimaryVertex(ivtx)};

        auto x0_i{primary_vertex->GetX0()};
        auto y0_i{primary_vertex->GetY0()};
        auto z0_i{primary_vertex->GetZ0()};

        auto theta{0.0305};
        auto cos_theta{std::cos(theta)}; 
        auto sin_theta{std::sin(theta)}; 
        x0_i = x0_i * cos_theta + z0_i * sin_theta;
        z0_i = z0_i * cos_theta - x0_i * sin_theta;

        auto sigma_x{300 * um};
        auto sigma_y{30 * um};
        auto sigma_z{20 * um};
        auto x0_f = G4RandGauss::shoot(x0_i, sigma_x) + x0_i;
        auto y0_f = G4RandGauss::shoot(y0_i, sigma_y) + y0_i;
        auto z0_f{sigma_z * (G4UniformRand() - 0.5) + z0_i};
        primary_vertex->SetPosition(x0_f, y0_f, z0_f);

        for (int iparticle{0};
             iparticle < primary_vertex->GetNumberOfParticle(); ++iparticle) {
          auto primary{primary_vertex->GetPrimary(iparticle)};
          auto px{primary->GetMomentum().x() * cos_theta +
                  primary->GetMomentum().z() * sin_theta};
          auto pz{primary->GetMomentum().z() * cos_theta -
                  primary->GetMomentum().x() * sin_theta};
          primary->SetMomentum(px, primary->GetMomentum().y(), pz);
        }
      }
    }

    /* If the event source hit the end of file, then abort the run. */
    if (_manager->isEOF()) {
      SlicApplication::instance()->setAborting(true);
    }

    m_pluginManager->generatePrimary(anEvent);
  }

  /*void PrimaryGeneratorAction::printBeginEventMessage(G4Event* anEvent) {
      log() << LOG::okay << ">>>> BeginEvent <" <<
  StringUtil::toString(anEvent->GetEventID()) << ">" << LOG::done;
  }*/
}
}  // namespace slic
