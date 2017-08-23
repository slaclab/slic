/**
 * @file LHEReader.h
 * @brief Class for reading LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef SLIC_LHEREADER_HH_
#define SLIC_LHEREADER_HH_

// LDMX
#include "LHEEvent.hh"

// STL
#include <fstream>

namespace slic {

/**
 * @class LHEReader
 * @brief Reads LHE event data into an LHEEvent object
 */
class LHEReader {

    public:

        /**
         * Class constructor.
         * @param fileName The input file name.
         */
        LHEReader(const std::string& fileName);

        /**
         * Class destructor.
         */
        virtual ~LHEReader();

        /**
         * Read the next event.
         * @return The next LHE event.
         */
        LHEEvent* readNextEvent();

    private:

        /**
         * The input file stream.
         */
        std::ifstream ifs_;
};

}

#endif
