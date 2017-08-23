#ifndef SLIC_MODULE_HH_
#define SLIC_MODULE_HH_ 1

// slic
#include "LogManager.hh"

// std
#include <string>

// geant4
#include "G4ClassificationOfNewTrack.hh"

class G4Event;
class G4Run;
class G4Step;
class G4Track;

namespace slic {

/**
 * @class Module
 * @brief Base class for slic code modules that provides common functionality,
 *        such as logging to an output stream.
 */
class Module {
    public:
        enum ModuleStatus {
            eOkay = 0, eNotInitialized = 1, eBadState = 2, eFatalError = 3
        };

    protected:

        /** 
         * Class constructor.  Only subclasses can access this.
         * @param[in] name The name of the Module.
         * @param[in] deleteOnExit Whether to call delete on this Module.
         * @param[in] autoRegister Whether to automatically register this Module with the ModuleRegistry.
         */
        Module(const std::string& name, bool deleteOnExit = true, bool autoRegister = true);

    public:

        /**
         * Class destructor.
         */
        virtual ~Module();

    public:

        /**
         * Get the name of the module.
         * @return The name of the module.
         */
        const std::string& getName() const {
            return m_name;
        }

        /**
         * Get the status of the module as a ModuleStatus enum.
         * @return The module's status.
         */
        ModuleStatus getStatus() const {
            return m_status;
        }

        /**
         * Perform initialization.
         */
        virtual void initialize() {
            ;
        }

        /**
         * Reset the state of the module.
         */
        virtual void reset() {
            ;
        }

        /**
         * Cleaup the module before deletion.
         */
        virtual void cleanup() {
            ;
        }

        /**
         * Get whether to delete the module.
         * @return Return true if the module should be deleted; false if not.
         */
        bool deleteOnExit() {
            return m_deleteOnExit;
        }

        /**
         * Get the LogStream for this Module.
         * @return The Module's LogStream.
         */
        LogStream& log() {
            return *m_log;
        }

        /**
         * Begin of event hook.
         * @param[in] event The G4Event.
         */
        virtual void beginEvent(const G4Event*) {
        }

        /**
         * End of event hook.
         * @param[in] event The G4Event.
         */
        virtual void endEvent(const G4Event*) {
        }

        /**
         * Begin of run hook.
         * @param[in] run The G4Run.
         */
        virtual void beginRun(const G4Run*) {
        }

        /**
         * End run hook.
         * @param[in] run The G4Run.
         */
        virtual void endRun(const G4Run*) {
        }

        /**
         * Pre-tracking hook called for new G4Track objects.
         * @param[in] track The G4Track.
         */
        virtual void preTracking(const G4Track*) {
        }

        /**
         * Post-tracking hook called before G4Track deletion.
         * @param[in] track The G4Track.
         */
        virtual void postTracking(const G4Track*) {
        }

        /**
         * Stepping hook called before a G4Step is executed.
         * @param[in] step The G4Step.
         */
        virtual void stepping(const G4Step*) {
        }

    public:
        ModuleStatus m_status;
        std::string m_name;
        LogStream* m_log;
        bool m_deleteOnExit;
};
}

#endif
