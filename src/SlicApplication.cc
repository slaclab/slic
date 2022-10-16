// SLIC
#include "SlicApplication.hh"
#include "CommandLineProcessor.hh"
#include "CommandQueue.hh"
#include "FieldManager.hh"
#include "SlicApplicationMessenger.hh"
#include "LcioManager.hh"
#include "MCParticleManager.hh"
#include "ModuleRegistry.hh"
#include "PackageInfo.hh"
#include "RunManager.hh"
#include "TrackManager.hh"
#include "TimeUtil.hh"
#include "HepPDTManager.hh"
#include "VRML2WriterMessenger.hh"
#include "PhysicsListManager.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"
#include "lcdd/core/LCDDDetectorConstruction.hh"

// Geant4
#include "G4ApplicationState.hh"
#include "G4StateManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

namespace slic {

SlicApplication::SlicApplication() :
        Module("SlicApplication", false), m_appMessenger(0), m_runManager(0), m_mode(SlicApplication::eBatch), m_returnCode(0), m_isInitialized(false), m_geant4VersionString(0) {
#ifdef SLIC_LOG
    log() << LOG::okay << LOG::head << "SLIC is starting." << LOG::done;
#endif
}

SlicApplication::~SlicApplication() {
    if (m_appMessenger != 0) {
        delete m_appMessenger;
    }

    // Deletion of all modules is commented out as iterating the
    // map causes a seg fault in more recent gcc versions like 9,
    // for unknown reasons. Should not have any effect since the
    // application is exiting anyways and there are no modules
    // which perform any meaningful cleanup in their destructors. --JM
/*
#ifdef linux
#ifdef SLIC_LOG
    log().debug("Deleting registered modules ...");
#endif
    ModuleRegistry::instance()->deleteModules();
#endif
*/

    // Delete the G4RunManager, which also deletes all the Geant4-related modules.
//#ifdef SLIC_LOG
//    log().debug("Deleting G4RunManager ...");
//#endif
//    delete G4RunManager::GetRunManager();

#ifdef SLIC_LOG
    log() << LOG::okay << LOG::head << "SLIC is exiting." << LOG::done;
    log() << "return code: " << getReturnCode() << " => " << getErrorCodeString(getReturnCode()) << LOG::done;
#endif

}

void SlicApplication::initialize(int argc, char** argv) {
    // Set the name and basename of slic's binary from the command line arguments.
    m_binaryname = std::string(argv[0]);
    setBinaryBasename();

    // Process arguments using the CommandLineProcessor.
    CommandLineProcessor* cmd = CommandLineProcessor::instance();
    cmd->process(argc, argv);

    m_ui = new G4UIExecutive(argc, argv);
}

void SlicApplication::initialize() {
    if (!m_isInitialized) {
        // Print the SLIC splash screen.
        printSplashScreen();

        // Initialize the application messenger.
        m_appMessenger = new SlicApplicationMessenger(this);

        // Create a new RunManager.
        m_runManager = new RunManager();

        PhysicsListManager::instance();

        // Initialize the LcioManager.
        LcioManager::instance();

        // Initialize the magnetic field messenger.
        FieldManager::instance();

        // Setup the LCDD subsystem.
        initializeLCDD();

        // Initialize the tracker manager.
        TrackManager::instance();

        // Initialize the MCParticle manager.
        MCParticleManager::instance();

        // Initialize visualization.
        initializeVis();

        // Set state variable.
        m_isInitialized = true;
    } else {
        log().warning("Call to SlicApplication::initialize() was ignored.  SLIC is already initialized!");
    }
}

void SlicApplication::initializeVis()
{
    G4VisExecutive* vis = new G4VisExecutive();
    vis->Initialize();

    // VRML writer within SLIC.
    new VRML2WriterMessenger();
}

void SlicApplication::initializeLCDD() {
    // LCDD geometry subsystem.
    m_runManager->SetUserInitialization(new LCDDDetectorConstruction());
}

void SlicApplication::setMode(SlicApplication::ERunMode rmode) {
    m_mode = rmode;
}

const std::string& SlicApplication::getBinaryName() const {
    return m_binaryname;
}

const std::string& SlicApplication::getBinaryBasename() const {
    return m_binarybasename;
}

int SlicApplication::getReturnCode() const {
    return m_returnCode;
}

void SlicApplication::setReturnCode(int rc) {
    m_returnCode = rc;
}

void SlicApplication::setBinaryBasename() {
    std::string::size_type sidx = m_binaryname.find_last_of('/');
    if (sidx != std::string::npos) {
        m_binarybasename = std::string(m_binaryname, sidx + 1);
    }
}

SlicApplication::ERunMode SlicApplication::getMode() const {
    return m_mode;
}

void SlicApplication::setAborting(bool abort) {
    if (abort) {
        G4RunManager::GetRunManager()->AbortRun(abort);
    }
}

RunManager* SlicApplication::getRunManager() {
    return m_runManager;
}

void SlicApplication::run() {
    // Initialize if not already.
    if (!m_isInitialized) {
        initialize();
    }

    // Retrieve the list of Geant4 commands created by CommandLineProcessor.
    CommandQueue* q = CommandLineProcessor::instance()->getCommandQueue();

    // Print, execute, and clear the CommandQueue.
#ifdef SLIC_LOG
    log().verbose("Printing the Geant4 command queue ...");
    q->printOut();
#endif
    q->execute();
    q->clear();

    // Start the UI session if in interactive mode.
    if (getMode() == eInteractive) {
        log().verbose("Starting interactive session ...");
        m_ui->SessionStart();
        delete m_ui;
    }
}

void SlicApplication::printVersion() {
    CommandLineProcessor::instance()->printVersion();
}

void SlicApplication::printSplashScreen() {
    log() << LOG::done;
    log() << LOG::always << "*************************************************************" << LOG::done;
    log() << LOG::always << " Application : " << PackageInfo::getFullName() << " (" << PackageInfo::getShortName() << ")" << LOG::done;
    log() << LOG::always << " Version : " << PackageInfo::getVersion() << LOG::done;
    log() << LOG::always << "*************************************************************" << LOG::done;
}

void SlicApplication::printUsage() {
    CommandLineProcessor::instance()->printUsage();
}

} // namespace slic
