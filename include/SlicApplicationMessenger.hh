#ifndef SLIC_SLICAPPLICATIONMESSENGER_HH_
#define SLIC_SLICAPPLICATIONMESSENGER_HH_ 1

// Geant4
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

namespace slic {

class SlicApplication;

/**
 * @class SlicApplicationMessenger
 * @brief G4UImessenger for the SLIC application.
 */
class SlicApplicationMessenger: public G4UImessenger {
    public:

        /**
         * Class constructor.
         * @param[in] app The associated SlicApplication.
         */
        SlicApplicationMessenger(SlicApplication* app);

        /**
         * Class destructor.
         */
        virtual ~SlicApplicationMessenger();

    public:

        /**
         * Execute macro command.
         * @param[in] cmd The macro command.
         * @param[in] newVals The argument values.
         */
        virtual void SetNewValue(G4UIcommand* cmd, G4String newVals);

    private:

        /**
         * Define commands handled by this macro.
         */
        void defineCommands();

    private:

        SlicApplication* m_app;

        G4UIdirectory* m_slicDir;

        G4UIcommand* m_interactive;
        G4UIcommand* m_slicUsage;
        G4UIcommand* m_slicVersion;
};
}

#endif
