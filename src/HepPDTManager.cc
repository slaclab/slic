#include "HepPDTManager.hh"

namespace slic {

HepPDTManager::HepPDTManager() :
        Module("HepPDTManager"), m_datacol(0), m_fileSet(false) {
    log().setOutputLevel(LOG::debug);
}

HepPDTManager::~HepPDTManager() {
    delete m_datacol;
}

HepPDT::ParticleDataTable* HepPDTManager::getParticleDataTable() {
    if (m_datacol == 0) {
        loadParticleData();
    }
    return m_datacol;
}

void HepPDTManager::setParticleDataFile(std::string pdfile) {
    m_pdfile = pdfile;
    m_fileSet = true;
}

void HepPDTManager::loadParticleData() {
    // See if we can get the data file from the environment if it wasn't setup via CL or G4 macro.
    if (!m_fileSet) {
        char *heppdt = getenv("HEPPDT_BASE");
        if (heppdt) {
            m_pdfile = std::string(heppdt) + std::string("/data/particle.tbl");
        } else {
            log() << LOG::error << "Could not find particle.tbl!" << LOG::done;
            return;
        }
    }

    std::ifstream pdfile(m_pdfile.c_str());
    if (!pdfile) {
#ifdef SLIC_LOG
        log() << LOG::error << "error reading file " << m_pdfile << LOG::done;
#endif
        return;
    }

    m_datacol = new HepPDT::ParticleDataTable("Particle Data Table");
    HepPDT::TableBuilder *tb = new HepPDT::TableBuilder((*m_datacol));
    if (!addParticleTable(pdfile, (*tb))) {
#ifdef SLIC_LOG
        log() << LOG::error << "error reading lines from " << m_pdfile << LOG::done;
#endif
    }

    delete tb;
    tb = 0;
    pdfile.close();

    // DEBUG
    //printParticleData();
}

void HepPDTManager::printParticleData() {
    log() << LOG::always << "-- HepPDT Particle Data Table --" << LOG::endl;
    log() << "ParticleDataTable has " << m_datacol->size() << " particles" << LOG::endl;

    for (HepPDT::ParticleDataTable::const_iterator it = m_datacol->begin(); it != m_datacol->end(); it++) {
        HepPDT::ParticleID id = it->first;
        HepPDT::ParticleData pdata = it->second;

        log() << LOG::always << id.pid() << " " << id.PDTname();

        if (id.isSUSY()) {
            log() << " <-- SUSY particle";
        }

        log() << LOG::endl;
    }

    log() << LOG::done;
}

} // namespace slic
