// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioMcpMessenger.hh,v 1.12 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMCPMESSENGER_HH
#define SLIC_LCIOMCPMESSENGER_HH 1

// geant4
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"

namespace slic {

/**
 * @class LcioMcpMessenger
 * @brief G4UImessenger for LcioMcpManager.
 */
class LcioMcpMessenger: public G4UImessenger {

public:

    /**
     * Class constructor.
     */
	LcioMcpMessenger();

	/**
	 * Class destructor.
	 */
	virtual ~LcioMcpMessenger();

public:

	/**
	 * Execute a macro command.
	 * @param[in] command The command to execute.
	 * @param[in] newValues The arguments.
	 */
	virtual void SetNewValue(G4UIcommand* command, G4String newValues);

private:

	/**
	 * Define the commands handled by this messenger.
	 */
	void defineCommands();

private:
	G4UIdirectory* m_mcpDir;

	G4UIcmdWithADoubleAndUnit* m_minTrackingDistanceCmd;
	G4UIcmdWithABool* m_printFinalCollection;
	G4UIcmdWithABool* m_printInitialCollection;
	G4UIcmdWithABool* m_writeInitialCollection;
};
}

#endif
