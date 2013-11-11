// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/StdHepToLcioConvertor.hh,v 1.23 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_STDHEPTOLCIOCONVERTOR_HH
#define SLIC_STDHEPTOLCIOCONVERTOR_HH 1

// lcdd
#include "Module.hh"

// lcio
#include "IMPL/MCParticleImpl.h"
#include "EVENT/LCCollection.h"
#include "UTIL/lStdHep.hh"

// std
#include <vector>
#include <cassert>

using UTIL::lStdHep;

namespace slic {

/**
 * @class StdHepToLcioConvertor
 * @brief Converts an input StdHep event to an LCIO MCParticle collection.
 */

/**
 @verbatim

 The index "pointers" daughter1, daughter2, mother1 and mother2 are handled as follows,
 where idx1 is daughter1/mother1, and idx2 is daughter2/mother2.

 idx1  cmp  idx2  action
 _______________________

 -1         -1    do nothing
 #          -1    use idx1
 -1          #    use idx2
 #     ==    #    use idx1
 #     <     #    range = idx1 to idx2
 #     >     #    use idx1 and idx2 as discrete indices

 @endverbatim
 */
class StdHepToLcioConvertor: public Module {

public:

    /**
     * Class constructor.
     * @param[in] reader The lStdHep to use.
     */
	StdHepToLcioConvertor(lStdHep* reader);

	/**
	 * Class constructor.
	 */
	StdHepToLcioConvertor();

	/**
	 * Class destructor.
	 */
	virtual ~StdHepToLcioConvertor();

public:

	/**
	 * Set the lStdHep reader to be used by the convertor.
	 * @param[in] rdr The lStdHep reader to use.
	 */
	void setStdHepReader(lStdHep* rdr) {
		assert(rdr);
		m_reader = rdr;
	}

	/**
	 * Create the next MCP collection using current event
	 * @note This function depends upon an external read operation to setup the current event.
	 */
	void fillParticleCollection();

	/**
	 * Reset the current MCParticle collection.
	 * @param[in] collection The new MCParticle collection.
	 */
	void setParticleCollection(EVENT::LCCollection* collection);

	/**
	 * Get the current MCParticle collection.
	 * @return The current MCParticle collection.
	 */
	EVENT::LCCollection* getParticleCollection();

	/**
	 * Create a single MCParticle from a StdHep record.
	 * @param[in] ihep The index of the StdHep record.
	 * @note Does not setup mother or daughter links.
	 *
	 */
	IMPL::MCParticleImpl* createParticle(int ihep);

	/**
	 * Setup the parent pointers of this MCParticle.
	 * @param[in] ihep The index of the StdHep record.
	 * @param[out] particle The MCParticle which will have its parent pointers setup.
	 */
	void setupParents(int ihep, IMPL::MCParticleImpl* particle);

	/**
	 * Assign parent MCParticle to the daugher from the index.
	 * @param[in] parentIndex The MCParticle index of the parent.
	 * @param[out] daughter The daughter particle.
	 */
	void addParent(int parentIndex, IMPL::MCParticleImpl* daughter);

	/**
	 * Assign the StdHep record at the index as a daughter track of the MCParticle.
	 * @param[in] ihep The StdHep record index.
	 * @param[out] particle The parent particle.
	 */
	void setupDaughters(int ihep, IMPL::MCParticleImpl* particle);

	/**
	 * Setup the input MCParticle as a parent of this track.
	 * @param[in] daughterIndex The index of the daughter.
	 * @param[in] parent The parent MCParticle.
	 */
	void addDaughter(int daughterIndex, IMPL::MCParticleImpl* parent);

	/**
	 * Check if an MCParticle has another particle as its parent.
	 * @param[in] daughter The proposed daughter MCParticle.
	 * @param[in] parent The parent MCParticle.
	 * @return True if daughter particle has this parent; false if not.
	 */
	bool hasParent(IMPL::MCParticleImpl* daughter, IMPL::MCParticleImpl* parent);

	/**
	 * Given two ihep indices, return a vector of all the included indices.
	 * @param[in] startIndex The starting index.
	 * @param[in] endIndex The ending index.
	 * @param[out] vec The vector of indices.
	 */
	std::vector<int> makeIndexVec(int startIndex, int endIndex, std::vector<int>& vec);

	/**
	 * Perform various sanity checks on the track's mother and daughter index values.
	 * @param[in] The StdHep record's index.
	 */
	void checkParentage(int ihep);

	/**
	 * Print the index.
	 * @param[in] ihep The index.
	 */
	void printIndex(int ihep);

	/**
	 * Print the mothers of this track.
	 * @param[in] ihep The index.
	 */
	void printMothers(int ihep);

	/**
	 * Print the daughters of this track.
	 * @param[in] ihep The index.
	 */
	void printDaughters(int ihep);

	/**
	 * Print the track.
	 * @param[in] The index.
	 */
	void printTrack(int ihep);

	/**
	 * Return true if index is basically valid, in that it is greater than zero
	 * and not greater than the total number of tracks in the event.
	 * @return True if daughter index is valid; false if not.
	 */
	bool isValidIndex(int ihep) {
		return (ihep > -1 && ihep < m_reader->nTracks());
	}

private:

	/* Pointer to the current MCParticle collection that is being filled. */
	EVENT::LCCollection* m_currentMcpColl;

	/* Pointer to the lStdHep reader being used as a data source. */
	lStdHep* m_reader;
};
}

#endif
