// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMcpFactory.cc,v 1.25 2013-11-06 00:23:37 jeremy Exp $
#include "LcioMcpFactory.hh"

// slic
#include "EventSourceManager.hh"
#include "LcioManager.hh"
#include "LcioMcpManager.hh"
#include "LcioMcpStatusSetter.hh"
#include "LcioMcpUtil.hh"
#include "Trajectory.hh"
#include "TrajectoryManager.hh"

// geant4
#include "globals.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4SystemOfUnits.hh"

// lcio
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCGenericObjectImpl.h"

using IMPL::LCCollectionVec;
using IMPL::LCGenericObjectImpl;
using IMPL::MCParticleImpl;
using EVENT::LCIO;
using EVENT::MCParticle;

namespace slic {
LcioMcpFactory::LcioMcpFactory(LcioMcpManager* manager) :
        Module("LcioMcpFactory"), m_manager(manager), m_finalColl(0), m_currentTrajectoryContainer(
                0) {
}

LcioMcpFactory::~LcioMcpFactory() {
}

void LcioMcpFactory::createFinalMcpCollection(const G4Event* event) {
    // new coll for final Mcps
    m_finalColl =
            static_cast<LCCollectionVec*>(m_manager->getFinalMcpCollection());

    // Set current G4TrajectoryContainer
    m_currentTrajectoryContainer = event->GetTrajectoryContainer();

    // Check if using StdHep or LCIO e.g. a file-based source?
    if (EventSourceManager::instance()->isFileSource()) {
        // Create from the initial coll of Mcp.
        createFinalCollectionFromInitial(
                m_manager->getInitialMcpCollection());
    }
    // G4 source using trajectories only.
    else {
        // Create from trajectory container of current G4Event.
        createFinalCollectionFromTrajectories(
                m_currentTrajectoryContainer);
    }

    // Fill supplementary collection with MCParticle endpoint energies.
    fillEndPointEnergy(m_finalColl);
}

IMPL::MCParticleImpl* LcioMcpFactory::fromTrajectory(Trajectory* trj) {
#ifdef SLIC_LOG
    log() << LOG::debug << "createMcpFromTrajectory() - trackId: " << trj->GetTrackID() << LOG::done;
    log() << LOG::debug << "trj PDG: " << trj->GetParticleDefinition()->GetPDGEncoding() << LOG::done;
#endif

    // new Mcp
    MCParticleImpl* mcp = new MCParticleImpl();

    // PDG
    mcp->setPDG(trj->GetPDGEncoding());

    // mom
    G4ThreeVector mom = trj->GetInitialMomentum() / GeV;
    float fmom[3] = { mom.x(), mom.y(), mom.z() };
    mcp->setMomentum(fmom);

    // Get particle def.
    const G4ParticleDefinition* pdef = trj->GetParticleDefinition();

    // To store mass for new particle.
    G4double m = 0;

    /*
     * For particles from extended particle table supplied by slic, use the StdHep mass instead.
     * Also, use the StdHep mass for PDG codes Geant4 doesn't know that aren't in particle.tbl.
     */
    if (pdef == 0
            || (pdef->GetParticleType().compare("extended") == 0
                    || pdef->GetParticleType().compare("susy") == 0)) {
        // Get the primary particle from the LCIO data map.
        MCParticle* primaryMcp =
                LcioMcpManager::instance()->getMaps()->findInitialParticle(
                        trj);

        // Require that this particle have a matching primary MCParticle to get mass.
        if (primaryMcp != 0) {
            m = primaryMcp->getMass();
#ifdef SLIC_LOG
            log() << "mass from primary: " << m << LOG::endl;
#endif
        }
        // Need a primary particle to get mass.
        else {
            G4Exception("", "", FatalException,
                    "Could not find required LCIO primary particle.");
        }
    }
    // Use Geant4's mass from the particle definition, which is appropriate for SM particles.
    else {
        m = trj->GetParticleDefinition()->GetPDGMass() / GeV;
#ifdef SLIC_LOG
        log() << "mass from pdef: " << m << LOG::endl;
#endif
    }

    // mass
    mcp->setMass(m);

    // charge
    mcp->setCharge(trj->GetCharge());

    // begin and end points
    G4int npts = trj->GetPointEntries();

    // begin pnt
    G4VTrajectoryPoint* beginTrjPnt = trj->GetPoint(0);
    G4ThreeVector beginPos = beginTrjPnt->GetPosition() / mm;
    double fBeginPos[3] = { beginPos.x(), beginPos.y(), beginPos.z() };
    mcp->setVertex(fBeginPos);

    // end pnt
    G4VTrajectoryPoint* endTrjPnt = trj->GetPoint(npts - 1);
    G4ThreeVector endPos = endTrjPnt->GetPosition() / mm;
    double fEndPos[3] = { endPos.x(), endPos.y(), endPos.z() };
    mcp->setEndpoint(fEndPos);

    // time
    mcp->setTime(trj->GetGlobalTime());

    // set status codes from trajectory info (with no dep on any associated Mcps)
    LcioMcpStatusSetter::setStatusCodesFromTrajectory(trj, mcp);

    // add to map of trkID -> mcp
#ifdef SLIC_LOG
    log() << LOG::debug << "adding trackId <" << trj->GetTrackID() << "> to MCP <" << mcp << "> link." << LOG::done;
#endif
    m_manager->getMaps()->addTrackToMCParticle(trj->GetTrackID(), mcp);

    return mcp;
}

// FIXME: This method doesn't seem to work correctly!
void LcioMcpFactory::createFinalCollectionFromTrajectories(
        G4TrajectoryContainer* m_currentTrajectoryContainer) {
    if (m_currentTrajectoryContainer) {
        // loop over trj cont
        int n_trj = m_currentTrajectoryContainer->entries();
        for (int i = 0; i < n_trj; i++) {
            Trajectory* trj =
                    static_cast<Trajectory*>((*m_currentTrajectoryContainer)[i]);

            // Only add primary trajectories; daughters are picked up within.
            if (trj->GetParentID() == 0) {
#ifdef SLIC_LOG
                log() << LOG::debug << "Making primary MCParticle for trajectory with trackID <" << trj->GetTrackID() << ">." << LOG::done;
#endif

                // Create MCParticle output from trajectory.
                MCParticleImpl* mcp = fromTrajectory(trj);

                // Add to the final MCParticle collection.
                m_finalColl->addElement(mcp);

                // Process the daughters.
#ifdef SLIC_LOG
                log() << LOG::debug << "Making primary MCParticle for trajectory with trackID <" << trj->GetTrackID() << ">." << LOG::done;
#endif
                createDaughters(mcp, trj->GetTrackID());
            }
        }
    } else {
        G4Exception("", "", FatalException, "G4TrajectoryContainer is null!");
    }

    // DEBUG
    LcioMcpManager::instance()->getMaps()->printTracksToParticles();

}

void LcioMcpFactory::createDaughters(
        MCParticleImpl* parMcp, int parTrkID) {
#ifdef SLIC_LOG
    log() << LOG::debug << "addMcpDaughtersFromTraj - parTrkId <" << parTrkID << ">." << LOG::done;
#endif

    // loop over trj cont
    int n_trj = m_currentTrajectoryContainer->entries();
    for (int i = 0; i < n_trj; i++) {
        Trajectory* trj =
                static_cast<Trajectory*>((*m_currentTrajectoryContainer)[i]);

        if (trj->GetParentID() == parTrkID) {

            // lkp dau Mcp by track ID
            MCParticleImpl* dauMcp = m_manager->getMaps()->findParticle(
                    trj->GetTrackID());

            if (dauMcp == 0) {
                dauMcp = fromTrajectory(trj);
            }

            if (dauMcp == 0)
                G4Exception("", "", FatalException,
                        "Failed to create MCParticle.");

            // add this to final coll
            m_finalColl->addElement(dauMcp);

            // Recursively add daughters of this daughter.
            createDaughters(dauMcp, trj->GetTrackID());

            // Set the parent particle in the daughter.
            dauMcp->addParent(parMcp);
        }
    }
}

void LcioMcpFactory::createFinalCollectionFromInitial(
        EVENT::LCCollection* mcpVecInitial) {
    // coll must exist
    if (mcpVecInitial) {
        // get num Mcp in coll
        int numInitMcp = mcpVecInitial->getNumberOfElements();

        // coll must have members
        if (numInitMcp > 0) {
            // loop over input Mcp coll
            for (int i = 0; i < numInitMcp; i++) {

#ifdef SLIC_LOG
                log() << LOG::debug << "proc initial MCP: " << i << LOG::done;
#endif        

                // get Mcp
                MCParticleImpl* mcp =
                        static_cast<MCParticleImpl*>(mcpVecInitial->getElementAt(
                                i));

                // create only from Mcp primaries, e.g. no parents
                if (LcioMcpUtil::isPrimary(mcp)) {
#ifdef SLIC_LOG
                    log() << LOG::debug << "isPrimary" << LOG::done;
#endif

                    fromInitialRecurse(mcp);
                }
            }
        } else {
            G4Exception("", "", FatalException,
                    "Initial McpVec has no members.");
        }
    } else {
        G4Exception("", "", FatalException, "Initial McpVec ptr is null.");
    }
}

IMPL::MCParticleImpl* LcioMcpFactory::fromInitialRecurse(EVENT::MCParticle* mcpInit) {
#ifdef SLIC_LOG 
    log() << LOG::debug << "createMcpFromInitialRecurse: " << m_manager->getParticleIndex(m_manager->getInitialMcpCollection(), mcpInit) << LOG::done;
#endif

    MCParticleImpl* mcp = 0;

    // find associated primary
    G4PrimaryParticle* g4primary = m_manager->getMaps()->findPrimary(
            mcpInit);

    // no associated primary with this initial mcp?
    if (!g4primary) {
#ifdef SLIC_LOG
        log() << LOG::debug << "initialOnly" << LOG::done;
#endif

        // create from initial Mcp only
        mcp = fromInitialOnly(mcpInit);
    }
    // create from primary
    else {
#ifdef SLIC_LOG
        log() << LOG::debug << "fromPrimary" << LOG::done;
#endif

        // create it from primary
        mcp = fromPrimary(g4primary, mcpInit);
    }

    // return the output MCP
    return mcp;
}

IMPL::MCParticleImpl* LcioMcpFactory::fromInitialOnly(
        EVENT::MCParticle* mcpInit) {
    log() << LOG::debug << "createMcpFromInitialOnly: "
            << m_manager->getParticleIndex(
                    m_manager->getInitialMcpCollection(), mcpInit) << LOG::done;

    // copy shallow the initial
    MCParticleImpl* mcp = shallowCopy(mcpInit);

    // should add to Mcp map here?
    m_manager->getMaps()->addInitialParticleToFinal(mcpInit, mcp);

    // given initial, create and add daughters
    addDaughtersFromInitial(mcp, mcpInit);

    // add to final coll
    m_finalColl->addElement(mcp);

    return mcp;
}

IMPL::MCParticleImpl* LcioMcpFactory::fromPrimary(
        G4PrimaryParticle* primary, EVENT::MCParticle* mcpInit) {
#ifdef SLIC_LOG
    log() << LOG::done << "createMcpFromPrimary - MCP idx <" << m_manager->getParticleIndex(m_manager->getInitialMcpCollection(), mcpInit) << ">" << LOG::done;
#endif

    int trkID = primary->GetTrackID();

#ifdef SLIC_LOG
    if (trkID <= 0)
    {
        log() << "WARNING: trkID <" << trkID << "> looks bad" << LOG::done;
    }
    log() << LOG::done << "primary TID <" << trkID << ">" << LOG::done;
#endif

    MCParticleImpl* mcp = 0;
    Trajectory* trj = TrajectoryManager::instance()->findTrajectory(trkID);

    // No trajectory was created.
    if (!trj) {

#ifdef SLIC_LOG
        log() << LOG::debug << "initialAndPrimary" << LOG::done;
#endif

        // Recursively create particles from primary and initial MCParticles.
        mcp = fromInitialAndPrimary(primary, mcpInit);

    }
    // Found a trajectory.
    else {

#ifdef SLIC_LOG
        log() << LOG::debug << "initialAndTrajectory" << LOG::done;
#endif

        // Recursively create MCParticles from a trajectory and the initial MCParticle.
        mcp = fromInitialAndTrajectory(trj, mcpInit);

    }

    // Set generator status from initial particle.
    mcp->setGeneratorStatus(mcpInit->getGeneratorStatus());

    // Primary particles do not originate in the simulation.
    mcp->setCreatedInSimulation(false);

    // Link the input MCP to an output MCP.
    m_manager->getMaps()->addInitialParticleToFinal(mcpInit, mcp);

//#ifdef SLIC_LOG
//        log() << LOG::debug << "createMcpFromPrimary() - adding trackId <" << trj->GetTrackID() << "> to MCP <" << mcp << "> link." << LOG::done;
//#endif

// Link the track ID to the output particle.
    if (trkID > 0) {
        m_manager->getMaps()->addTrackToMCParticle(trkID, mcp);
    } else {
#ifdef SLIC_LOG
        log() << LOG::debug << "WARNING: not adding trkID <" << trkID << "> to map!" << LOG::done;
#endif
    }

//#ifdef SLIC_LOG
//	log() << LOG::done;
//#endif

    return mcp;
}

IMPL::MCParticleImpl* LcioMcpFactory::fromInitialAndPrimary(
        G4PrimaryParticle* primary, EVENT::MCParticle* mcpInit) {
    log() << LOG::done << "createMcpFromInitialAndPrimary: "
            << m_manager->getParticleIndex(
                    m_manager->getInitialMcpCollection(), mcpInit) << LOG::done;

    /*
     * Create new Mcp from primary particle,
     * shallow copy only (no recursion).
     */
    MCParticleImpl* mcpNew = fromPrimaryShallowCopy(primary);

    /* Get vertex from initial particle, because primary does not have it -- was immediately decayed */
    double vtx[3] = { mcpInit->getVertex()[0], mcpInit->getVertex()[1],
            mcpInit->getVertex()[2] };
    mcpNew->setVertex(vtx);

    /* setup primary's daughters */
    daughtersFromPrimary(primary, /* this primary */
    mcpInit, /* associated initial Mcp particle */
    mcpNew /* parent Mcp particle */
    );

#ifdef SLIC_DEBUG
    if (mcpNew->getDaughters().size() == 0)
    {
#ifdef SLIC_LOG
        log() << LOG::debug << "No Mcp daughters added." << LOG::done;
#endif
    }
#endif

    // Copy spin and color flow information if LCIO version is compatible.
#if LCIO_VERSION_GE(1, 60)
    copySpinAndColorFlow(mcpInit, mcpNew);
#endif

    // done with particle; add to coll
    m_finalColl->addElement(mcpNew);

    return mcpNew;
}

IMPL::MCParticleImpl* LcioMcpFactory::fromInitialAndTrajectory(
        Trajectory* trj, EVENT::MCParticle* mcpInit) {
#ifdef SLIC_LOG
    log() << LOG::done << "createMcpFromInitialAndTrajectory: " << m_manager->getParticleIndex(m_manager->getInitialMcpCollection(), mcpInit) << LOG::done;
#endif

    // shallow copy create with basic info and no daughters
    IMPL::MCParticleImpl* mcp = fromTrajectory(trj);

    // set generator status and created in sim flag
    LcioMcpStatusSetter::setGeneratorStatus(mcpInit, mcp);

    // loop over all trajectories to find trajectory daughter
    int numTrj = m_currentTrajectoryContainer->entries();

#ifdef SLIC_LOG
    log() << LOG::done << "nTrajectoryDau: " << numTrj << LOG::done;
#endif

//#ifdef SLIC_LOG
//        if ( thisTrj->GetParentID() == -1 )
//        {
//            log() << LOG::always << "The trajectory with trackID <" << trj->GetTrackID() << "> has a parent with trackID of -1 !" << LOG::done;
//        }
//#endif

    for (int j = 0; j < numTrj; j++) {

        Trajectory* thisTrj =
                static_cast<Trajectory*>((*m_currentTrajectoryContainer)[j]);

        // found daughter
        if (thisTrj->GetParentID() == trj->GetTrackID()) {
#ifdef SLIC_LOG
            log() << LOG::debug << "Trajectory with trackID <" << trj->GetTrackID() << "> has parent with trackID <" << thisTrj->GetParentID() << ">" << LOG::done;
#endif

            // find matching primary Mcp input particle
            MCParticle* mcpPrim =
                    m_manager->getMaps()->findInitialParticle(
                            thisTrj);

            // create MCP output particle from this input daughter
            MCParticleImpl* mcpDauNew = fromInitialAndTrajectory(
                    thisTrj, mcpPrim);

            // set parent link of new daughter
            mcpDauNew->addParent(mcp);
        }
        // DEBUG
        //else
        //{
        //    log() << LOG::done << "BAD NEWS!  Could not find Trajectory <" << j << "> with TID <" << trj->GetTrackID() << ">" << LOG::done;
        //}
        // END DEBUG
    }

    // add to collection
    m_finalColl->addElement(mcp);

    return mcp;
}

IMPL::MCParticleImpl* LcioMcpFactory::shallowCopy(
        EVENT::MCParticle* mcp) {
    IMPL::MCParticleImpl* mcpNew = new IMPL::MCParticleImpl();

    mcpNew->setPDG(mcp->getPDG());

    float p[3] = { mcp->getMomentum()[0], mcp->getMomentum()[1],
            mcp->getMomentum()[2] };

    mcpNew->setMomentum(p);
    mcpNew->setMass(mcp->getMass());

    double vtx[3] = { mcp->getVertex()[0], mcp->getVertex()[1],
            mcp->getVertex()[2] };
    mcpNew->setVertex(vtx);

    mcpNew->setGeneratorStatus(mcp->getGeneratorStatus());
    mcpNew->setSimulatorStatus(0);
    mcpNew->setCharge(mcp->getCharge());
    mcpNew->setTime(mcp->getTime());

    copySpinAndColorFlow(mcp, mcpNew);

    return mcpNew;
}

// FIXME: Need to pass along colorflow and spin in this method???
IMPL::MCParticleImpl* LcioMcpFactory::fromPrimaryShallowCopy(
        G4PrimaryParticle* primary) {
    // new MCP
    MCParticleImpl* mcp = new MCParticleImpl();

    // PDG
    mcp->setPDG(primary->GetPDGcode());

    // momentum
    G4ThreeVector pVec = primary->GetMomentum() / GeV;
    float p[3] = { pVec.x(), pVec.y(), pVec.z() };
    mcp->setMomentum(p);

    // mass
    mcp->setMass(primary->GetMass() / GeV);

#ifdef SLIC_LOG
    log() << "mass from primary: " << mcp->getMass() << LOG::endl;
#endif

    // FIXME: Wrong if G4PrimaryParticle actually has valid charge.  (Can it happen?)
    mcp->setCharge(LcioMcpManager::m_NAN);

    return mcp;
}

// create LCIO MCParticles from daughters, setting proper linkage
void LcioMcpFactory::daughtersFromPrimary(G4PrimaryParticle* primary, EVENT::MCParticle* initial, IMPL::MCParticleImpl* parentParticle) {

#ifdef SLIC_LOG
    log() << LOG::debug << "createDaughtersFromPrimary" << LOG::done;
    log() << LOG::debug << "primary: " << primary << LOG::done;
    log() << LOG::debug << "trkID: " << primary->GetTrackID() << LOG::done;
    log() << LOG::debug << "mcpInit: " << initial << LOG::done;
    log() << LOG::debug << "parentParticle: " << parentParticle << LOG::done;
#endif

    // loop over primary daughters
    G4PrimaryParticle* primaryDaughter = primary->GetDaughter();
    while (primaryDaughter) {

        // find input Mcp for the primary dau
        MCParticle* mcpDau = m_manager->getMaps()->findDaughterParticle(initial, primaryDaughter);

        // must find it or die
        if (0 == mcpDau) {
            G4Exception("", "", FatalException, "Mcp daughter was not found.");
        }

        // create dau Mcp
        MCParticleImpl* mcpDauNew = fromPrimary(primaryDaughter, mcpDau);

        // make mcpPar the parent
        mcpDauNew->addParent(parentParticle);

        // get next primary dau
        primaryDaughter = primaryDaughter->GetNext();
    }
}

void LcioMcpFactory::addDaughtersFromInitial(IMPL::MCParticleImpl* mcpNew,
        EVENT::MCParticle* mcpInit) {
#ifdef SLIC_LOG
    log() << LOG::debug << "addMcpDaughtersFromInitial: " << m_manager->getParticleIndex(m_manager->getInitialMcpCollection(), mcpInit) << LOG::done;
#endif

    int numDau = mcpInit->getDaughters().size();

    for (int i = 0; i < numDau; i++) {
        // child of input particle
        MCParticle* mcpChildInit = mcpInit->getDaughters()[i];

        // use a search function to avoid insert when null!
        MCParticleImpl* mcpChildFinal =
                m_manager->getMaps()->findFinalParticle(
                        mcpChildInit);

        // if not added already
        if (mcpChildFinal == 0) {
            // create daughter
            mcpChildFinal = fromInitialRecurse(mcpChildInit);
        }

        // add parent regardless of above; maybe new MCP or already created
        mcpChildFinal->addParent(mcpNew);
    }
}

void LcioMcpFactory::fillEndPointEnergy(LCCollectionVec* mcpColl) {
    /* Add endpoint energies to LCGenericObject collection. */
    LcioMcpMaps* maps = LcioMcpManager::instance()->getMaps();
    LCCollectionVec* epColl = new LCCollectionVec(LCIO::LCGENERICOBJECT);
    for (LCCollectionVec::iterator it = mcpColl->begin(); it != mcpColl->end(); it++) {

        MCParticle* mcp = static_cast<MCParticle*>(*it);
        G4int trkID = maps->findTrack(mcp);
        double epE = -1.0;
        if (trkID != -1) {
            Trajectory* trj = TrajectoryManager::instance()->findTrajectory(
                    trkID);

            if (0 != trj) {
                epE = trj->getEndPointEnergy();
            }
        }

        LCGenericObjectImpl* obj = new LCGenericObjectImpl();
        obj->setFloatVal(0, epE);
        epColl->push_back(obj);
    }
    LcioManager::instance()->getCurrentLCEvent()->addCollection(epColl, "MCParticleEndPointEnergy");
}

void LcioMcpFactory::copySpinAndColorFlow(MCParticle* mcp1, MCParticle* mcp2) {
    // Set spin.
    const float* spin1 = mcp1->getSpin();
    float spin2[3];
    spin2[0] = spin1[0];
    spin2[1] = spin1[1];
    spin2[2] = spin1[2];
    ((IMPL::MCParticleImpl*) mcp2)->setSpin(spin2);

    // Set color flow.
    const int* cf1 = mcp1->getColorFlow();
    int cf2[2];
    cf2[0] = cf1[0];
    cf2[1] = cf1[1];
    ((IMPL::MCParticleImpl*) mcp2)->setColorFlow(cf2);
}
}
