#ifndef SLIC_EVENTSOURCEMANAGER_HH_
#define SLIC_EVENTSOURCEMANAGER_HH_ 1

// SLIC
#include "Module.hh"
#include "EventSource.hh"
#include "EventSourceWithInputFile.hh"
#include "GPSEventSource.hh"
#include "ParticleGunEventSource.hh"
#include "GeneratorMessenger.hh"
#include "Singleton.hh"

// Geant4
#include "G4VPrimaryGenerator.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleGun.hh"
#include "G4UnitsTable.hh"

namespace slic {

/**
 * @class EventSourceManager
 * @brief This singleton is responsible for managing physics event sources.
 */
class EventSourceManager: public Module, public Singleton<EventSourceManager> {

public:

    /**
     * Enum for different source types.
     */
    enum ESourceType {
        eUnknown = 0, eStdHep = 1, eLCIO, eGPS, eParticleGun
    };

public:

    /**
     * Class destructor.
     */
    virtual ~EventSourceManager();

    /**
     * Class constructor.
     */
    EventSourceManager();

public:

    /**
     * Set the source type to use.
     * @param[in] est The event source type.
     */
    void setSourceType(ESourceType est);

    /**
     * Set the source type to use.
     * @param[in] est The event source type string.
     */
    void setSourceType(const std::string& s);

    /**
     * Get the current source type.
     * @return The current source type.
     */
    ESourceType getCurrentSourceType();

    /**
     * Determine the source type from a file extension.
     * @param[in] s The file extension.
     * @return The event source type.
     */
    ESourceType getSourceTypeFromFileExtension(const std::string& ext);

    /**
     * Get the name of the current source.
     * @return The name of the current source.
     */
    const std::string& getCurrentSourceName();

    /**
     * Get the source name string from a type.
     * @return egt The source name from the type.
     */
    const std::string& getSourceNameFromType(ESourceType egt) const;

    /**
     * Get the source type from a name.
     * @param[in] name The name of the source.
     * @return The source type.
     */
    ESourceType getSourceTypeFromName(const std::string& name) const;

    /**
     * Get the current file name.
     * @return The file name.
     */
    const std::string& getFilename();

    /**
     * Set the current file name.
     * @param[in] fileName The file name.
     */
    void setFilename(const std::string& fileName);

    /**
     * Setup the current event source.
     */
    void setupEventSource();

    /**
     * Reset the current event source.
     */
    void resetCurrentEventSource();

    /**
     * Set the number of events to skip.
     * @param[in] n The number of events to skip.
     */
    void setSkipEvents(unsigned int n);

    /**
     * Get the number of events to skip.
     * @return The number of events to skip.
     */
    unsigned int getSkipEvents();

    /**
     * Dump the current event.
     */
    void dumpCurrentEvent();

    /**
     * Print the number of events generated.
     */
    void printNumEventsGenerated();

    /**
     * Get the number of events generated.
     * @return The number of events generated.
     */
    int getNumEventsGenerated();

    /**
     * Increment the number of events generated.
     */
    void incrNumEventsGenerated();

    /**
     * Generate a Geant4 event.
     * @param[in] The target Geant4 event.
     */
    void generate(G4Event* event);

    /**
     * Set the current event source.
     * @param[in] eventSource The event source.
     */
    void setEventSource(EventSource* eventSource);

    /**
     * Delete the current event source.
     */
    void deleteCurrentEventSource();

    /**
     * Create an event source from a type.
     * @return The event source from the type.
     */
    EventSource* createEventSource(ESourceType st);

    /**
     * Get the current event source.
     * @return The current event source.
     */
    EventSource* getCurrentSource();

    /**
     * Setup an event source from a type.
     * @param[in] sourceType The event source type.
     */
    void setupEventSource(ESourceType sourceType);

    /**
     * Setup an event source from a type string.
     * @param[in] sourceType The event source type string.
     */
    void setupEventSource(const std::string& sourceType);

    /**
     * Begin event hook.
     * @param[in] event The Geant4 event.
     */
    void beginEvent(const G4Event* event);

    /**
     * End event hook.
     * @param[in] event The Geant4 event.
     */
    void endEvent(const G4Event* event);

    /**
     * Begin run hook.
     * @param[in] run The Geant4 run.
     */
    void beginRun(const G4Run* run);

    /**
     * End run hook.
     * @param[in] run The Geant4 run.
     */
    void endRun(const G4Run* run);

    /**
     * Check if end of file has been reached for file-based sources.
     * @return True if end of file has been reached; false if not.
     */
    bool isEOF();

    /**
     * Check if this is a new source which needs to be setup.
     * @return True if source is new for the run; false if not.
     */
    bool isNewSource();

    /**
     * Check if this is a new source by type.
     * @return True if source type is new for the run; false if not.
     */
    bool isNewSource(ESourceType est);

    /**
     * Check if the filename is different from the previous.
     * @return True if file name is new; false if not.
     */
    bool isNewFilename();

    /**
     * Check if source is file-based.
     * @return True if source is file-based; false if not.
     */
    bool isFileSource();

    /**
     * Get the source with input file.
     * @return The source with an input file.
     */
    EventSourceWithInputFile* getFileSource();

    /**
     * Get the GPS particle source.
     * @return The GPS particle source.
     */
    G4GeneralParticleSource* getGPS();

    /**
     * Get the particle gun source.
     * @return The particle gun source.
     */
    G4ParticleGun* getParticleGun();

    /**
     * Get the Lorentz transformation angle.
     * @return The Lorentz transformation angle.
     */
    double getLorentzTransformationAngle() {
        return m_lorentzTransformationAngle;
    }

    /**
     * Set the Lorentz transformation angle.
     * @param[in] angle The Lorentz transformation angle.
     */
    void setLorentzTransformationAngle(const G4double angle);

    /**
     * Set the Z smearing parameter.
     * @param[in] zSmear The Z smearing parameter.
     */
    void setZSmearing(const G4double zSmear) {
        m_zSmearingParam = zSmear;
        G4cout << "Set m_zSmearingParam = " << m_zSmearingParam << G4endl;
    }

    /**
     * Get the Z smearing parameter.
     * @return The Z smearing parameter.
     */
    G4double getZSmearing() {
        return m_zSmearingParam;
    }

    void enablePrintEvent(G4bool enable) {
        m_enablePrintEvent = enable;
    }

public:

    void generateNextEvent(G4Event* evt);

private:

    // generator messenger
    GeneratorMessenger* m_messenger;

    // current event source
    EventSource* m_currentEventSource;

    // ParticleGun source.  Always enabled.
    EventSource* m_particleGunSource;

    // filename
    std::string m_filename;
    bool m_fileIsSet;
    bool m_newFilename;

    // num events generated on current generator
    int m_ngen;

    // num events to skip
    unsigned int m_nskip;

    // need to setup a new source new run?
    bool m_newSource;

    // gen strings
    static std::string STDHEP;
    static std::string LCIO;
    static std::string GPS;
    static std::string PARTICLE_GUN;
    static std::string UNKNOWN;

    // source type
    ESourceType m_sourceType;

    // Lorentz transformation angle which will be applied to event.
    G4double m_lorentzTransformationAngle;

    // Parameter which will be used to smear vertex Z position.
    G4double m_zSmearingParam;

    G4bool m_enablePrintEvent;

};
}

#endif
