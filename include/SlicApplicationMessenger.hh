// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/SlicApplicationMessenger.hh,v 1.3 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_SLICAPPLICATIONMESSENGER_HH
#define SLIC_SLICAPPLICATIONMESSENGER_HH 1

// geant4
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
