#include "HitsCollectionUtil.hh"

// LCDD
#include "lcdd/core/LCDDProcessor.hh"
#include "lcdd/detectors/SensitiveDetector.hh"

// Geant4
#include "G4SDManager.hh"
#include "G4HCtable.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

// STL
#include <vector>
#include <algorithm>

namespace slic {
std::vector<SensitiveDetector*> HitsCollectionUtil::getSensitiveDetectors() {
    G4SDManager* SDmgr = G4SDManager::GetSDMpointer();
    G4HCtable* HCtbl = SDmgr->GetHCtable();
    G4int num_entries = HCtbl->entries();

    std::vector<SensitiveDetector*> sds;

    for (G4int i = 0; i < num_entries; i++) {
        // get the name of this SD
        G4String sdname = HCtbl->GetSDname(i);
        // retrieve corresponding Sensitive Detector from Geant4 store and cast to LCDD type
        SensitiveDetector* sd = static_cast<SensitiveDetector*>(G4SDManager::GetSDMpointer()->FindSensitiveDetector(sdname));
        // Add, checking for dups.
        if (sd && std::find(sds.begin(), sds.end(), sd) == sds.end())
            sds.push_back(sd);
    }
    return sds;
}

std::vector<int> HitsCollectionUtil::getHCIDs() {
    std::vector<int> hcids;
    const std::vector<SensitiveDetector*>& sds = getSensitiveDetectors();
    for (std::vector<SensitiveDetector*>::const_iterator it = sds.begin(); it != sds.end(); it++) {
        SensitiveDetector* sd = (*it);
        for (int i = 0; i < sd->getNumberOfHitsCollections(); i++) {
            hcids.push_back(sd->getHCID(i));
        }
    }
    return hcids;
}

std::vector<std::string> HitsCollectionUtil::getHCNames() {
    std::vector<int> hcids = getHCIDs();

    const G4Event* currEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();

    G4HCofThisEvent* hce = currEvent->GetHCofThisEvent();

    std::vector<std::string> hcnames;

    if (currEvent) {

        G4VHitsCollection* hc = 0;

        for (std::vector<int>::const_iterator iter = hcids.begin(); iter != hcids.end(); iter++) {
            hc = hce->GetHC((*iter));
            hcnames.push_back(hc->GetName());
        }
    }

    return hcnames;
}
}

