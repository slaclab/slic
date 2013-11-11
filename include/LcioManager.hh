// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioManager.hh,v 1.62 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMANAGER_HH
#define SLIC_LCIOMANAGER_HH 1

// lcdd
#include "lcdd/detectors/SensitiveDetector.hh"
#include "lcdd/hits/CalorimeterHit.hh"
#include "lcdd/hits/TrackerHit.hh"

// slic
#include "LcioPrimaryGenerator.hh"
#include "LcioMcpManager.hh"
#include "LcioHitsCollectionBuilder.hh"
#include "LcioFileNamer.hh"
#include "Trajectory.hh"
#include "Singleton.hh"

// lcio
#include "IO/LCWriter.h"
#include "EVENT/MCParticle.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/MCParticleImpl.h"

// geant4
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"

// std
#include <string>
#include <map>

class G4Run;
class G4VHitsCollection;
class G4TrajectoryContainer;

namespace slic {

class LcioMessenger;

/**
 * @class LcioManager
 * @brief Singleton manager class of LCIO configuration and output file operations.
 * @note  This class is the top-level instantiator of most other LCIO-related managers
 *        and helper classes.
 */
class LcioManager: public Module, public Singleton<LcioManager> {

public:

    /**
     * Enum for setting what action to take when an output LCIO file already exists.
     */
    enum EFileExistsAction {
        eInvalid = -1, eFail = 0, eDelete = 1, eAppend = 2
    };

    /**
     * Typedef for run number.
     */
    typedef unsigned int RunNumberType;

public:

    /**
     * Class constructor.
     */
    LcioManager();

    /**
     * Class destructor.
     */
    virtual ~LcioManager();

public:

    /**
     * Open the output LCIO file.
     */
    void openLcioFile();

    /**
     * Set the output path.
     */
    void setPath(const std::string& path);

    /**
     * Set the output file name, which will be appended to the path.
     */
    void setFilename(const std::string& filename);

    /**
     * Set the list of fields to use for output file autonaming.
     */
    void setAutonameFields(const std::vector<std::string>& fields);

    /**
     * Get the output path for the LCIO files.
     * @return The output path for the LCIO files.
     */
    const std::string& getPath() const;

    /**
     * Get the output file name.
     * @return The output file path.
     */
    const std::string& getFilename() const;

    /**
     * Get the path plus the filename and optionally include the extension.
     * @param[in] withExtension Whether or not to include the file extension.
     * @return The full output file path.
     */
    std::string getFullOutputPath(bool withExtension = false);

    /**
     * Get the FileExists enum value from a string.
     * @param[in] fileExists The file exists string.
     * @return The file exists action from the string.
     */
    static EFileExistsAction getFileExistsActionFromString(const std::string& fileExists);

    /**
     * Set the current run number.
     * @param[in] runNumber The run number.
     */
    void setRunNumber(RunNumberType runNumber);

    /**
     * Begin of run hook.
     * @param[in] run The run.
     */
    void beginRun(const G4Run* run);

    /**
     * End of run hook.
     * @param[in] run The run.
     */
    void endRun(const G4Run* run);

    /**
     * Create the LCEvent from the G4Event.
     * @param[in] anEvent The input G4Event.
     * @return The output LCEvent.
     */
    IMPL::LCEventImpl* createLCEvent(const G4Event* anEvent);

    /**
     * Create the LCEvent from the current G4Event.
     * @return The LCEvent.
     */
    IMPL::LCEventImpl* createLCEvent();

    /**
     * Create the final output particle collction.
     */
    void createFinalMcpCollection();

    /**
     * Add a collection to the LCEvent.
     * @param[in] lcsevent The LCEvent.
     * @param[in] collection The collection.
     * @param[in] name The name of the collection.
     */
    void addCollection(EVENT::LCEvent* lcevent, EVENT::LCCollection* collection, const std::string& name);

    /**
     * Add a collection the current LCEvent.
     * @param[in] The collection.
     */
    void addCollection(EVENT::LCCollection* collection, const std::string& name);

    /**
     * Add the initial particle collection.
     */
    void addInitialMCParticleCollection();

    /**
     * End of event hook to write out an LCIO file.
     * @param[in] The physics event.
     */
    void endEvent(const G4Event* anEvent);

