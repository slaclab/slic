// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioMcpManager.hh,v 1.57 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMCPMANAGER_HH
#define SLIC_LCIOMCPMANAGER_HH 1

// slic
#include "Module.hh"
#include "LcioMcpMaps.hh"
#include "LcioPrimaryGenerator.hh"
#include "Singleton.hh"

//#include "MCParticleMap.hh"

// lcio
#include "EVENT/LCCollection.h"
#include "EVENT/LCEvent.h"
#include "EVENT/MCParticle.h"
#include "IMPL/MCParticleImpl.h"

// geant4
#include "globals.hh"
#include "G4PrimaryParticle.hh"

// std
#include <string>

class G4Event;
class G4TrajectoryContainer;

namespace slic {

class LcioMcpMessenger;
class LcioMcpFactory;
class LcioMcpPrinter;
class Trajectory;

/**
 * @class LcioMcpManager
 * @brief This singleton manages creation of the LCIO MCParticle output collection from transient
 *        G4Event and slic data.
 */
class LcioMcpManager: public Module, public Singleton<LcioMcpManager> {

public:
    // Mcp collections
    typedef std::map<std::string, EVENT::LCCollection*> McpCollectionMap;

public:
    virtual ~LcioMcpManager();
    LcioMcpManager();

public:

    /**
     * End of event hook.
     * @param[in] anEvent The event.
     */
    void endEvent(const G4Event* anEvent);

    /**
     * Get the MCParticle collection generated that was generated from an input StdHep event.
     * This will create an empty collection if it does not exist.
     * @return The MCParticle collection from the input StdHep event.
     */
    EVENT::LCCollection* getInitialMcpCollection();

    /**
     * Get the MCP collection generated at EndOfEvent.
     * Creates empty collection if doesn't exist.
     */
    EVENT::LCCollection* getFinalMcpCollection();

    /**
     * Set the minimum tracking distance for creating G4PrimaryParticles and G4PrimaryVertex objects
     * during event generation.
     * @param[in] minimumTrackingDistance The minimum tracking distance.
     */
    void setMinimumTrackingDistance(double minimumTrackingDistance);

    /**
     * Get the minimum tracking distance.
     * @return The minimum tracking distance.
     */
    double getMinimumTrackingDistance();

    /**
     * Reset the manager's state.  To be called after done processing a single event.
     */
    void reset();

    /**
     * Set the initial MCParticle collection created from event generation.
     * @param[in] initialCollection The initial MCParticle collection.
     */
    void setInitialMcpCollection(EVENT::LCCollection* initialCollection);

    /**
     * Register by name an LCCollection containing MCParticles.
     * @param[in] name The name of the collection.
     * @param[in] collection The collection to register.
     */
    void registerMcpCollection(const std::string& name, EVENT::LCCollection* collection);

    /**
     * Find the first MCParticle collection in the LCevent.
     * @param[in] event The LCEvent to search.
     * @return The found collection or null if not found.
     */
    EVENT::LCCollection* findParticleCollection(EVENT::LCEvent* event);

    /*
     * Find a registered MCParticle collection by name.
     * @param[in] name The name of the collection to find.
     * @return The found collection or null if not found.
     */
    EVENT::LCCollection* findParticleCollection(const std::string& name);

    /**
     * Using the LcioMcpPrinter, print out the named collection from the current event.
     * @param[in] name The name of the collection to print out.
     */
    void printMcpCollection(const std::string& name);

    /**
     * Using the LcioMcpPrinter, print out the named collection from the current event.
     * @param[in] name The name of the collection to print out.
     */
    void printMcpCollection(const std::string& collName, EVENT::LCCollection* coll);

    /**
     * Get the MCParticle data maps.
     * @return The MCParticle maps.
     */
    LcioMcpMaps* getMaps();

    /**
     * Enable printing of the final MCParticle collection.
     * @param[in] p The enable setting.
     */
    void enablePrintFinal(bool p = true);

    /**
     * Enable printing of the initial MCParticle collection.
     * @param[in] p The enable setting.
     */
    void enablePrintInitial(bool p = true);

    /**
     * Begin of event hook.
     * @param[in] anEvent The event.
     */
    void beginEvent(const G4Event* anEvent);

    /**
     * Set whether to write out the initial MCParticle collection to the event
     * as a supplementary collection.
     * @param[in] b Set to true in order to write this collection; false to not write.
     */
    inline void setWriteInitialMCParticleCollection(bool b) {
        m_writeInitialMCParticleCollection = b;
    }

    /**
     * Get whether to write out the intial MCParticle collection to the event.
     * @return True if initial MCParticle collection will be written; false if not.
     */
    inline bool writeInitialMCParticleCollection() {
        return m_writeInitialMCParticleCollection;
    }

    /**
     * Get the index of a particle in a collection or -1 if not present.
     * @param[in] collection The LCCollection to search.
     * @param[in] particle The MCParticle to find.
     * @return The index of the MCParticle or -1 if not found.
     */
    int getParticleIndex(EVENT::LCCollection* collection, EVENT::MCParticle* particle);

private:

    /**
     * Delete the current, initial MCParticle collection.
     */
    void deleteInitialMcpCollection();

    /*
     * Create an empty MCParticle collection indexed by name and register it in the map.
     * @param[in] name The name of the collection.
     * @return The new MCParticle collection.
     */
    EVENT::LCCollection* createParticleCollection(const std::string& name);

    // clear the maps
    void clearMaps();

private:

    // current LCEvent
    IMPL::LCEventImpl* m_currentLCEvent;

    // printer for MCP debugging
    LcioMcpPrinter* m_printer;

    // user settings
    double m_vertexIsNotEndpointOfParentTolerance;
    double m_minimumTrackingDistance;

    // G4 messenger for Mcp handling
    LcioMcpMessenger* m_messenger;

    // map of Mcp colls being tracked by the manager
    McpCollectionMap m_mcpColls;

    /* Factory for creating MCParticles from Geant4 runtime data. */
    LcioMcpFactory* m_factory;

    /* Collection of maps with MCParticle information. */
    LcioMcpMaps* m_maps;

    bool m_enablePrintFinal;
    bool m_enablePrintInitial;
    bool m_writeInitialMCParticleCollection;

public:
    static const double m_NAN;
    static const double DEFAULT_MIN_TRACKING_DISTANCE;
};
}

#endif
