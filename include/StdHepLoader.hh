// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/StdHepLoader.hh,v 1.18 2013-11-06 00:23:35 jeremy Exp $

#ifndef slic_StdhepLoader_hh
#define slic_StdhepLoader_hh 1

// slic
#include "Module.hh"

// lcio
#include "UTIL/lStdHep.hh"

// std
#include <string>
#include <fstream>

class G4Event;
class G4PrimaryVertex;
class G4PrimaryParticle;

using UTIL::lStdHep;

namespace slic {
enum EErrorCode {
    eEOF = 106
};

/**
 @class StdHepLoader
 @brief Simple wrapper to lStdHep interface.
 */
class StdHepLoader: public Module {

public:

    /**
     * Class constructor.
     */
    StdHepLoader();

    /**
     * Class destructor.
     */
    virtual ~StdHepLoader();

public:

    /**
     * Get the file name.
     * @return The file name.
     */
    const std::string& getFilename() const {
        return m_filename;
    }

    /**
     * Get the backing lStdHep reader.
     * @return The lStdHep reader object.
     */
    lStdHep* getStdHepReader() {
        return m_reader;
    }

    /**
     * Return true if end of input events.
     * @return True if end of input events; false if not.
     */
    bool isEndOfInput() {
        return m_isEndOfInput;
    }

public:

    /**
     * Open a StdHep file for reading.
     * @param[in] filename The full path to the file.
     */
    void openStdHepFile(const std::string& filename);

    /**
     * Open a StdHep file for reading.
     * @param[in] filename The full path to the file.
     */
    void openStdHepFile(const char* filename);

    void closeStdHepFile();

    void readNextEvent();
    void dumpCurrentEvent();

private:

    std::string m_filename;
    lStdHep* m_reader;
    bool m_isEndOfInput;
};
}

#endif
