// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/RunManager.hh,v 1.18 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_RUNMANAGER_HH
#define SLIC_RUNMANAGER_HH 1

// geant4
#include "G4RunManager.hh"

// slic
#include "Module.hh"
#include "SlicApplication.hh"

namespace slic {
/**
 * @class RunManager
 * @brief Extension of G4RunManager.
 * @note  Initializes the default physics list setup using PhysicsListManager.
 */
class RunManager: public G4RunManager, public Module {

public:

    /**
     * Class constructor.
     */
    RunManager();

    /**
     * Class destructor.
     */
    virtual ~RunManager();

public:

    /**
     * Get the instance of RunManager.
     * @return The singleton RunManager.
     */
    static RunManager* instance() {
        return dynamic_cast<RunManager*>(G4RunManager::GetRunManager());
    }

    /**
     * Abort the current run.
     */
    void abortRun() {
        //std::cout << "RunManager.abortRun - run will be aborted!!!" << std::endl;
        log() << LOG::fatal << "Aborting run." << LOG::done;
        m_abortRun = true;
        // Set return code to generic error but don't override if error is already set.
        if (SlicApplication::instance()->getReturnCode() == SlicApplication::OKAY) {
            SlicApplication::instance()->setReturnCode(SlicApplication::UNKNOWN_ERROR);
        }
        G4RunManager::GetRunManager()->AbortRun(false);
    }

    /**
     * Abort the current run with an error code.
     * @param[in] errorCode The error code.
     */
    void abortRun(int errorCode) {
        log() << LOG::fatal << LOG::name << LOG::sep << "Aborting run with return code: " << errorCode << LOG::done;
        m_abortRun = true;
        SlicApplication::instance()->setReturnCode(errorCode);
        G4RunManager::GetRunManager()->AbortRun(false);
    }

    /**
     * Initialize this class.
     */
    void Initialize();

    /**
     * Get the number of events to run.
     * @return The number of events to run.
     */
    int getNumberOfEventsToRun();

    /**
     * Return true if run has been aborted.
     * @return True if run has been aborted; false if not.
     */
    bool isRunAborted();

protected:

    /**
     * This method overrides the Geant4 implementation.  The super class's method
     * is called but wrapped in code for application control flow.
     * @param[in] nevent The number of events to execute.
     * @param[in] macroFile A macro file to be executed after each event (not used by SLIC).
     * @param[in] nselect The number of events to wait before macro execution.
     */
    void BeamOn(G4int nevent, const char* macroFile = 0, G4int nselect = -1);

    /**
     * Initialize the current physics configuration.
     */
    void InitializePhysics();

    /**
     * Initialize the current geometry configuration.
     */
    void InitializeGeometry();

    //void DoEventLoop(G4int, const char*, G4int);

private:

    /**
     * Initialize the user actions defined by this application.
     */
    void initializeUserActions();

private:

    bool m_userActionsInitialized;
    int m_numberOfEventsToRun;
    bool m_abortRun;
};
}

#endif
