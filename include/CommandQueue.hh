#ifndef SLIC_COMMANDQUEUE_HH_
#define SLIC_COMMANDQUEUE_HH_ 1

// SLIC
#include "Module.hh"

// STL
#include <vector>
#include <string>

namespace slic {

/**
 * @class CommandQueue
 * @brief A queue of G4 commands to be executed in order.
 * @note Created by CommandLineProcessor from the command line arguments.
 */
class CommandQueue: public Module {
    public:

        typedef std::vector<std::string> CommandVectorType;
        typedef std::string CommandType;
        typedef size_t SizeType;

    public:

        /**
         * Class constructor.
         */
        CommandQueue() :
                Module("CommandQueue") {
        }

        /**
         * Class destructor.
         */
        virtual ~CommandQueue() {
        }

        /**
         * Add a command to the queue.
         * @param[in] command The Geant4 command to add to the queue.
         */
        void add(const CommandType& command) {
            m_commands.push_back(command);
        }

        /**
         * Add a command to the queue.
         * @param[in] command The Geant4 command to add to the queue.
         */
        void add(const char* command) {
            std::string cmdStr = command;
            add(std::string(cmdStr));
        }

        /**
         * Get the begin iterator of the queue.
         * @return An iterator pointing to the beginning of the command queue.
         */
        CommandVectorType::iterator cmdsBegin() {
            return m_commands.begin();
        }

        /**
         * Get the end iterator of the queue.
         * @return An iterator pointing to the end of the command queue.
         */
        CommandVectorType::iterator cmdsEnd() {
            return m_commands.end();
        }

        /**
         * Print the queue to the log.
         */
        void printOut();

        /**
         * Execute the commands in the queue sequentially.
         */
        void execute();

        /**
         * Clear all the commands in the queue.
         */
        void clear() {
            m_commands.clear();
        }

        /**
         * Find the first command that matches a string.
         * @param[in] command The macro command to find.
         * @return An iterator pointing to the command or end iterator if not found.
         */
        CommandVectorType::iterator find(const std::string& command);

    private:
        CommandVectorType m_commands;
};
}

#endif
