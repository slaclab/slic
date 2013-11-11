// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioMcpFilter.hh,v 1.9 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMCPFILTER_HH
#define SLIC_LCIOMCPFILTER_HH 1

// slic
#include "Module.hh"

// lcio
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "IMPL/MCParticleImpl.h"

namespace slic {
/**
 * @class LcioMcpFilter
 * @brief Filters simulation particles from an MCParticle collection.
 */
class LcioMcpFilter: public Module {

public:

    /**
     * Class constructor.
     */
    LcioMcpFilter();

    /**
     * Class destructor.
     */
    virtual ~LcioMcpFilter();

public:

    /**
     * Create a new MCParticle collection with only generator particles included.
     * @param[in] inputCollection The input MCParticle collection.
     * @return The filtered, output collection.
     */
    EVENT::LCCollection* filterSimParticles(EVENT::LCCollection* inputCollection);

    /**
     * Create a new MCParticle and add it to the target collection, unless it was already added.
     * @param[in] particle The input MCParticle to be copied.
     * @param[out] targetCollection The collection to be updated.
     * @return The new MCParticle.
     */
    IMPL::MCParticleImpl* makeParticle(EVENT::MCParticle* particle, EVENT::LCCollection* targetCollection);

    /**
     * Add simulation daughters from a parent particle to the target.
     * @param[out] targetParticle The target particle to update.
     * @param[in] parentParticle The source particle with the daughters.
     */
    void addSimDaughters(EVENT::MCParticle* targetParticle, EVENT::MCParticle* parentParticle, EVENT::LCCollection* mcpColl);

    /**
     * Check if a particle was already added to the list.
     * @return True if particle is already in list; false if not.
     */
    bool wasAdded(EVENT::MCParticle* particle);

private:
    std::vector<EVENT::MCParticle*> m_mcpList;
};
}

#endif
