#ifndef SLIC_HEPPDTMANAGER_HH_
#define SLIC_HEPPDTMANAGER_HH_ 1

// HepPDT
#include "HepPDT/TableBuilder.hh"
#include "HepPDT/ParticleDataTable.hh"

// SLIC
#include "Module.hh"
#include "Singleton.hh"

// STL
#include <stdio.h>
#include <cstdlib>

using HepPDT::TableBuilder;

namespace slic {

/**
 * @class HepPDTManager
 * @brief A class for managing the HepPDT particle table.
 */
class HepPDTManager: public Module, public Singleton<HepPDTManager> {
public:

    /**
     * Class constructor.
     */
    HepPDTManager();

    /**
     * Class destructor.
     */
    virtual ~HepPDTManager();

public:

    /**
     * Get the particle data table, which is created on demand.
     * @return The particle data table.
     */
    HepPDT::ParticleDataTable* getParticleDataTable();

    /**
     * Set the path to the particle data table.
     * @param[in] pdfile The path to the particle data table.
     */
    void setParticleDataFile(std::string pdfile);

private:

    /**
     * Load all data into the particle data table object.
     */
    void loadParticleData();

    /**
     * Print out the particle table data.
     */
    void printParticleData();

private:
    HepPDT::ParticleDataTable* m_datacol;
    std::string m_pdfile;
    bool m_fileSet;
};
}

#endif
