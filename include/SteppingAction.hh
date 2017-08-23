#ifndef SLIC_STEPPINGACTION_HH_
#define SLIC_STEPPINGACTION_HH_ 1

// SLIC
#include "Module.hh"
#include "PluginManagerAccessor.hh"

// Geant4
#include "G4UserSteppingAction.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"

class UserRegionInformation;

namespace slic {

/**
 * @class SteppingAction
 * @brief Implementation of G4UserSteppingAction.
 */
class SteppingAction: public G4UserSteppingAction, public Module, public PluginManagerAccessor {

    public:

        /**
         * Class constructor.
         */
        SteppingAction();

        /**
         * Class destructor.
         */
        virtual ~SteppingAction();

        /**
         * Implementation of user stepping action.
         * @param[in] step The G4Step.
         */
        void UserSteppingAction(const G4Step* step);

        /**
         * Return true if this track appears to be backscattering from a non-tracking region
         * into the tracking region.
         * @return True if track is backscattering; false if not.
         */
        bool isBackScattering(const G4Step* step);

    private:

        /**
         * Kill a G4Track if it is inside a track-killing region.
         * @param[in] step The G4Step.
         */
        void checkKillTrack(const G4Step* step);
};
}

#endif
