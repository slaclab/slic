// $Id: LogMessenger.hh,v 1.12 2013-11-06 00:23:35 jeremy Exp $
#ifndef LOG_MESSENGER_HH
#define LOG_MESSENGER_HH 1

// geant4
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

namespace slic {
class LogStream;

/**
 * @class LogMessenger
 * @brief G4UImessenger assigned to a LogStream.
 */
class LogMessenger: public G4UImessenger {
public:

    /**
     * Class constructor.
     * @param[in] logstream The LogStream.
     */
	LogMessenger(LogStream* logstream);

	/**
	 * Class destructor.
	 */
	virtual ~LogMessenger();

public:

	/**
	 * Execute macro command.
	 * @param[in] command The macro command.
	 * @param[in] newValues The argument values.
	 */
	void SetNewValue(G4UIcommand* command, G4String newValues);

private:

	/*
	 * Define commands handled by this messenger.
	 */
	void defineCommands();

private:
	LogStream* m_logStream;

	G4UIcmdWithAnInteger* m_setLevelCmd;
	G4UIcmdWithAString* m_fileCmd;
	G4UIdirectory* m_loggerDir;
};
}

#endif
