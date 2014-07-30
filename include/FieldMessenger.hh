#ifndef SLIC_FIELDMESSENGER_HH_
#define SLIC_FIELDMESSENGER_HH_ 1

// geant4
#include "G4UImessenger.hh"

class G4UIcmdWithADoubleAndUnit;
class G4UIcommand;
class G4UIdirectory;

namespace slic {
class FieldManager;

/**
 * @class FieldMessenger
 *
 * @brief G4UImessenger to G4FieldManager and related classes.
 *
 * @note
 * This messenger handles the following commands:
 *   - /field/selectStepper
 *   - /field/setDeltaOneStep
 *   - /field/setDeltaIntersection
 *
 * @note
 * For more information on these settings, see the
 * <a href="http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch04s03.html">Electromagnetic Field section</a>
 * of the Geant4 Application Developer's Guide.
 *
 */

class FieldMessenger: public G4UImessenger {

public:

    /**
     * Class constructor.
     */
    FieldMessenger(FieldManager* mgr);

    /**
     * Class destructor.
     */
    virtual ~FieldMessenger();

public:

    /**
     * Process macro commands.
     * @param[in] command The macro command.
     * @param[in] newValues The command arguments.
     */
    virtual void SetNewValue(G4UIcommand* command, G4String newValues);

private:

    /**
     * Define the messenger's commands.
     */
    void defineCommands();

private:

    FieldManager* m_mgr;
    G4UIdirectory* m_fieldDir;
    G4UIcommand* m_selectStepperCmd;
    G4UIcmdWithADoubleAndUnit* m_setDeltaOneStepCmd;
    G4UIcmdWithADoubleAndUnit* m_setDeltaIntersectionCmd;
};
}

#endif
