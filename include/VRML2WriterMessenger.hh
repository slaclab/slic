#ifndef SLIC_VRML2WRITERMESSENGER_HH_
#define SLIC_VRML2WRITERMESSENGER_HH_ 1

#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithAString;

namespace slic {

/**
 * @class VRML2WriterMessenger
 * @brief This is the G4UImessenger for the VRML2Writer class.
 */
class VRML2WriterMessenger: public G4UImessenger {

public:

    /**
     * Class constructor.
     */
	VRML2WriterMessenger();

	/**
	 * Class destructor.
	 */
	virtual ~VRML2WriterMessenger();

public:

	/**
	 * Execute macro command.
	 * @param[in] cmd The macro command.
	 * @param[in] newValues The command values.
	 */
	void SetNewValue(G4UIcommand* cmd, G4String newVals);

private:
	G4UIdirectory* m_vrmlDir;
	G4UIcmdWithAString* m_writeVrmlCmd;
};
}

#endif
