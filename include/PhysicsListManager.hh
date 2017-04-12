#ifndef SLIC_PHYSICSLISTMANAGER_HH_
#define SLIC_PHYSICSLISTMANAGER_HH_ 1

// lcdd
#include "Module.hh"

// slic
#include "PhysicsMessenger.hh"
#include "Singleton.hh"

// geant4
#include "G4VUserPhysicsList.hh"
#include "G4Decay.hh"

// std
#include <string>

class G4PhysListFactory;
class G4VUserPhysicsList;

namespace slic {

/**
 * @class PhysicsListManager
 * @brief Provides management of user physics lists.
 * @note  Current, primary function is selection of a list and .
 *        setting the default, which is LCPhys.
 */
class PhysicsListManager: public Module, public Singleton<PhysicsListManager> {

public:

    /**
     * The class constructor.
     */
    PhysicsListManager();

    /**
     * The class destructor.
     */
    virtual ~PhysicsListManager();

public:

    /**
     * Initialize the currently selected physics list.
     * This will also register option physics such as optical
     * and extended particle set.
     */
    G4VUserPhysicsList* createPhysicsList();

    /**
     * Set the name of the current physics list.
     * Only available in pre-init state.
     * @param[in] name The name of the physics list.
     */
    void setCurrentListName(const std::string& name);

    /**
     * Get the name of the current physics list.
     * @return The name of the current physics list.
     */
    inline const std::string& getCurrentListName() {
        return m_currentListName;
    }

    /**
     * Return true if the physics list has been initialized.
     * @return True if list has been initialized; false if not.
     */
    inline bool isInitialized() {
        return m_isInitialized;
    }

    /**
     * Enable physics limits.
     * @param[in] enableLimits True to enable limits; false to disable.
     */
    inline void enableLimits(bool enableLimits) {
        m_enableLimits = enableLimits;
    }

    /**
     * Return true if physics limits are enabled.
     * @return True if physics limits are enabled; false if not.
     */
    inline bool enableLimits() {
        return m_enableLimits;
    }

    /**
     * Return true if optical physics are enabled.
     * @return True if optical physics are enabled; false if not.
     */
    inline bool enableOptical() {
        return m_enableOptical;
    }

    /**
     * Enable optical physics.
     * @param[in] enableOptical Set to true to enable optical physics; false to disable.
     */
    inline void enableOptical(bool enableOptical) {
        m_enableOptical = enableOptical;
    }

    /**
     * Set up the user limits process.
     */
    void setupUserLimitsProcesses();

    /**
     * Print a list of available physics lists.
     */
    void printAvailablePhysicsLists();

private:

    /**
     * Setup SUSY physics.
     */
    void setupSUSY();

private:

    G4VUserPhysicsList* m_currentList;

    bool m_isInitialized;
    bool m_isFromDefault;
    bool m_enableLimits;
    bool m_setupLimits;
    bool m_enableOptical;

    PhysicsMessenger* m_messenger;

    std::string m_currentListName;
    static const std::string m_defaultListName;

    G4PhysListFactory *m_factory;
};
}

#endif
