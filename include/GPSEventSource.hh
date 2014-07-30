#ifndef SLIC_GPSEVENTSOURCE_HH_
#define SLIC_GPSEVENTSOURCE_HH_ 1

// SLIC
#include "EventSource.hh"

// Geant4
#include "G4GeneralParticleSource.hh"

namespace slic {

/**
 * @class GPSEventSource
 * @brief Wrapper for Geant4's general particle source.
 */
class GPSEventSource: public EventSource {

public:

    /**
     * Class constructor.
     */
    GPSEventSource();

    /**
     * Class destructor.
     */
    virtual ~GPSEventSource();

public:

    /**
     * Generate a Geant4 event.
     * @param[in] event The Geant4 event.
     */
    void generate(G4Event* event);

    /**
     * Dump the current event to std::cout.
     */
    void printCurrentEvent();

    /**
     * Get the Geant4 general particle source class.
     * @return The general particle source.
     */
    G4GeneralParticleSource* getGPS();

private:

    /**
     * Setup the default GPS event settings.
     */
    void setupGeneratorDefaults();

private:

    G4GeneralParticleSource* m_generator;

};
}

#endif
