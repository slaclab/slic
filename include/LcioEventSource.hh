// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioEventSource.hh,v 1.10 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOEVENTSOURCE_HH
#define SLIC_LCIOEVENTSOURCE_HH 1

// slic
#include "EventSourceWithInputFile.hh"
#include "Module.hh"
#include "LcioMcpFilter.hh"
#include "LcioManager.hh"
#include "LcioMcpManager.hh"

// lcio
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "IOIMPL/LCFactory.h"
#include "IO/LCReader.h"

namespace slic {
/**
 * @class LcioEventSource
 * @brief Implements event generation from the MCParticle collection of an LCIO file.
 */
class LcioEventSource: public EventSourceWithInputFile {

public:

    /**
     * Class constructor.
     * @param[in] fname The file name.
     */
    LcioEventSource(const std::string& filename = "");

    /**
     * Class destructor.
     */
    virtual ~LcioEventSource();

public:

    /**
     * Open the current file.
     */
    void open();

    /**
     * Close the current file.
     */
    void close();

    /**
     * Read the next event.
     */
    void readNextEvent();

    /**
     * Dump the current event.
     */
    void dumpCurrentEvent();

    /**
     * Generate a Geant4 event.
     * @param[in] anEvent The target event.
     */
    void generate(G4Event* anEvent);

    /**
     * Begin run hook.
     * @param[in] aRun The run.
     */
    void beginRun(const G4Run* aRun);

    /**
     * Begin event hook.
     * @param[in] anEvent The event.
     */
    void beginEvent(const G4Event* anEvent);

    /**
     * Get the current MCParticle collection.
     * @return The current MCParticle collection.
     */
    EVENT::LCCollection* getCurrentMcpLCCollection();

private:
    LcioMcpFilter* m_filter;
    IO::LCReader* m_reader;
    EVENT::LCEvent* m_event;
};
}

#endif
