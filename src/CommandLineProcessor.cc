// $Id: CommandLineProcessor.cc,v 1.14 2012-11-27 19:32:19 jeremy Exp $

#include "CommandLineProcessor.hh"

// SLIC
#include "PackageInfo.hh"
#include "CommandQueue.hh"
#include "CommandLineOption.hh"

// getopt
#include "getopt.h"

// STL
#include <sstream>
#include <cassert>

using std::stringstream;
using std::left;

namespace slic {
CommandLineProcessor::CommandLineProcessor() :
		Module("CommandLineProcessor"), m_help_flag(0), m_interactive_flag(0), m_version_flag(0) {
	registerOptions();
	createOptionsString();
}

CommandLineProcessor::~CommandLineProcessor() {
}

void CommandLineProcessor::process(int argc, char** argv) {
	// Description of SLIC's options in getopt's format.
	static struct option long_options[] = {
	        { "help", no_argument, 0, 'h' },
	        { "help", no_argument, 0, '?' },
			{ "version", no_argument, 0, 'v' },
			{ "interactive", no_argument, 0, 'n' },
			{ "macro", required_argument, 0, 'm' },
			{ "event-file", required_argument, 0, 'i' },
			{ "lcdd-url", required_argument, 0, 'g' },
			{ "lcio-file", required_argument, 0, 'o' },
			{ "lcio-path", required_argument, 0, 'p' },
			{ "autoname", optional_argument, 0, 'O' },
			{ "lcio-delete", no_argument, 0, 'x' },
			{ "run-events", required_argument, 0, 'r' },
			{ "skip-events", required_argument, 0, 's' },
			{ "physics-list", required_argument, 0, 'l' },
			{ "log-file", required_argument, 0, 'L' },
			{ "seed", required_argument, 0, 'd' },
			{ "dump-gdml", required_argument, 0, 'G' },
			{ "optical", no_argument, 0, 'c' },
			{ "pdg-file", required_argument, 0, 'P' },
			{ "lcdd-version", required_argument, 0, 'V' },
			{ "lcdd-setup", required_argument, 0, 'S' },
			{ 0, 0, 0, 0 }
	};

	const char* optstr = m_getoptOptions.c_str();

	while (1) {
		int option_index = 0;
		int c;

		// Call getopt for next option.
		c = getopt_long(argc, argv, optstr, long_options, &option_index);

#ifdef SLIC_LOG
		log() << LOG::verbose << "Handling option <" << c << ">." << LOG::done;
#endif

		// Done.
		if (c == -1)
			break;

		// Handle options.
		switch (c) {

		case 'h':
			m_help_flag = 1;
			break;

		case '?':
			m_help_flag = 1;
			break;

		case 'n':
			m_interactive_flag = 1;
			break;

		case 'v':
			m_version_flag = 1;
			break;

		default:

			stringstream ostr;
			char cc = (char) c;
			ostr << cc;
			string theopt = ostr.str();

			if (optarg) {
				m_commandline.push_back(OptionsPair(theopt, optarg));
			} else {
				m_commandline.push_back(OptionsPair(theopt, ""));
			}

			break;
		}
	}

	//
	// The processing flow...
	//
	// 1) If version flag is set, print version and exit.
	//
	// 2) If usage flag is set or no arguments were received,
	//    print the usage and exit.
	//
	// 3) If a single non-option command was received, treat
	//    it as a macro to execute.
	//
	// 4) If no options were received or the number of options
	//    is less than the number of arguments, print usage
	//    and exit (syntax incorrect).
	//
	// 5) Finally, process the options in order if the syntax
	//    appears to be okay.
	//

	// Print the version information and abort.
	if (m_version_flag) {
		printVersion();
		abort();
	}
	// Print the usage and abort if the help flag is set
	// or no arguments were received.
	else if (m_help_flag || argc == 1) {
		printUsage();
		abort();
	}
	// Single macro to execute, if got 1 argument that is a non-option.
	else if (argc == 2 && optind < argc) {
		stringstream str;
		str << "/control/execute ";
		str << argv[1];
		m_g4q.add(str.str());
	}
	// Probably extra arguments or no options.
	else if (optind == 1 || optind < argc) {
		printUsage();
		abort();
	}
	// Default is process all the options and build the CommandQueue.
	else {
		processOptions();
	}
}

void CommandLineProcessor::printUsage() {
	printVersion();

	log() << LOG::okay << LOG::done;
	log() << LOG::okay << "**************" << LOG::done;
	log() << LOG::okay << "* SLIC Usage *" << LOG::done;
	log() << LOG::okay << "**************" << LOG::done;

	log() << LOG::okay << LOG::done;
	log() << LOG::okay << "Command line usage: " << LOG::done;
	log() << LOG::okay << LOG::done;
	log() << LOG::okay << "slic [single_macro_path]" << LOG::done;
	log() << LOG::okay << "slic [options]" << LOG::done;

	log() << LOG::okay << LOG::done;

	log() << LOG::okay << "Interactive usage: " << LOG::done;
	log() << LOG::okay << LOG::done;
	log() << LOG::okay << "slic -n" << LOG::done;

	log() << LOG::okay << LOG::done;

	printOptions();

	log() << LOG::okay << LOG::done;
}

void CommandLineProcessor::abort() {
	exit(0);
}

void CommandLineProcessor::registerOptions() {
	// Print application usage. 
	addOption(new CommandLineOption("h", "help", "Print SLIC usage.", 0, 0, "/slic/usage"));

	// Usage is also activated with the '?' switch.
	addOption(new CommandLineOption("?", "help", "Print SLIC usage.", 0, 0, "/slic/usage"));

	// Run in interactive mode.
	addOption(new CommandLineOption("n", "interactive", "Start a Geant4 interactive session.", 0, 0, "/control/interactive"));

	// Print SLIC version info.
	addOption(new CommandLineOption("v", "version", "Print SLIC version.", 0, 0, "/slic/version"));

	// Execute the commands from a macro file.
	addOption(new CommandLineOption("m", "macro", "Execute Geant4 commands from a file.", 1, 1, "/control/execute"));

	// Set the URL of the input LCDD file. 
	addOption(new CommandLineOption("g", "lcdd-url", "Set LCDD geometry file URL.", 1, 1, "/lcdd/url"));

	// Set the path to a generator file, e.g. StdHep or LCIO.
	addOption(new CommandLineOption("i", "event-file", "Set event input file full path.", 1, 1, "/generator/filename"));

	// Set the name of the LCIO output file. 
	addOption(new CommandLineOption("o", "lcio-file", "Set name of LCIO output file.", 1, 1, "/lcio/filename"));

	// Set the path of output files. 
	addOption(new CommandLineOption("p", "lcio-path", "Set directory for LCIO output.", 1, 1, "/lcio/path"));

	// Automatically name the LCIO output file.
	addOption(new CommandLineOption("O", "autoname", "Automatically name the LCIO output file.", 0, 1, "/lcio/autoname"));

	// Overwrite an existing LCIO output file.
	addOption(
			new CommandLineOption("x", "lcio-delete", "Delete an existing LCIO file.", 0, 0,
					"/lcio/fileExists delete"));

	// Number of events to run.
	addOption(new CommandLineOption("r", "run-events", "Run # of events.", 1, 1, "/run/beamOn"));

	// Set number of events to skip from a file-based event source (e.g. StdHep).
	addOption( new CommandLineOption("s", "skip-events", "Set number of events to skip.", 1, 1, "/generator/skipEvents"));

	// Set the physics list. 
	addOption(new CommandLineOption("l", "physics-list", "Set Geant4 physics list.", 1, 1, "/physics/select"));

	// Set the name of the log file. 
	addOption(new CommandLineOption("L", "log-file", "Set logfile name.", 0, 0, "/log/filename"));

	// Set the random seed.
	addOption(new CommandLineOption("d", "seed", "Set the random seed. (No argument seeds with time.)", 0, 1, "/random/seed"));

	// Dump the loaded Geant4 geometry to a GDML file.
	addOption(new CommandLineOption("G", "dump-gdml", "Dump geometry to GDML file.", 1, 1, "/lcdd/dumpGDML"));

	// Enable optical physics processes.
	addOption(new CommandLineOption("c", "optical", "Enable optical physics processes.", 0, 0, "/physics/enableOptical"));

	// Set location of particle.tbl.
	addOption(new CommandLineOption("P", "pdg-file", "Set location of particle.tbl file.", 1, 1, "/physics/setPDGFile"));

	// Set the version of the GDML setup.
	addOption(new CommandLineOption("V", "lcdd-version", "Set the version of the GDML setup tag.", 1, 1, "/lcdd/version"));

	// Set the name of the GMDL setup.
	addOption(new CommandLineOption("S", "lcdd-setup", "Set the name of the GDML setup tag.", 1, 1, "/lcdd/setupName"));
}

void CommandLineProcessor::processOption(const string& opt) {
	// Loop over all input arguments to look for this option.
	for (CommandLineArguments::iterator it = m_commandline.begin(); it != m_commandline.end(); it++) {

		// Look for a matching switch in the arguments.
		if (it->first == opt) {

			// Get the associated specification for this option.
			CommandLineOption* cmdlineopt = getCommandLineOption(opt);

			// We must have a valid option!
			assert(cmdlineopt != 0);

			// Get the associated Geant4 command string.
			string cmdstr = cmdlineopt->getG4CommandString();

			// Add arguments to the command string, if some were passed.
			if (it->second != "") {
				cmdstr += " ";
				cmdstr += it->second;
			}

			// Add the command string to the CommandQueue.
			m_g4q.add(cmdstr);
		}
	}
}

void CommandLineProcessor::addOption(CommandLineOption* opt) {
	m_cmds.push_back(opt);
}

void CommandLineProcessor::processOptions() {
	// Logger.
	processOption("L");

	// GDML file version in setup tag.
	processOption("V");

	// GDML name in setup tag.
	processOption("S");

	// Geometry file.
	processOption("g");

	// Optical processes.
	processOption("c");

	// SUSY data.
	processOption("P");

	// Physics list.
	processOption("l");

	// Add initialization after geometry and physics,
	// but only if there was a geometry file argument.
	// Otherwise, initialize is not called, and the
	// user is given the PreInit prompt.
	if (hasOption("g")) {
		m_g4q.add("/run/initialize");
	}

	// Dump gdml.
	processOption("G");

	// Random seed.
	processOption("d");

	// Event file.
	processOption("i");

	// Output path.
	processOption("p");

	// Delete existing LCIO file.
	processOption("x");

	// Set LCIO file name.
	processOption("o");

	// Autoname the LCIO file.
	processOption("O");

	// Events to skip.
	processOption("s");

	// Geant4 macro to run.
	processOption("m");

	// Events to run.
	processOption("r");

	// Interactive mode, if selected.
	if (m_interactive_flag) {
		m_g4q.add("/control/interactive");
	}
}

void CommandLineProcessor::printOptions() {
	log() << LOG::okay << "************************" << LOG::done;
	log() << LOG::okay << "* Command Line Options *" << LOG::done;
	log() << LOG::okay << "************************" << LOG::done;
	log() << LOG::okay << LOG::done;

	static const int opt_width = 8;
	static const int descr_width = 15;
	static const int name_width = 16;
	static const int cmd_width = 24;
	static const int sep_width = 60;

	log().getOutputStream().width(opt_width);
	log().getOutputStream() << left;

	log().getOutputStream() << left;
	log() << LOG::okay << "Option";

	log().getOutputStream().width(name_width);
	log().getOutputStream() << left;
	log() << LOG::okay << "Full Name";

	log().getOutputStream().width(cmd_width);
	log().getOutputStream() << left;
	log() << LOG::okay << "Macro Command";

	log().getOutputStream().width(descr_width);
	log().getOutputStream() << left;
	log() << LOG::okay << "Description";

	log().getOutputStream().fill('-');
	log().getOutputStream().width(sep_width);

	log() << LOG::okay << LOG::done;
	log() << LOG::okay << '-' << LOG::done;

	log().getOutputStream().fill(' ');

	for (OptionsList::const_iterator iter = m_cmds.begin(); iter != m_cmds.end(); iter++) {
		CommandLineOption* cl_opt = (*iter);

		std::string opt_str = "-" + cl_opt->getShortName();
		log().getOutputStream().width(opt_width);
		log().getOutputStream() << left;
		log() << LOG::okay << opt_str;

		std::string name_str = "--" + cl_opt->getLongName();
		log().getOutputStream().width(name_width);
		log().getOutputStream() << left;
		log() << LOG::okay << name_str;

		log().getOutputStream().width(cmd_width);
		log().getOutputStream() << left;
		log() << LOG::okay << cl_opt->getG4CommandString();

		log().getOutputStream().width(descr_width);
		log().getOutputStream() << left;
		log() << LOG::okay << cl_opt->getDescription();

		log() << LOG::okay << LOG::done;
	}
}

CommandLineOption* CommandLineProcessor::getCommandLineOption(const std::string& opt) {
	CommandLineOption* clo = 0;
	for (OptionsList::const_iterator iter = m_cmds.begin(); iter != m_cmds.end(); iter++) {
		if ((*iter)->getShortName() == opt) {
			clo = (*iter);
			break;
		}
	}
	return clo;
}

bool CommandLineProcessor::hasOption(const std::string& opt) {
	bool hasoption = false;
	for (CommandLineArguments::iterator it = m_commandline.begin(); it != m_commandline.end(); it++) {
		if ((*it).first == opt) {
			hasoption = true;
			break;
		}
	}
	return hasoption;
}

void CommandLineProcessor::createOptionsString() {
	m_getoptOptions = "";
	for (OptionsList::iterator it = m_cmds.begin(); it != m_cmds.end(); it++) {
		CommandLineOption* opt = *it;
		m_getoptOptions += opt->getShortName();
		if (opt->getMinArgs()) {
			m_getoptOptions += ":";
		} else if (opt->getMaxArgs()) {
			m_getoptOptions += "::";
		}
	}
}
} // namespace slic
