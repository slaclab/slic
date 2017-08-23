#ifndef SLIC_LCIOFILENAMER_HH_
#define SLIC_LCIOFILENAMER_HH_ 1

// SLIC
#include "Module.hh"

// Geant4
#include "G4ThreeVector.hh"

// STL
#include <string>
#include <vector>

namespace slic {

/**
 * @class LcioFileNamer
 * @brief Automatically name LCIO output files according to input settings.
 */
class LcioFileNamer: public Module {

    public:

        /**
         * Class constructor.
         */
        LcioFileNamer();

        virtual ~LcioFileNamer();

    public:

        /**
         * Create a file name based on a list of fields to include.
         * @param[in] fields The list of fields.
         * @return The file name.
         */
        std::string makeFileName(std::vector<std::string> fields);

    private:

        /**
         * Get the value of a field.
         * @param[in] field The field name.
         */
        std::string getFieldValue(std::string field);

        /**
         * Create the file name using the default set of fields.
         * @return The file name.
         */
        std::string makeDefaultFileName();

        /**
         * Create the file name based on the current event generator.
         * @return The file name.
         */
        static std::string makeEventName();

        /**
         * Create the file name based on a generator with an input file.
         * @return The file name.
         */
        static std::string makeFileBasedName();

        /**
         * Create the file name based on settings of the Geant4 GPS.
         * @return The file name.
         */
        static std::string makeGPSName();

        /**
         * Create the file name based on settings of the Geant4 particle gun.
         * @return The file name.
         */
        static std::string makeGunName();

        /**
         * Create a string from the current run number information in the RunManager.
         * @return The run number string.
         */
        static std::string makeRunNumberString();

        /**
         * Create a string from the number of events that were run using 'beamon'.
         * @return The number of events that were run.
         */
        static std::string makeEventNumberString();

        /**
         * Create a string with the Geant4 version information.
         * @return The Geant4 version string.
         */
        static std::string getGeant4VersionString();

    private:

        static std::string m_sep;
        std::vector<std::string> m_defaultFields;
};
}

#endif
