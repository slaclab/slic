#ifndef SLIC_PHYSICSMESSENGER_HH_
#define SLIC_PHYSICSMESSENGER_HH_ 1

// geant4
#include "G4UImessenger.hh"

class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;
class G4UIdirectory;

namespace slic {

/**
 * @class PhysicsMessenger
 * @brief G4UImessenger for selection of physics list at runtime.
 * @note Also handles data for the HepPDT interface.
 */
class PhysicsMessenger: public G4UImessenger {

    public:

        /**
         * Class constructor.
         */
        PhysicsMessenger();

        /**
         * Class destructor.
         */
        virtual ~PhysicsMessenger();

    public:

        /**
         * Execute macro command.
         * @param[in] cmd The command to execute.
         * @param[in] newVals The command arguments.
         */
        virtual void SetNewValue(G4UIcommand* cmd, G4String newVals);

    private:

        /**
         * Define commands handled by this messenger.
         */
        void defineCommands();

    private:

        G4UIdirectory* m_physicsListDir;

        G4UIcommand* m_selectCmd;
        G4UIcmdWithoutParameter* m_printListsCmd;
        G4UIcmdWithoutParameter* m_printCurrentCmd;
        G4UIcmdWithABool* m_enableOpticalCmd;
        G4UIcommand* m_pdgCmd;
};
}

#endif
