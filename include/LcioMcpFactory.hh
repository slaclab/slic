// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioMcpFactory.hh,v 1.11 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMCPFACTORY_HH
#define SLIC_LCIOMCPFACTORY_HH 1

// lcio
#include "EVENT/LCCollection.h"
#include "EVENT/LCEvent.h"
#include "EVENT/MCParticle.h"
#include "IMPL/MCParticleImpl.h"

// slic
#include "Module.hh"

class G4PrimaryParticle;
class G4TrajectoryContainer;
class G4Event;

namespace slic {
class LcioMcpManager;
class Trajectory;

/**
 * @class LcioMcpFactory
 * @brief Creates LCIO MCParticles from saved Geant4/SLIC runtime information.
 */
class LcioMcpFactory: public Module {

public:

    /**
     * Class constructor.
     * @param[in] manager The LcioMcpManager used for particle utilities.
     */
    LcioMcpFactory(LcioMcpManager* manager);

    /**
     * Class destructor.
     */
    virtual ~LcioMcpFactory();

public:

    /**
     * Create a new, empty MCParticle collection.
     * @param[in] collectionName The name of the collection.
     * @param[in] errorOnExists Whether to throw an error if the collection already exists (otherwise will overwrite).
     * @return The new, empty LCCollection.
     */
    EVENT::LCCollection* createEmptyMcpCollection(const std::string& collectionName, bool errorOnExist = true);

    /**
     * Create the final MCParticle output collection from the event.
     * @param[in] event The current Geant4 event.
     */
    void createFinalMcpCollection(const G4Event* event);

private:

    /**
     * Create the final MCParticle vector for an event using the MCParticle vector
     * from the StdHep input.
     * @param[in] mcpVecInitial The initial LCIO MCParticle collection.
     */
    void createFinalCollectionFromInitial(EVENT::LCCollection* mcpVecInitial);

    /**
     * Create the output MCParticle collection using trajectory data only.
     * @param[in] trajectories The trajectory container.
     */
    void createFinalCollectionFromTrajectories(G4TrajectoryContainer* trajectories);

    /**
     * Create the MCParticle daughters of a trajectory that has the given track ID.
     * @param[in] parentParticle The parent MCParticle.
     * @param[in] parentID The parent's track ID.
     */
    void createDaughters(IMPL::MCParticleImpl* parentParticle, int parentID);

    /**
     * Create an MCParticle from a trajectory only.
     * @param[in] The trajectory.
     * @return The new MCParticle.
     */
    IMPL::MCParticleImpl* fromTrajectory(Trajectory* trajectory);

    /**
     * Create MCParticles recursively given an initial starting particle.
     * @param[in] The start point for creating daughter particles recursively.
     * @return The initial top particle with daughters added.
     */
    IMPL::MCParticleImpl* fromInitialRecurse(EVENT::MCParticle* particle);

    /**
     * Create an MCParticle from an initial MCParticle, only.
     * @param[in] mcpInit The initial MCParticle.
     * @return The new MCParticle.
     */
    IMPL::MCParticleImpl* fromInitialOnly(EVENT::MCParticle* mcpInit);

    /**
     * Create an MCParticle from a G4PrimaryParticle.
     * @param[in] The input G4PrimaryParticle.
     * @param[in] The corresponding initial MCParticle.
     * @return The new MCParticle.
     */
    IMPL::MCParticleImpl* fromPrimary(G4PrimaryParticle* primary, EVENT::MCParticle* particle);

    /**
     * Create an MCParticle from a G4PrimaryParticle and associated, initial MCParticle.
     * @param[in] primary The input G4PrimaryParticle.
     * @param[in] initial The associated initial MCParticle.
     * @return The new MCParticle.
     */
    IMPL::MCParticleImpl* fromInitialAndPrimary(G4PrimaryParticle* primary, EVENT::MCParticle* initial);

    /**
     * Create an MCParticle from a trajectory and an input MCParticle.
     * @param[in] trajectory The G4Trajectory.
     * @param[in] initial The initial, associated MCParticle.
     * @return The new MCParticle.
     */
    IMPL::MCParticleImpl* fromInitialAndTrajectory(Trajectory* trajectory, EVENT::MCParticle* initial);

    /**
     * Create an MCParticle from a G4PrimaryParticle via shallow copy, e.g. daughters are not created automatically.
     * @param[in] The G4PrimaryParticle.
     * @return The new MCParticle.
     */
    IMPL::MCParticleImpl* fromPrimaryShallowCopy(G4PrimaryParticle* primary);

    /**
     * Shallow copy an MCParticle.
     * @param[in] particle The MCParticle to copy.
     * @return The new MCParticle.
     */
    IMPL::MCParticleImpl* shallowCopy(EVENT::MCParticle* particle);

    /**
     * Create the daughter MCParticles from the G4PrimaryParticle and initial MCParticle
     * and add them to the parent.
     * @param[in] primary The starting G4PrimaryParticle.
     * @param[in] initial The associated, initial MCParticle.
     */
    void daughtersFromPrimary(G4PrimaryParticle* primary, EVENT::MCParticle* initial, IMPL::MCParticleImpl* mcpPar);

    /**
     * Add daughters to an MCParticle based on the associated initial MCParticle.
     * @param[in] newParticle The new MCParticle to be updated.
     * @param[in] initialParticle The initial MCParticle containing daughter information.
     */
    void addDaughtersFromInitial(IMPL::MCParticleImpl* newParticle, EVENT::MCParticle* initialParticle);

    /**
     * Create an LCGenericObject collection with MCParticle endpoint energies.
     * The order will match that of the MCParticle collection.
     * @param[in] particleCollection The MCParticle collection.
     */
    void fillEndPointEnergy(IMPL::LCCollectionVec* particleCollection);

    /**
     * Copy spin and color flow information from one particle to another.
     * @param[in] sourceParticle The input MCParticle.
     * @param[out] targetParticle The output MCParticle with updated spin and color flow information.
     */
    void copySpinAndColorFlow(EVENT::MCParticle* sourceParticle, EVENT::MCParticle* targetParticle);

private:
    LcioMcpManager* m_manager;
    IMPL::LCCollectionVec* m_finalColl;
    G4TrajectoryContainer* m_currentTrajectoryContainer;
};
}

#endif
