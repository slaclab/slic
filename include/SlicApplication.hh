#ifndef SLIC_SLICAPPLICATION_HH_
#define SLIC_SLICAPPLICATION_HH_ 1

// SLIC
#include "Singleton.hh"
#include "Module.hh"

// Geant4
#include "G4UIsession.hh"
#include "G4ApplicationState.hh"

class G4RunManager;
class G4UIExecutive;

namespace slic {

// slic
class FieldMessenger;
class SlicApplicationMessenger;
class RunManager;

static const char* ErrorCodeStrings[] = {
        "okay",
        "unknown error",
        "caught unknown exception",
        "caught STD exception",
        "output file already exists",
        "failed to delete old output file",
        "event underflow",
        "invalid geometry setup",
        "event generator was never setup"
};

/**
 * @class SlicApplication
 * @brief Singleton class for a Geant4 application
 * @note  Instantiates manager and messenger classes, including
 *        user actions, G4RunManager, GeneratorManager, and PhysicsListManager.
 */
class SlicApplication: public Singleton<SlicApplication>, public Module {

    public:

        /**
         * Indicates the run mode: batch or interactive.
         */
        enum ERunMode {
            eBatch = 1, eInteractive = 2
        };

        /**
         * Error codes.
         */
        enum ErrorCode {
            OKAY = 0, UNKNOWN_ERROR,
            UNKNOWN_EXCEPTION,
            STD_EXCEPTION,
            OUTPUT_FILE_EXISTS,
            FAILED_DELETE_LCIO_FILE,
            EVENT_UNDERFLOW,
            INVALID_GEOMETRY_SETUP,
            GENERATOR_NOT_SETUP,
            ERROR_CODE_SIZE
        };

    private:

    public:

        /**
         * The constructor.
         */
        SlicApplication();

        /**
         * The destructor.
         */
        virtual ~SlicApplication();

    public:

        /**
         * Return SLIC's RunManager instance.
         * @return The RunManager instance.
         */
        RunManager* getRunManager();

        /**
         * Initialize the application.
         */
        void initialize();

        /**
         * Run the application.
         */
        void run();

        /**
         * Print the usage by calling the CommandLineProcessor.
         */
        void printUsage();

        /**
         * Print the version string.
         */
        void printVersion();

        /**
         * Print SLIC's splash screen.
         */
        void printSplashScreen();

        /**
         * Print the start time.
         */
        void printStartTime();

        /**
         * Print the end time.
         */
        void printEndTime();

        /**
         * Get the return code.
         * @return The return code.
         */
        int getReturnCode() const;

        /**
         * Get the run mode, interactive or batch.
         * @return The run mode.
         */
        ERunMode getMode() const;

        /**
         * Set abort state.
         * @param[in] a The application abort state.
         */
        void setAborting(bool a = true);

        /**
         * Get the abort state.
         */
        bool isAborting() const;

        /**
         * Set the run mode.
         * @param[in] rmode The run mode, either batch or interactive.
         */
        void setMode(ERunMode rmode);

        /**
         * Get the name of the binary, as it was invoked from the command line.
         * @return The name of the binary.
         */
        const std::string& getBinaryName() const;

        /**
         * Get the base name of the binary (file name only).
         * @return The base name of the binary.
         */
        const std::string& getBinaryBasename() const;

        /**
         * Setup SLIC from command line arguments using the CommandLineProcessor.
         * @param[in] argc The argument count.
         * @param[in] argv The argument values.
         */
        void initialize(int argc, char** argv);

        /**
         * Set the return code.
         * @param[in] rc The return code.
         */
        void setReturnCode(int rc);

        /**
         * Get a string representation of an error code.
         * @param[in] errorCode The errorCode.
         * @return The string representation of the error code.
         */
        const char* getErrorCodeString(unsigned int errorCode) {
            if (errorCode > (ERROR_CODE_SIZE - 1)) {
                log() << LOG::error << "invalid error code requested: " << errorCode << LOG::done;
                throw new std::exception();
            }
            return ErrorCodeStrings[errorCode];
        }

    private:

        /**
         * Set the binary base name from the full name.
         */
        void setBinaryBasename();

        /**
         * Register the Geant4 user action classes with the G4RunManager.
         */
        void initializeUserActions();

        /**
         * Register the LCDDDetectorConstruction.
         */
        void initializeLCDD();

        /**
         * Initialize the Geant4 visualization subsystem.
         */
#ifdef G4VIS_USE
        void initializeVis();
#endif

    protected:

        // G4 UI manager.
        G4UIExecutive* m_ui;

        // app messenger
        SlicApplicationMessenger* m_appMessenger;

        // field messenger
        FieldMessenger* m_fieldMessenger;

        RunManager* m_runManager;

        // application run mode: batch or interactive
        ERunMode m_mode;

        // return code
        int m_returnCode;

        // Has the initialize() function been called?
        bool m_isInitialized;

        // What was the name of the SLIC binary from CL? (used by LcioFileNamer)
        std::string m_binaryname;
        std::string m_binarybasename;

        // Version string of Geant4.
        std::string* m_geant4VersionString;
};
}

#endif
