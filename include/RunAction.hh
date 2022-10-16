#ifndef SLIC_RUNACTION_HH_
#define SLIC_RUNACTION_HH_ 1

// slic
#include "Module.hh"
#include "PluginManagerAccessor.hh"

// geant4
#include "G4UserRunAction.hh"
#include "G4Timer.hh"

namespace slic {

/**
 * @class RunAction
 * @brief Implementation of G4UserRunAction.
 */
class RunAction: public G4UserRunAction, public Module, public PluginManagerAccessor {

    public:

        /**
         * Class constructor.
         */
        RunAction();

        /**
         * Class destructor.
         */
        ~RunAction();

    public:

        /**
         * Implementation of begin of run action.
         * @param[in] aRun The G4Run that is starting.
         */
        void BeginOfRunAction(const G4Run *aRun);

        /**
         * Implementation of end of run action.
         * @param[in] aRun The G4Run that is ending.
         */
        void EndOfRunAction(const G4Run *aRun);

    private:

        /**
         * Start the run timer.
         */
        void startRunTimer();

        /**
         * Stop the run timer.
         */
        void stopRunTimer();

        /**
         * Print the beginning of run message.
         * @param[in] aRun The G4Run that is starting.
         */
        //void printBeginOfRunMessage(const G4Run *aRun);

        /**
         * Print the end of run message.
         * @param[in] aRun The G4Run that is ending.
         */
        //void printEndOfRunMessage(const G4Run *aRun);

    private:

        G4Timer m_runTimer;
};
}

#endif
