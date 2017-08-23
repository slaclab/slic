#ifndef SLIC_COMMANDLINEOPTION_HH_
#define SLIC_COMMANDLINEOPTION_HH_ 1

// STL
#include <map>
#include <vector>
#include <string>

namespace slic {

/**
 * @author Jeremy McCormick <jeremym@slac.stanford.edu>
 * @class  CommandLineOption
 * @brief  Command-line option with a single letter, full name and description
 *         plus an associated Geant4 command (which is optional).
 */
class CommandLineOption {

    public:

        /**
         * The fully qualified constructor for CommandLineOption.
         * @param[in] shortname The single letter command switch.
         * @param[in] longname The longer name of the command.
         * @param[in] minArgs The minimum number of arguments.
         * @param[in] maxArgs The maximum number of arguments.
         * @param[in] macroCommand The corresponding Geant4 macro string.
         */
        CommandLineOption(const std::string& shortname,
                const std::string& longname,
                const std::string& description,
                unsigned int minArgs = 0,
                unsigned int maxArgs = 0,
                const std::string& macroCommand = "") :
                    m_shortname(shortname),
                    m_longname(longname),
                    m_description(description),
                    m_g4cmdstr(macroCommand),
                    m_minArgs(minArgs),
                    m_maxArgs(maxArgs) {
        }

        /**
         * Class destructor.
         */
        virtual ~CommandLineOption() {
        }

    public:

        /**
         * Get the short name associated with this option.
         * This must be a single letter, such as 'm'.
         * @return The single letter command switch.
         */
        const std::string& getShortName() const {
            return m_shortname;
        }

        /**
         * Get the long name associated with this option.
         * This must be one or more words separated by dashes,
         * such as "lcio-path".
         * @return The long name of the command.
         */
        const std::string& getLongName() const {
            return m_longname;
        }

        /**
         * Get a brief description of this option.
         * @return A description of the option.
         */
        const std::string& getDescription() const {
            return m_description;
        }

        /**
         * Get the Geant4 command associated with this option.
         * This may be a null string if there is no associated
         * Geant4 command, though currently all of SLIC's options
         * have a corresponding macro.
         * @return The Geant4 command string for the option.
         */
        const std::string& getG4CommandString() const {
            return m_g4cmdstr;
        }

        /**
         * Get the minimum number of arguments to this option.
         * @return The minimum number of arguments.
         */
        unsigned int getMinArgs() const {
            return m_minArgs;
        }

        /**
         * Get the maximum number of arguments to this option.
         * @return The maximum number of arguments.
         */
        unsigned int getMaxArgs() const {
            return m_maxArgs;
        }

    private:

        std::string m_shortname;
        std::string m_longname;
        std::string m_description;
        std::string m_g4cmdstr;
        int m_minArgs;
        int m_maxArgs;
};
}

#endif
