/**
 * @file PluginManagerAccessor.h
 * @brief Mixin class for accessing the plugin manager
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef SLIC_PLUGINMANAGERACCESSOR_HH_
#define SLIC_PLUGINMANAGERACCESSOR_HH_

// LDMX
#include "PluginManager.hh"

namespace slic {

/**
 * @class PluginManagerAccessor
 * @brief Mixin for accessing the PluginManager
 *
 * @note
 * This class is used to assign a plugin manager to each of the user action classes.
 */
class PluginManagerAccessor {

    public:

        /**
         * Set the plugin manager pointer.
         * @param pluginManager Pointer to the plugin manager.
         */
        void setPluginManager(PluginManager* pluginManager) {
            m_pluginManager = pluginManager;
        }

        /**
         * Get the plugin manager.
         * @return The plugin manager.
         */
        PluginManager* getPluginManager() {
            return m_pluginManager;
        }

    protected:

        /* The plugin manager pointer; allow protected access for convenience of sub-classes. */
        PluginManager* m_pluginManager;
};

}

#endif
