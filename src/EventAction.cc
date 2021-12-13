#include "EventAction.hh"

// LCDD
#include "lcdd/detectors/CurrentTrackState.hh"
#include "lcdd/util/StringUtil.hh"

// SLIC
#include "EventMessenger.hh"
#include "EventSourceManager.hh"
#include "LcioManager.hh"
#include "MCParticleManager.hh"
#include "RunManager.hh"
#include "TrackManager.hh"

// Geant4
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"

namespace slic {

EventAction::EventAction()
    : G4UserEventAction(), Module("EventAction", false),
      m_enableEventTimer(false) {
  m_messenger = new EventMessenger();
}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event *anEvent) {
  // Reset current track state.
  CurrentTrackState::setCurrentTrackID(-1);

  m_pluginManager->beginEvent(anEvent);
}

void EventAction::EndOfEventAction(const G4Event *anEvent) {

  /* Only execute these hooks if run was not aborted. */
  if (!RunManager::instance()->isRunAborted()) {

    /* Create an empty LCEvent. */
    LcioManager::instance()->createLCEvent();

    /* Save track information to MCParticle collection in LCIO output event. */
    TrackManager::instance()->saveTrackSummaries(
        anEvent, LcioManager::instance()->getCurrentLCEvent());

    /* End of event processing for the current event generator. */
    EventSourceManager::instance()->endEvent(anEvent);

    /* Execute LcioManager's end of event action. */
    LcioManager::instance()->endEvent(anEvent);
  }

  m_pluginManager->endEvent(anEvent);

  /* Stop the event timer. */
  stopEventTimer();

  /* Print the end event message. */
  printEndEventMessage(anEvent);
}

void EventAction::printEndEventMessage(const G4Event *anEvent) {
  log() << LOG::okay
        << ">>>> EndEvent <" + StringUtil::toString(anEvent->GetEventID()) + ">"
        << LOG::endl
        << LOG::done;
}

EventAction *EventAction::getEventAction() {
  const EventAction *ea = static_cast<const EventAction *>(
      G4RunManager::GetRunManager()->GetUserEventAction());
  return const_cast<EventAction *>(ea);
}

void EventAction::stopEventTimer() {
  if (m_enableEventTimer) {
    m_eventTimer.Stop();
#ifdef SLIC_LOG
    log() << LOG::okay << "Event Timer: " << m_eventTimer << LOG::done;
#endif
  }
}

void EventAction::startEventTimer() {
  if (m_enableEventTimer) {
    m_eventTimer.Start();
#ifdef SLIC_LOG
    log() << LOG::verbose << "Started Event Timer." << LOG::done;
#endif
  }
}

void EventAction::enableEventTimer(bool et) { m_enableEventTimer = et; }
} // namespace slic
