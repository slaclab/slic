#ifndef SLIC_MODULEREGISTRY_HH_
#define SLIC_MODULEREGISTRY_HH_ 1

// std
#include <map>
#include <string>

// slic
#include "Module.hh"
#include "Singleton.hh"

namespace slic {

/**
 * @class ModuleRegistry
 * @brief A central store for accessing Module objects.
 */
class ModuleRegistry: public Singleton<ModuleRegistry>, public Module {

public:

    /**
     * A map of names to Modules.
     */
    typedef std::map<std::string, Module*> ModuleMap;

public:

    /**
     * Class constructor.
     */
    ModuleRegistry();

    /**
     * Class destructor.
     */
    virtual ~ModuleRegistry();

public:

    /**
     * Register a module by its name.
     * @param[in] module The Module to register.
     */
    void registerModule(Module* module);

    /**
     * Find a module by its name.
     * @param[in] name The name of the Module to find.
     * @return The Module with matching name or null if not found.
     */
    Module* findModule(const std::string& name);

    /**
     * Remove a Module from the registry.
     * @param[in] module The Module to deregister.
     */
    void deregisterModule(Module* module);

    /**
     * Remove a Module from the registry by name.
     * @param[in] name The name of the Module to deregister.
     */
    void deregisterModule(const std::string& name);

    /** 
     * Print the list of registered modules to the log.
     */
    void print();

    /**
     * Delete all modules.
     */
    void deleteModules();

private:
    ModuleMap m_modules;
};
}

#endif
