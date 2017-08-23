#ifndef SLIC_EVENTSOURCEWITHINPUTFILE_HH_
#define SLIC_EVENTSOURCEWITHINPUTFILE_HH_ 1

// SLIC
#include "EventSource.hh"
#include "FileUtil.hh"
#include "Module.hh"

namespace slic {

/**
 * @class EventSourceWithInputFile
 * @brief An EventSource with an associated input file, such as StdHep or LCIO event files.
 */
class EventSourceWithInputFile: public EventSource {

    protected:

        /**
         * Class constructor.
         * @param[in] name The name of the source.
         * @param[in] filename The file name of the source.
         */
        EventSourceWithInputFile(const std::string& name, const std::string& filename = "") :
                EventSource(name), m_filename(filename), m_eof(false), m_fileIsOpen(false), m_fileIsNew(false), m_fileIsSet(false) {
        }

    public:

        /**
         * Class destructor.
         */
        virtual ~EventSourceWithInputFile() {
        }

    public:

        /**
         * Set the file name of the source.
         * @param[in] filename The file name.
         */
        void setFilename(const std::string& filename);

        /**
         * Get the file name of the source.
         * @return The file name.
         */
        const std::string& getFilename();

        /**
         * Reset the source back to the beginning of the file.
         */
        void reset();

        /**
         * Skip a number of events in the source's file.
         * @param[in] nevents The number of events to skip.
         */
        unsigned int skipEvents(unsigned int nevents);

        /**
         * Check if the end of file has been reached.
         */
        bool isEOF();

        /**
         * Begin of event hook.
         * @param[in] event The Geant4 event.
         */
        void beginEvent(const G4Event* event);

        /**
         * Begin run hook.
         * @param[in] run The Geant4 run.
         */
        void beginRun(const G4Run* run);

        /**
         * Open the file associated with the source.
         */
        virtual void open() = 0;

        /**
         * Close the file associated with the source.
         */
        virtual void close() = 0;

        /**
         * Read the next event.
         */
        virtual void readNextEvent() = 0;

        /**
         * Dump the current event to std::cout.
         */
        virtual void printCurrentEvent() = 0;

    protected:

        // name of input file
        std::string m_filename;

        // state vars
        bool m_eof;
        bool m_fileIsOpen;
        bool m_fileIsNew;
        bool m_fileIsSet;
};
}

#endif