    /**
     * Set the current output LCIO event.
     * @param[in] anLCEvent The LCIO event.
     */
    void setCurrentLCEvent(IMPL::LCEventImpl* anLCEvent) {
        m_currentLCEvent = anLCEvent;
    }

    /**
     * Get the current LCEvent.
     * @return The current LCEvent.
     */
    IMPL::LCEventImpl* getCurrentLCEvent() {
        return m_currentLCEvent;
    }

    /**
     * Set whether to store momentum in output hits.
     * @param[in] s Setting for storing momentum in output hits.
     */
    void setStoreMomentum(bool s = true) {
        m_HCBuilder->setStoreMomentum(s);
    }

    /**
     * Set the file exists action.
     * @param[in] fea The file exists action.
     */
    void setFileExistsAction(EFileExistsAction fea) {
        if (fea == eInvalid) {
#ifdef SLIC_LOG
            log() << LOG::error << "Invalid setting for FileExistsAction." << LOG::done;
#endif
            return;
        }

        m_fileExistsAction = fea;
    }

    /**
     * Get the file exists action.
     * @return The file exists action.
     */
    EFileExistsAction getFileExistsAction() {
        return m_fileExistsAction;
    }

    /**
     * Get the LCIO generator.
     * @return The LCIO generator.
     */
    inline LcioPrimaryGenerator* getGenerator() {
        return m_eventGenerator;
    }

    /**
     * Get the MCParticle manager.
     * @return The MCParticle manager.
     */
    inline LcioMcpManager* getMcpManager() {
        return m_mcpManager;
    }

    /**
     * Get the LcioHitsCollectionBuilder for creating output hit collections.
     * @return The LcioHitsCollectionBuilder.
     */
    inline LcioHitsCollectionBuilder* getHCBuilder() {
        return m_HCBuilder;
    }

    /**
     * Get the LcioFileNamer for automatically naming output files.
     * @return The LcioFileNamer for naming output files.
     */
    inline LcioFileNamer* getFileNamer() {
        return m_namer;
    }

    /**
     * Enable dump event output for debugging.
     * @param[in] The dump event setting.
     */
    void enableDumpEvent(bool p = true) {
        m_enableDumpEvent = p;
    }

private:

    /**
     * Make the LCIO output file name using the LcioFileNamer.
     */
    void makeAutoname();

    /**
     * Create the hits collections using the current set of objects.
     */
    void createHitsCollections();

    /**
     * Create the output LCWriter.
     */
    void createWriter();

    /**
     * Delete the current LCWriter.
     */
    void deleteWriter();

    /**
     * Create the run header from the Geant4 run.
     * @param[in] run The Geant4 run.
     */
    void createRunHeader(const G4Run* run);

    /**
     * Delete the current run header.
     */
    void deleteRunHeader();

    /**
     * Set the detector name in the current run header.
     */
    void setDetectorName();

    /**
     * Set the time stamp in the current event header.
     */
    void setEventTimeStamp();

    /**
     * Add the list of active subdetectors into the event header.
     */
    void addActiveSubdetectors();

    /**
     * Reset the instance variables of this class.
     */
    void reset();

private:

    // final Mcp collection with all info from McpInitial, Trajectories, Primaries
    IMPL::LCCollectionVec* m_McpFinalColl;

    // writer
    IO::LCWriter* m_writer;

    // run header
    IMPL::LCRunHeaderImpl* m_runHdr;

    // file info
    std::string m_filename;
    std::string m_path;

    // Mcp manager
    LcioMcpManager* m_mcpManager;

    // event generator from MCP Coll
    LcioPrimaryGenerator* m_eventGenerator;

    // creation of HCs
    LcioHitsCollectionBuilder* m_HCBuilder;

    // current LCEvent
    IMPL::LCEventImpl* m_currentLCEvent;

    // messenger
    LcioMessenger* m_messenger;

    // action when file exists
    EFileExistsAction m_fileExistsAction;

    // starting run number
    RunNumberType m_runNumber;

    LcioFileNamer* m_namer;

    static std::string m_defaultFileName;
    std::vector<std::string> m_currentAutonameFields;

    bool m_enableDumpEvent;
    bool m_writerIsOpen;
    bool m_usingAutoname;
    //bool m_abortCurrentRun;
};
}

#endif
