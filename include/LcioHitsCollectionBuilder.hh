#ifndef SLIC_LCIOHITSCOLLECTIONBUILDER_HH_
#define SLIC_LCIOHITSCOLLECTIONBUILDER_HH_ 1

// SLIC
#include "Module.hh"

// LCDD
#include "lcdd/detectors/SensitiveDetector.hh"
#include "lcdd/hits/CalorimeterHit.hh"
#include "lcdd/hits/TrackerHit.hh"

// LCIO
#include "EVENT/LCEvent.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/SimTrackerHitImpl.h"

// Geant4
#include "G4Event.hh"

namespace slic {

class LcioMcpManager;

/**
 @class LcioHitsCollectionBuilder
 @brief Builds Lcio hits collections from G4/SLIC event data.
 @note  Makes use of LcioMcpManager for Mcp hit contributions.
 */
class LcioHitsCollectionBuilder: public Module {

    public:

        /**
         * Class constructor.
         */
        LcioHitsCollectionBuilder();

        /**
         * Class destructor.
         */
        virtual ~LcioHitsCollectionBuilder();

        /**
         * Setup instance variables and then create the LCIO output hits collections
         * from the Geant4 sensitive detector data for the event.
         * @param[in] event The Geant4 event.
         * @param[in] lcioEvent The LCIO target event.
         * @return The LCIO event with the hits collections.
         */
        EVENT::LCEvent* createHitCollectionsFromEvent(const G4Event* event, EVENT::LCEvent* lcioEvent);

        /**
         * Set to true in order to include positions in output hits.
         * @param[in] longFlag The setting of the long flag.
         */
        void setLongFlag(bool longFlag = true);

        /**
         * Set to true in order to include all step contributions in output hits
         * rather than aggregating them together by PDG ID.
         * @param[in] pdgFlag The setting of the PDG flag.
         */
        void setPDGFlag(bool pdgFlag = true);

        /**
         * Set to true in order to store momentum in output step contributions.
         * @param[in] storeMomentum The store momentum setting.
         */
        void setStoreMomentum(bool storeMomentum = true) {
            m_storeMomentum = storeMomentum;
        }

        /**
         * Get whether to store momentum in output step contributions.
         * @return True if momentum should be stored; false if not.
         */
        bool getStoreMomentum() {
            return m_storeMomentum;
        }

    private:

        /*
         * Create the output LCIO hit collections from the current Geant4 event,
         * using the instance variables of this class.
         */
        void createHitCollections();

        /**
         * Create the LCCollectionVec for a given hits collection.
         * @param[in] hitCollection The hit collection.
         * @param[in] sensitiveDetectorType The type of the sensitive detector (e.g. tracker or calorimeter).
         */
        IMPL::LCCollectionVec* createCollectionVec(G4VHitsCollection* hitCollection, SensitiveDetector::EType sensitiveDetectorType);

        /**
         * Create an output collection of LCIO tracker hits.
         * @param[in] The input TrackerHit collection.
         */
        IMPL::LCCollectionVec* createTrackerCollectionVec(G4VHitsCollection* hitCollection);

        /**
         * Create an output collection of LCIO caloriemter hits.
         * @param[in] The input CalorimeterHit collection.
         */
        IMPL::LCCollectionVec* createCalorimeterCollectionVec(G4VHitsCollection* hitCollection);

        /**
         * Save the CalorimeterHits into the output LCIO collection.
         * @param[in]
         *
         */
        void saveHits(CalorimeterHitsCollection* calorimeterHits, IMPL::LCCollectionVec* collection);

        /**
         * Save TrackerHits to a collection.
         * @param[in] trackerHits The TrackerHits to add.
         * @param[out] collection The target collection.
         */
        void saveHits(TrackerHitsCollection* trackerHits, IMPL::LCCollectionVec* collection);

        /**
         * Create a SimCalorimeterHit from a CalorimeterHit.
         * @param[in] calorimeterHit The source CalorimeterHit.
         * @return The output SimCalorimeterHit.
         */
        IMPL::SimCalorimeterHitImpl* createHit(CalorimeterHit* calorimeterHit);

        /**
         * Create a SimTrackerHit from a TrackerHit.
         * @param[in] trackerHit The input TrackerHit.
         * @return The output SimTrackerHit.
         */
        IMPL::SimTrackerHitImpl* createHit(TrackerHit* trackerHit);

        /**
         * Copy MCParticle hit contributions from a CalorimeterHit to a SimCalorimeterHit.
         * @param[in] calorimeterHit The CalorimeterHit with the contributions.
         * @param[out] simCalorimeterHit The SimCalorimeterHit to update.
         */
        void addParticleContributions(CalorimeterHit* calorimeterHit, IMPL::SimCalorimeterHitImpl* simCalorimeterHit);

        /**
         * Set default CalorimeterHit flag settings.  This is called from this classe's constructor.
         */
        void setFlagDefaults();

        /**
         * Combine hit contributions using track IDs.
         * @param[in] allContributions The full hit contribution list.
         * @param[out] combinedContributions The hit contributions grouped by track ID.
         */
        void combineMcpHitContribs(const HitContributionList& allContributions, HitContributionList& combinedContributions);

        /**
         * Find a single hit contribution by track ID within the combined list.
         * @param[in] trackID The track ID.
         * @param[in] contributions The hit contribution list to search.
         * @return The matching HitContribution or null if there are none.
         */
        HitContribution* findHitContribution(int trackID, HitContributionList& contributions);

        /**
         * Set the endcap flag depending on the type of the sensitive detector.
         * @param[in] sensitiveDetector The SensitiveDetector.
         */
        void setEndcapFlag(SensitiveDetector* sensitiveDetector);

        /**
         * Check if an event contains a named collection.
         * @param[in] event The LCEvent to search.
         * @param[in] name The name of the collection.
         * @return True if the LCEvent contains a collection with given name; false if not.
         */
        static bool containsCollection(EVENT::LCEvent* event, const std::string& name);

    private:

        // current Lcio event
        EVENT::LCEvent* m_currentLCEvent;

        // current G4 event
        const G4Event* m_currentG4Event;

        // default flags
        IMPL::LCFlagImpl m_trkCollFlag;
        IMPL::LCFlagImpl m_calCollFlag;

        // store momentum
        bool m_storeMomentum;
};
}

#endif
