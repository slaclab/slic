#include "LcioHitsCollectionBuilder.hh"

// SLIC
#include "HitsCollectionUtil.hh"
#include "MCParticleManager.hh"
#include "SlicApplication.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// Geant4
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

// LCIO
#include "EVENT/MCParticle.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"

// LCIO
using IMPL::SimCalorimeterHitImpl;
using IMPL::SimTrackerHitImpl;
using IMPL::LCEventImpl;
using IMPL::LCFlagImpl;
using IMPL::LCCollectionVec;
using EVENT::LCIO;
using EVENT::MCParticle;

// STL
#include <iostream>

using std::string;

namespace slic {

LcioHitsCollectionBuilder::LcioHitsCollectionBuilder() :
        Module("LcioHitsCollectionBuilder"), m_storeMomentum(0) {

    // setup default coll flag for cal hits
    setFlagDefaults();

    // Set store momentum bit for TrackerHits
    m_trkCollFlag.setBit(LCIO::THBIT_MOMENTUM);
}

LcioHitsCollectionBuilder::~LcioHitsCollectionBuilder() {
}

// create the hit collections
void LcioHitsCollectionBuilder::createHitCollections() {

    // fetch HCIDs
    std::vector<int> hcids = HitsCollectionUtil::getHCIDs();

    // fetch hit collection of event
    G4HCofThisEvent* HCE = m_currentG4Event->GetHCofThisEvent();

    // HC table
    G4HCtable* HCtbl = G4SDManager::GetSDMpointer()->GetHCtable();

    // HCID
    G4int hcid;

    LCCollectionVec* collVec = 0;

    // Loop over HC IDs.
    for (std::vector<int>::const_iterator iter = hcids.begin(); iter != hcids.end(); iter++) {

        hcid = *iter;

        // retrieve Sensitive Detector ptr
        SensitiveDetector *SD = static_cast<SensitiveDetector*>(G4SDManager::GetSDMpointer()->FindSensitiveDetector(HCtbl->GetSDname(hcid)));

        // Get hits collections from SD.
        for (int i = 0; i < SD->getNumberOfHitsCollections(); i++) {
            if (SD->getHCID(i) == hcid) {
                G4VHitsCollection* HC = HCE->GetHC(hcid);

                // add a LCCollectionVec if got a HC
                if (HC) {
                    // set LCIO endcap bit in the flag (i.e. CHBIT_BARREL ) according to SDs setting
                    setEndcapFlag(SD);

                    // create collection vector based on type of SD
                    collVec = createCollectionVec(HC, SD->getType());

                    // Store the cellID description into the LCIO::cellIDEncoding parameter in the collection.
                    if (SD->getIdSpec()) {
                        std::string id = SD->getIdSpec()->getFieldDescription();
                        collVec->parameters().setValue(LCIO::CellIDEncoding, id);
                    }

                    // Check for existing collection.
                    if (containsCollection(m_currentLCEvent, HC->GetName())) {
                        // Update existing collection.
                        // TODO: Check for matching id scheme and flags!
                        LCCollectionVec * collection = (LCCollectionVec*) m_currentLCEvent->getCollection(HC->GetName());
                        collection->insert(collection->begin(), collVec->begin(), collVec->end());
                    }
                    // No collection found.
                    else {
                        // Add new collection vector to LCEvent.
                        m_currentLCEvent->addCollection(collVec, HC->GetName());

#ifdef SLIC_LOG
                        log() << LOG::always << HC->GetName() << " has " << collVec->size() << " hits" << LOG::done;
#endif
                    }
                }

                else {
                    G4Exception("LcioHitsCollectionBuilder::createHitCollections()", "", FatalException, "No collection found for Hits Collection ID");
                }
            }
        }
    }
}

// create the CollectionVec (decides which overloaded subfunction to call)
IMPL::LCCollectionVec* LcioHitsCollectionBuilder::createCollectionVec(G4VHitsCollection* g4HC, SensitiveDetector::EType SDtype) {

    // vec to create
    LCCollectionVec* collVec = 0;

    // cal hits
    if (SDtype == SensitiveDetector::eCalorimeter) {
        collVec = createCalorimeterCollectionVec(g4HC);
    }
    // tracker hits
    else if (SDtype == SensitiveDetector::eTracker) {
        collVec = createTrackerCollectionVec(g4HC);
    }
    // unknown type of hit
    else {
        G4Exception("", "", FatalException, "Unknown HC type.");
    }

    return collVec;
}

LCCollectionVec* LcioHitsCollectionBuilder::createTrackerCollectionVec(G4VHitsCollection* g4HC) {
    // create Lcio tracker coll
    LCCollectionVec* collVec = new LCCollectionVec(LCIO::SIMTRACKERHIT);

    // cast to G4 trk HC
    TrackerHitsCollection* trkHits = dynamic_cast<TrackerHitsCollection*>(g4HC);

    // call overloaded save function for trk hits
    saveHits(trkHits, collVec);

    // set trk flags
    collVec->setFlag(m_trkCollFlag.getFlag());

    return collVec;
}

LCCollectionVec* LcioHitsCollectionBuilder::createCalorimeterCollectionVec(G4VHitsCollection* g4HC) {
    // create Lcio cal coll
    LCCollectionVec* collVec = new LCCollectionVec(LCIO::SIMCALORIMETERHIT);

    // cast to G4 cal HC
    CalorimeterHitsCollection* calHits = dynamic_cast<CalorimeterHitsCollection*>(g4HC);

    // call overloaded save function for cal hits
    saveHits(calHits, collVec);

    // set cal flags
    collVec->setFlag(m_calCollFlag.getFlag());

    return collVec;
}

void LcioHitsCollectionBuilder::setFlagDefaults() {
    m_calCollFlag.setBit(LCIO::CHBIT_LONG);
    m_calCollFlag.setBit(LCIO::CHBIT_ID1);
}

void LcioHitsCollectionBuilder::setEndcapFlag(SensitiveDetector* g4sd) {
    bool ec_flag = g4sd->getEndcapFlag();

    // set for cal
    if (g4sd->getType() == SensitiveDetector::eCalorimeter) {
        if (ec_flag) {
            m_calCollFlag.unsetBit(LCIO::CHBIT_BARREL);
        } else {
            m_calCollFlag.setBit(LCIO::CHBIT_BARREL);
        }
    }
    // set for trk
    else if (g4sd->getType() == SensitiveDetector::eTracker) {
        if (ec_flag) {
            m_trkCollFlag.unsetBit(LCIO::THBIT_BARREL);
        } else {
            m_trkCollFlag.setBit(LCIO::THBIT_BARREL);
        }
    }
}

// save cal hits
void LcioHitsCollectionBuilder::saveHits(CalorimeterHitsCollection* calHits, IMPL::LCCollectionVec* lcioColl) {
    size_t s = calHits->GetSize();
    for (size_t i = 0; i < s; i++) {
        CalorimeterHit* calHit = static_cast<CalorimeterHit*>(calHits->GetHit(i));
        lcioColl->push_back(createHit(calHit));
    }
}

// save trk hits
void LcioHitsCollectionBuilder::saveHits(TrackerHitsCollection* trkHits, IMPL::LCCollectionVec* lcioColl) {
    size_t s = trkHits->GetSize();
    for (size_t i = 0; i < s; i++) {
        TrackerHit* trkHit = static_cast<TrackerHit*>(trkHits->GetHit(i));
        lcioColl->push_back(createHit(trkHit));
    }
}

// create cal hit from G4
IMPL::SimCalorimeterHitImpl* LcioHitsCollectionBuilder::createHit(CalorimeterHit* calHit) {
    SimCalorimeterHitImpl* simCalHit = new SimCalorimeterHitImpl();

    // set cellid from cal hit's id64
    const Id64bit& id64 = calHit->getId64bit();
    simCalHit->setCellID0(id64.getId0());
    simCalHit->setCellID1(id64.getId1());

    // position
    const G4ThreeVector hitPos = calHit->getPosition();
    float pos[3] = { hitPos.x(), hitPos.y(), hitPos.z() };
    simCalHit->setPosition(pos);

    //  copy Mcp contrib info; energy is also incremented by contrib addition
    addParticleContributions(calHit, simCalHit);

    // compare edep of calHit with simHit when debugging
//#ifdef SLIC_DEBUG
    const HitContributionList& contribs = calHit->getHitContributions();
    double totE = 0;
    for (HitContributionList::const_iterator iter = contribs.begin(); iter != contribs.end(); iter++) {
        totE += (*iter).getEdep();
    }

    // Set energy from total of individual contributions.    
    //simCalHit->setEnergy(totE/GeV);

    //std::cout << "set totE = " << totE/GeV << std::endl;
    // sanity check so that new and old edeps must match
    //if (abs( totE/GeV - simCalHit->getEnergy() ) > ( 0.001 * totE ) )
    //{
    //    log() << LOG::debug << "g4 hit E: " << totE << LOG::done;
    //    log() << LOG::debug << "sim hit E: " << simCalHit->getEnergy() << LOG::done;
    //    G4Exception("", "", JustWarning, "LCIO simCalHit E != G4 CalHit E, within tolerance");
    //}
//#endif

    return simCalHit;
}

// create trk hit from G4
IMPL::SimTrackerHitImpl* LcioHitsCollectionBuilder::createHit(TrackerHit* trackerHit) {

    SimTrackerHitImpl* simTrackerHit = new SimTrackerHitImpl();

    // position in mm
    const G4ThreeVector hitPos = trackerHit->getPosition();
    double pos[3] = { hitPos.x(), hitPos.y(), hitPos.z() };
    simTrackerHit->setPosition(pos);

    // momentum in GeV
    const G4ThreeVector& momentum = trackerHit->getMomentum();
    simTrackerHit->setMomentum(momentum.x() / GeV, momentum.y() / GeV, momentum.z() / GeV);

    // pathLength = distance between exit and entry points in mm
    simTrackerHit->setPathLength(trackerHit->getLength());

    // dEdx in GeV (LCIO units)
    float edep = trackerHit->getEdep();
    simTrackerHit->setEDep(edep / GeV);

    // time in NS
    float tEdep = trackerHit->getTdep();
    simTrackerHit->setTime(tEdep);

    // Cell ID.
    simTrackerHit->setCellID0(trackerHit->getId());

    // MCP using McpManager
    MCParticle* mcp = MCParticleManager::instance()->findMCParticle(trackerHit->getTrackID());

    if (!mcp) {
        log().error("No MCParticle found for trackID <" + StringUtil::toString(trackerHit->getTrackID()) + "> when processing SimTrackerHit");
    } else {
        simTrackerHit->setMCParticle(mcp);
    }

    return simTrackerHit;
}

// add an MCParticle hit contribution from G4 to LCIO
void LcioHitsCollectionBuilder::addParticleContributions(CalorimeterHit* g4CalHit, IMPL::SimCalorimeterHitImpl* simCalHit) {
    // Create empty hit contrib list.
    HitContributionList contribs;

    // Use aggregation of contribs by track ID if CHBIT_PDG is not set.
    if (!m_calCollFlag.bitSet(LCIO::CHBIT_PDG)) {
        // Pass a ref to contrib list, which will get filled.
        combineMcpHitContribs(g4CalHit->getHitContributions(), contribs);
    }
    // Otherwise, use the complete list from the CalHit.
    else {
        contribs = g4CalHit->getHitContributions();
    }

    // Add contribs to the LCIO MCParticle.
    size_t ncontrib = 0;
    for (HitContributionList::const_iterator iter = contribs.begin(); iter != contribs.end(); iter++) {
        // This contrib.
        const HitContribution& contrib = (*iter);

        // Get the MCParticle pointer from the track ID.
        MCParticle* contribMcp = MCParticleManager::instance()->findMCParticle(contrib.getTrackID());

        if (contribMcp != 0) {
            // Add the MCParticle contribution to the hit.
            simCalHit->addMCParticleContribution(contribMcp, (float) (contrib.getEdep() / GeV), (float) (contrib.getGlobalTime()), contrib.getPDGID(), const_cast<float*>(contrib.getPosition()));
            ++ncontrib;
        }
        // Problem!  Contributing particle is missing from MCParticle list.
#ifdef SLIC_LOG
        else
        {
            log() << LOG::error << "No MCParticle from track ID <" << contrib.getTrackID() << "> when processing SimCalorimeterHit" << LOG::endl;
        }
#endif
    }

#ifdef SLIC_LOG
    if ( ncontrib == 0 ) {
        log().error("No hit contributions for CalorimeterHit.");
    }
#endif
}

void LcioHitsCollectionBuilder::combineMcpHitContribs(const HitContributionList& longContrib, HitContributionList& combinedContrib) {
    combinedContrib.clear();

    // iterate over long list (one entry for every hit)
    for (HitContributionList::const_iterator iter = longContrib.begin(); iter != longContrib.end(); iter++) {
        int trkId = (*iter).getTrackID();

        //log().debug("Combining hits on trk_id: " + StringUtil::toString( trk_id ) );

        // old track id in new combined list?
        HitContribution* trk_contrib = 0;
        if ((trk_contrib = findHitContribution((*iter).getTrackID(), combinedContrib))) {
            // Add to the energy deposition.
            trk_contrib->incrEdep((*iter).getEdep());

            // Set the minimum time.
            trk_contrib->setMinTime((*iter).getGlobalTime());
        }
        // no existing contrib
        else {
            // Create a new contribution.
            combinedContrib.push_back(HitContribution(trkId, (*iter).getEdep(), (*iter).getPDGID(), (*iter).getGlobalTime()));
        }
    }
}

HitContribution* LcioHitsCollectionBuilder::findHitContribution(int trk_id, HitContributionList& contribs) {
    HitContribution* c = 0;
    for (HitContributionList::iterator iter = contribs.begin(); iter != contribs.end(); iter++) {
        if ((*iter).getTrackID() == trk_id) {
            c = &(*iter);
            break;
        }
    }

    return c;
}

EVENT::LCEvent* LcioHitsCollectionBuilder::createHitCollectionsFromEvent(const G4Event* g4evt, EVENT::LCEvent* lcevt) {
    // set instance vars
    m_currentG4Event = g4evt;
    m_currentLCEvent = lcevt;

    // call real HC creation function
    createHitCollections();

    // return evt pntr, which is same as input
    return m_currentLCEvent;
}

void LcioHitsCollectionBuilder::setLongFlag(bool setting) {
    if (setting) {
        m_calCollFlag.setBit(LCIO::CHBIT_LONG);
    } else {
        m_calCollFlag.unsetBit(LCIO::CHBIT_LONG);
    }

#ifdef SLIC_LOG
    log().verbose("Set CHBIT_LONG: " + StringUtil::toString( setting ) );
#endif
}

void LcioHitsCollectionBuilder::setPDGFlag(bool setting) {
    if (setting) {
        m_calCollFlag.setBit(LCIO::CHBIT_PDG);
    } else {
        m_calCollFlag.setBit(LCIO::CHBIT_PDG);
    }

#ifdef SLIC_LOG
    log().verbose("Set CHBIT_PDG: " + StringUtil::toString( setting ) );
#endif
}

bool LcioHitsCollectionBuilder::containsCollection(EVENT::LCEvent* event, const string& collectionName) {
    for (std::vector<string>::const_iterator iter = event->getCollectionNames()->begin(); iter != event->getCollectionNames()->end(); iter++) {
        const string thisName = *iter;
        if (thisName.compare(collectionName) == 0) {
            return true;
        }
    }
    return false;
}
} // namespace
