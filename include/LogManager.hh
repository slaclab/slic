#ifndef SLIC_LOGMANAGER_HH
#define SLIC_LOGMANAGER_HH 1

// SLIC
#include "LogStream.hh"
#include "LogMessenger.hh"
#include "Singleton.hh"

// Geant4
#include "G4UIdirectory.hh"

// STL
#include <algorithm>
#include <vector>
#include <map>

namespace slic {

/**
 * @class LogManager
 *
 * @brief Manages LogStreams and their associated LogMessenger objects.
 *
 * @note
 * This class "owns" both LogStream and LogMessenger objects, so
 * neither of these classes should be deleted outside of this class.
 *
 * @note
 * LogManager and LogStream are both friends of each other.
 */
class LogManager: public Singleton<LogManager> {

private:

	// LogStream vector.
	typedef std::vector<LogStream*> LogList;

	// LogStream vector iterator.
	typedef LogList::iterator LogListIterator;

	// LogMessenger vector.
	typedef std::vector<LogMessenger*> LogMessengerList;

	// LogMessenger vector iterator.
	typedef LogMessengerList::iterator LogMessengerListIterator;

	// Name to LogStream map.
	typedef std::map<std::string, LogStream*> LogNameMap;

	// LogMessenger to LogStream map.
	typedef std::map<LogStream*, LogMessenger*> LogMessengerMap;

public:

	/**
	 * Class constructor.
	 */
	LogManager() {
		m_logDir = new G4UIdirectory("/log/");
		m_logDir->SetGuidance("Commands for changing log output settings. [SLIC]");
	}

	/**
	 * Class destructor.
	 */
	virtual ~LogManager() {
		delete m_logDir;
	}

public:

	/**
	 * Create a LogStream.
	 * @param[in] name The name of the LogStream.
	 * @param[in] level The log level.
	 * @param[in] os The output stream.
	 */
	LogStream* createLog(std::string name = "NONE",
	        LOG::ELevel level = LOG::okay,
	        ostream* os = &std::cout) {
		LogStream *l = new LogStream(name, level, os);
		m_logs.push_back(l);
		m_logNameMap[name] = l;
		return l;
	}

	/**
	 * Create a LogStream and associated LogMessenger.
	 * @param[in] name The name of the LogStream.
	 * @param[in] level The log level.
	 * @param[in] os The output stream.
	 */
	LogStream* createLogWithMessenger(std::string name = "NONE",
	        LOG::ELevel level = LOG::okay,
	        ostream* os = &std::cout) {
		LogStream *l = createLog(name, level, os);
		createLogMessenger(l);
		return l;
	}

	/**
	 * Remove a LogStream and also its LogMessenger, if it exists.
	 * @param[in] log The LogStream to remove.
	 */
	void removeLog(LogStream *log) {
		if (log == 0)
			return;
		LogListIterator it = findLog(log);
		if (it != m_logs.end()) {
			LogStream *l = (*it);

			// Delete the LogStream's messenger, if it exists.
			LogMessenger *lm = findLogMessenger(l);
			if (lm != 0) {
				removeLogMessenger(lm);
			}

			// Delete the LogStream object.
			delete (*it);

			// Remove the LogStream entry.
			m_logs.erase(it);
		}
	}

private:

	/**
	 * Create a LogMessenger attached to the given LogStream.
	 * @param[in] l The LogStream.
	 * @return The new LogMessenger assigned to the LogStream.
	 */
	LogMessenger* createLogMessenger(LogStream *l) {
		// Check for and return an existing LogMessenger.
		LogMessenger *check = findLogMessenger(l);
		if (check != 0) {
			return check;
		}

		// Make a new LogMessenger.
		LogMessenger *lm = new LogMessenger(l);
		m_messengers.push_back(lm);
		m_logMessengerMap[l] = lm;
		return lm;
	}

	/**
	 * Remove a LogMessenger.
	 * @param lm The LogMessenger to remove.
	 */
	void removeLogMessenger(LogMessenger *lm) {
		if (lm == 0)
			return;
		LogMessengerListIterator it = findLogMessenger(lm);
		if (it != m_messengers.end()) {
			delete (*it);
			m_messengers.erase(it);
		}
	}

	/**
	 * Find a LogStream pointer by name.
	 * @param[in] name The name of the LogStream.
	 */
	LogStream* findLog(const std::string &name) {
		return m_logNameMap[name];
	}

	/**
	 * Get position of LogStream by its pointer.
	 * @param[in] log The LogStream.
	 * @return An iterator pointing to the LogStream.
	 */
	LogListIterator findLog(LogStream *log) {
		if (log == 0)
			return m_logs.end();
		return find(m_logs.begin(), m_logs.end(), log);
	}

	/**
	 * Find position of LogMessenger by its pointer.
	 * @param[in] lm The LogMessenger.
	 * @return An iterator pointing to the LogMessenger.
	 */
	LogMessengerListIterator findLogMessenger(LogMessenger *lm) {
		if (lm == 0)
			return m_messengers.end();
		return find(m_messengers.begin(), m_messengers.end(), lm);
	}

	/**
	 * Find a LogMessenger by its associated LogStream.
	 * @param[in] s The LogStream.
	 * @return The LogMessenger or null if not found.
	 */
	LogMessenger* findLogMessenger(LogStream *s) {
		return m_logMessengerMap[s];
	}

	friend class LogStream;

private:
	LogList m_logs;
	LogMessengerList m_messengers;
	LogNameMap m_logNameMap;
	LogMessengerMap m_logMessengerMap;

	G4UIdirectory *m_logDir;
};
}

#endif
