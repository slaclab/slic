// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/LcioMcpPrinter.hh,v 1.21 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_LCIOMCPPRINTER_HH
#define SLIC_LCIOMCPPRINTER_HH 1

// LCIO
#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"

// SLIC
#include "LcioMcpManager.hh"

// STL
#include <map>

namespace slic {

/**
 * @brief Prints MCParticle information for debugging.
 */
class LcioMcpPrinter: public Module {
public:

    /**
     * List of indices.
     */
	typedef std::vector<int> IndexVector;

public:

	/**
	 * Class constructor.
	 */
	LcioMcpPrinter();

	/**
	 * Class destructor.
	 */
	virtual ~LcioMcpPrinter();

public:

	/**
	 * Print out an MCParticle collection.
	 * @param[in] name The name of the collection.
	 * @param[in] collection The LCCollection containing MCParticle objects.
	 */
	void printParticleCollection(const std::string& name, EVENT::LCCollection* collection);

private:

	/**
	 * Print current MCParticle collection.
	 */
	void printParticleCollection();

	/*
	 * Print information for a single MCParticle.
	 */
	void printParticle(EVENT::MCParticle* particle);

	/**
	 * Make a string from a double array of length 3.
	 * @param[in] array The double array.
	 * @return The string.
	 */
	static std::string makeDoubleArray3String(const double* array);

	/**
	 * Make a string from a float array of length 3.
	 * @param[in] array The float array.
	 * @return The string.
	 */
	static std::string makeFloatArray3String(const float* array);

	/**
	 * Make a string from the status codes of an MCParticle.
	 * @param[in] particle The MCParticle.
	 * @return The list of status codes converted to a string.
	 */
	static std::string makeSimStatusCodesString(EVENT::MCParticle* particle);

	/**
	 * Make a string from a vector of indices.
	 * @param[in] indices The IndexVector.
	 * @return The list of indices converted to a string.
	 */
	static std::string makeIndexVectorString(const IndexVector& indices);

	/**
	 * Get the indices of an MCParticle's daughters.
	 * @param[in] particle The MCParticle.
	 * @return The vector of indices.
	 */
	IndexVector findDaughterIndices(EVENT::MCParticle* particle);

    /**
     * Get the indices of an MCParticle's parents.
     * @param[in] particle The MCParticle.
     * @return The vector of indices.
     */
	IndexVector findParentIndices(EVENT::MCParticle* particle);

	/**
	 * Find the index of a particle.
	 * @param[in] particle The MCParticle.
	 * @return The index of the MCParticle or -1 if not found.
	 */
	int findParticleIndex(EVENT::MCParticle* particle);

	/**
	 * Insert a field separator.
	 */
	inline void sep();

	/**
	 * Insert a new line.
	 */
	inline void line();

	/**
	 * Print the header.
	 */
	inline void head();

	/**
	 * Print the footer.
	 */
	inline void foot();

	/**
	 * Print the column labels.
	 */
	void colLabels();

	/**
	 * Print the status flag labels.
	 */
	void flagLabels();

public:

	static const int idx_width;
	static const int ptr_width;
	static const int pdg_width;
	static const int par_width;
	static const int dau_width;
	static const int mom_width;
	static const int vtx_width;
	static const int end_width;
	static const int time_width;
	static const int mass_width;
	static const int chrg_width;
	static const int e_width;
	static const int stat_width;
	static const int flags_width;

private:

	EVENT::LCCollection* m_coll;
	std::string m_collName;
};
}

#endif
