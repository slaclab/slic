#include "TrackSummary.hh"

// SLIC
#include "TrackUtil.hh"
#include "TrackManager.hh"

// Geant4
#include "G4Track.hh"
#include "G4DecayProducts.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"

// LCIO
#include "EVENT/MCParticle.h"

using EVENT::MCParticle;
using EVENT::MCParticleVec;

namespace slic {

G4Allocator<TrackSummary> TrackSummaryAllocator;

TrackManager* TrackSummary::m_trackManager = TrackManager::instance();

TrackSummary::TrackSummary(const G4Track* track, G4bool toBeSaved, G4bool backScattering) :
        _parent(NULL), _toBeSaved(toBeSaved), _backScattering(backScattering), _mcparticle(NULL), _hasTrackerHit(false), _mcParticleIsUpToDate(false), _trackLength(0), _hepEvtStatus(0) {

    /* Set information of track that is immutable. */
    _charge = track->GetDefinition()->GetPDGCharge();
    _mass = track->GetDynamicParticle()->GetMass();
    _PDG = track->GetDefinition()->GetPDGEncoding();
    _trackID = track->GetTrackID();
    _parentTrackID = track->GetParentID();

    /* The momentum appears to be set correctly before tracking occurs so set here along with energy. */
    _momentum = track->GetMomentum();
    _energy = track->GetTotalEnergy();

    /* Assign track time. */
    // G4cout << "setting track time to " << track->GetGlobalTime() << G4endl;
    _globalTime = track->GetGlobalTime();
}

void TrackSummary::update(const G4Track* track) {

    //G4cout << "updating track " << this->_trackID << " from track " << track->GetTrackID() << G4endl;

    // TODO: Status flags to check; x means verified
    // x BITCreatedInSimulation
    // x BITBackscatter
    // x BITEndpoint (seems to be always turned on for sim particles)
    // x BITDecayedInTracker
    // x BITLeftDetector
    // x BITStopped
    // BITDecayedInCalorimeter (this is the default!)
    // BITVertexIsNotEndpointOfParent
    // BITOverlay

    /* Set the vertex position. */
    _vertex = track->GetVertexPosition();

    /* Set the end point. */
    _endPoint = track->GetPosition();

    /* Set the track length. */
    _trackLength = track->GetTrackLength();

    /* Set the generator status. */
    _hepEvtStatus = 0;
    if (_parentTrackID == 0) {
        const G4DecayProducts* preAssignedDecayProducts = track->GetDynamicParticle()->GetPreAssignedDecayProducts();
        if (preAssignedDecayProducts && preAssignedDecayProducts->entries() > 0)
            _hepEvtStatus = 2;
        else
            _hepEvtStatus = 1;
    }

    /* Initialize sim status. */
    _simstatus = 0;

    /* This indicates the particle has an endpoint. */
    _simstatus[MCParticle::BITEndpoint] = 1;

    /*
     * Get the last step point.
     */
    const G4Step* lastStep = track->GetStep();
    G4StepPoint* lastPostStepPoint = NULL;
    if (lastStep)
        lastPostStepPoint = lastStep->GetPostStepPoint();

    /* Check for back scattering. */
    if (_hepEvtStatus == 0) {
        if (getBackScattering())
            _simstatus[MCParticle::BITBackscatter] = 1;
    }

    /* Check if left detector. */
    G4bool leftDetector = false;
    if (lastPostStepPoint) {
        /* Tracks with the last step on the world boundary are flagged as LeftDetector. */
        if ((leftDetector = (lastPostStepPoint->GetStepStatus() == fWorldBoundary))) {
            _simstatus[MCParticle::BITLeftDetector] = 1;
        }
    }

    // BITStopped
    if (track->GetKineticEnergy() <= 0.)
        _simstatus[MCParticle::BITStopped] = 1;

    G4bool insideTrackerRegion = TrackUtil::getRegionInformation(track)->getStoreSecondaries();

    if (insideTrackerRegion)
        _simstatus[MCParticle::BITDecayedInTracker] = 1;

    // BITDecayedInCalorimeter
    // FIXME: This should not be the default.
    if (!insideTrackerRegion && !leftDetector)
        _simstatus[MCParticle::BITDecayedInCalorimeter] = 1;

    // BITVertexIsNotEndpointOfParent
    TrackSummary* myParent = findParent();
    // TODO: Check if parent is EVER available here.
    if (myParent)
        if (myParent->getEndPoint().isNear(getVertex()))
            _simstatus[MCParticle::BITVertexIsNotEndpointOfParent] = 1;

    // set momentum at endpoint from the current momentum
    _momentumAtEndpoint = track->GetMomentum();
}

void TrackSummary::setParentToBeSaved() {
    if (!(_mcparticle && _mcParticleIsUpToDate))
        buildMCParticle();
    else
        // In LCIO mode, if the theMCParticle exists we have already
        // passed by here.
        return;

    TrackSummary* myParent = findParent();
    /*
     if (myParent == this) {
     G4cout << "summary for " << this->getTrackID() << " is its own parent (probably a primary)" << G4endl;
     }
     */
    if (myParent) {

        // std::cout << " **** TrackSummary::SetParentToBeSaved for trackID: " << myParent->GetTrackID() << " this trackid: " << GetTrackID() << std::endl ;

        myParent->_toBeSaved = true;
        myParent->setParentToBeSaved();

        MCParticle* theParentMCParticle = myParent->getMCParticle();

        // fg: need to check if particle already has this parent assigned ...
        // this will be fixed in a future (>1.6) LCIO version
        const MCParticleVec& parents = _mcparticle->getParents();

        // I think this check for existing parent can probably be removed. --JM
        if (std::find(parents.begin(), parents.end(), theParentMCParticle) == parents.end()) {

            // fix B.Vormwald:  only add parent if particle created in simulation
            if (_mcparticle->getGeneratorStatus() == 0) {
                _mcparticle->addParent(theParentMCParticle);
            }
        }
        // DEBUG
    } else {
        if (this->getParentID() != 0)
            G4cout << "WARNING: no parent track ID " << this->getParentID() << " found for " << this->getTrackID() << G4endl;
        }
    }

TrackSummary* TrackSummary::findParent() const {

    TrackSummary* trackSummary = 0;
    if (_parentTrackID == 0) {
        return trackSummary;
    }

    return m_trackManager->findTrackSummary(_parentTrackID);
}

void TrackSummary::buildMCParticle() {
    // if we have an MCParticle already we just need to update it
    // otherwise we need to create one ...
    if (_mcparticle == 0) {
        _mcparticle = new MCParticleImpl;
        _simstatus[MCParticle::BITCreatedInSimulation] = 1;    // created in simulation
        _mcparticle->setGeneratorStatus(getHepEvtStatus());
        _mcparticle->setPDG(getPDG());
    }

    /* Set charge. */
    _mcparticle->setCharge(getCharge());

    /* Set endpoint. */
    double endpoint[3];
    endpoint[0] = getEndPoint()(0);
    endpoint[1] = getEndPoint()(1);
    endpoint[2] = getEndPoint()(2);
    _mcparticle->setEndpoint(endpoint);

    /* Set mass. */
    _mcparticle->setMass(_mass / GeV);

    /* Set simulator status. */
    _mcparticle->setSimulatorStatus(getSimulatorStatus());

    /* Set momentum. */
    float p[3];
    G4ThreeVector momentum = getMomentum();
    p[0] = momentum(0) / GeV;
    p[1] = momentum(1) / GeV;
    p[2] = momentum(2) / GeV;
    _mcparticle->setMomentum(p);

    /* Set vertex. */
    double vertex[3];
    vertex[0] = getVertex()(0);
    vertex[1] = getVertex()(1);
    vertex[2] = getVertex()(2);
    _mcparticle->setVertex(vertex);

    /* Set global time. */
    _mcparticle->setTime(_globalTime);

    /* Set momentum at endpoint. */
//#ifdef MCPARTICLE_HAS_MOMENTUMATENDPOINT
    float momentumAtEndpoint[3];
    momentumAtEndpoint[0] = _momentumAtEndpoint(0) / GeV;
    momentumAtEndpoint[1] = _momentumAtEndpoint(1) / GeV;
    momentumAtEndpoint[2] = _momentumAtEndpoint(2) / GeV;
    _mcparticle->setMomentumAtEndpoint(momentumAtEndpoint);
//#endif

    /* Set up to date. */
    _mcParticleIsUpToDate = true;
}

}
