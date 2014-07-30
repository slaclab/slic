#ifndef SLIC_LCIOMESSENGER_HH_
#define SLIC_LCIOMESSENGER_HH_ 1

// Geant4
#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIdirectory;

namespace slic {

class LcioManager;

/**
 * @class LcioMessenger
 * @brief G4UIcommand class for LCIO commands.
 */
class LcioMessenger: public G4UImessenger {
public:

    /**
     * Class constructor.
     */
	LcioMessenger(LcioManager* mgr);

	/**
	 * Class destructor.
	 */
	virtual ~LcioMessenger();

public:

	/**
	 * Execute a macro command.
	 * @param[in] command The macro command.
	 * @param[in] newValues The command arguments.
	 */
	virtual void SetNewValue(G4UIcommand* command, G4String newValues);

private:

	/**
	 * Define the commands handled by this messenger.
	 */
	void defineCommands();

private:

	LcioManager* m_mgr;

	G4UIdirectory* m_lcioDir;
	G4UIcommand* m_pathCmd;
	G4UIcommand* m_filenameCmd;
	G4UIcommand* m_verboseCmd;
	G4UIcommand* m_setLongFlagCmd;
	G4UIcommand* m_setPDGFlagCmd;
	G4UIcommand* m_fileExistsActionCmd;
	G4UIcommand* m_mcpFilenameCmd;
	G4UIcmdWithAnInteger* m_setRunNumberCmd;
	G4UIcommand* m_autonameCmd;
	G4UIcmdWithABool* m_printEventCmd;
	G4UIcmdWithABool* _writeCompleteEventCmd;
};
}

#endif
