#ifndef SLIC_COMMANDLINEPROCESSOR_HH_
#define SLIC_COMMANDLINEPROCESSOR_HH_ 1

// STL
#include <string>
#include <map>
#include <vector>

// SLIC
#include "CommandLineOption.hh"
#include "CommandQueue.hh"
#include "Module.hh"
#include "PackageInfo.hh"
#include "Singleton.hh"

namespace slic {

/**
 * @author Jeremy McCormick 
 * @brief  Command line processor for SLIC that uses getopt.
 * @note All options are mapped in a straightforward fashion
 *       to Geant4 macro commands that may be defined 
 *       within LCDD, SLIC, or Geant4 itself.  The result
 *       of processing the command line using this class is
 *       to fill the CommandQueue, which is then retrieved 
 *       and executed by G4Application.
 */
class CommandLineProcessor: public Module, public Singleton<CommandLineProcessor> {

public:

    /**
     * OptionsPair is a short option name and its associated arguments.
     */
    typedef std::pair<std::string, std::string> OptionsPair;

    /**
     * CommandLineArguments is the list of options and arguments from the command line.
     */
    typedef std::vector<CommandLineProcessor::OptionsPair> CommandLineArguments;

    /**
     * OptionsList is a list of CommandLineOption classes describing valid command line switches.
     */
    typedef std::vector<CommandLineOption*> OptionsList;

public:

    /**
     * Class constructor.
     */
    CommandLineProcessor();

    /**
     * Class destructor.
     */
    virtual ~CommandLineProcessor();

public:

    /**
     * This is the primary function to process the raw arguments.
     * Options that will cause an abort, e.g. '-h', '-v', and '-n',
     * are handled explicitly so that the correct flag is set.
     * Other arguments are handled generically and put into the
     * CommandLineArguments vector, which associates a short
     * option name with its arguments (if any).  Options that can
     * occur more than once, such as the '-m' command, get one entry
     * per occurrence in the CommandLineArguments.  getopt will
     * check that the option is valid and that enough arguments
     * were received, before the command is put into CommandLineArguments.
     * After calling process(), the getCommandQueue() function can
     * be used to retrieve the list of Geant4 macro commands that
     * resulted from processing the raw command line arguments.
     * @param[in] argc The argument count.
     * @param[in] argv The argument values.
     */
    void process(int argc, char** argv);

    /**
     * Print out the options table.
     */
    void printOptions();

    /**
     * Print the version information.
     */
    void printVersion() {
        log() << LOG::okay << PackageInfo::getShortName() << " " << PackageInfo::getVersion() << LOG::done;
    }

    /**
     * Get the queue of Geant4 macro commands created from
     * the command line arguments.
     * @return The queue of Geant4 macro commands to execute.
     */
    CommandQueue* getCommandQueue() {
        return &m_g4q;
    }

    /**
     * Print the usage statement.
     */
    void printUsage();

private:

    /**
     * Process all the command line arguments in correct
     * order to create the queue of Geant4 commands.
     */
    void processOptions();

    /**
     * Abort the application before Geant4 starts,
     * e.g. if usage or version are selected.
     * This is not called if the command line arguments
     * are invalid, as getopt will handle this and
     * call exit() itself.
     */
    void abort();

    /**
     * Process a command line switch by adding
     * the corresponding Geant4 macro command to
     * the CommandQueue.
     * @param[in] option The option to process.
     */
    void processOption(const std::string& option);

    /**
     * Add a command line option describing a
     * command line switch, its full name,
     * description, geant4 command, etc.
     * @param[in] option The command line option to add.
     */
    void addOption(CommandLineOption* option);

    /**
     * Find an option specification by its single letter designation (with no '-').
     * @param[in] option The option to find.
     * @return The command line option object or null if does not exist.
     */
    CommandLineOption* getCommandLineOption(const std::string& option);

    /**
     * Automatically create the getopt options string from OptionsList
     * and set the m_getoptOptions variable to this value.
     */
    void createOptionsString();

    /**
     * Register the list of valid command line options.
     */
    void registerOptions();

    /**
     * Returns true if the option was found in the command line arguments.
     * @param[in] option The command line option.
     * @return True if option exist; false if not.
     */
    bool hasOption(const std::string& option);

private:

    // A vector of options with their arguments.
    CommandLineArguments m_commandline;

    // A list of Geant4 macro commands filled by processing the command line arguments.
    CommandQueue m_g4q;

    // The list of valid options and their associated meta-data.
    OptionsList m_cmds;

    // Flags to be set by the process function.
    int m_help_flag;
    int m_interactive_flag;
    int m_version_flag;

    // The getopt options string, which is created automatically.
    std::string m_getoptOptions;
};
}

#endif
