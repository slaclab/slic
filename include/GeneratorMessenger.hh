// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/GeneratorMessenger.hh,v 1.12 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_GENERATORMESSENGER_HH
#define SLIC_GENERATORMESSENGER_HH 1

// geant4
#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;

namespace slic {

/**
 * @class GeneratorMessenger
 * @brief Geant4 commands related to event generation.
 */
class GeneratorMessenger: public G4UImessenger {

public:

    /**
     * Class constructor.
     */
    GeneratorMessenger();

    /**
     * Class destructor.
     */
    virtual ~GeneratorMessenger();

public:

    /**
     * Process a macro command.
     * @param[in] command The macro command.
     * @param[in] newValues The command arguments.
     */
    virtual void SetNewValue(G4UIcommand* command, G4String newValues);

private:

    /**
     * Define the commands of the messenger.
     */
    void defineCommands();

private:

    G4UIdirectory* m_generatorDir;
    G4UIcommand* m_randomSeedCmd;
    G4UIcommand* m_selectCmd;
    G4UIcommand* m_resetCmd;
    G4UIcmdWithAnInteger* m_skipEventsCmd;
    G4UIcommand* m_filenameCmd;
    G4UIcommand* m_dumpCurrentEventCmd;
    G4UIcommand* m_printNumEventsGeneratedCmd;
    G4UIcmdWithADoubleAndUnit* m_setLorentzTransformationAngleCmd;
    G4UIcmdWithADouble* m_setZSmearingParameterCmd;
};
}

#endif
