// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/EventSourceWithInputFile.cc,v 1.10 2012-11-27 19:32:19 jeremy Exp $
#include "EventSourceWithInputFile.hh"

// SLIC
#include "EventSourceManager.hh"

namespace slic {
void EventSourceWithInputFile::setFilename(const std::string& s) {
	if (s != std::string("")) {

		if (FileUtil::fileExists(s)) {
			m_filename = s;

			m_fileIsSet = true;
			m_fileIsNew = true;
		}
#ifdef SLIC_LOG
		else {
			log() << LOG::error << "Ignoring nonexistant input file <" << s << ">." << LOG::done;
		}
#endif
	}
#ifdef SLIC_LOG
	else {
		log() << LOG::error << "Ignoring blank filename." << LOG::done;
	}
#endif
}

const std::string& EventSourceWithInputFile::getFilename() {
	return m_filename;
}

void EventSourceWithInputFile::reset() {
	// close if open
	if (m_fileIsOpen) {
		close();
	}

	// (re)open
	open();

	// should not be EOF now (maybe on next read)
	m_eof = false;
}

unsigned int EventSourceWithInputFile::skipEvents(unsigned int nevents) {
	unsigned int i;
	for (i = 0; i < nevents; i++) {

		if (isEOF()) {
			break;
		}

		readNextEvent();
	}
	return i;
}

bool EventSourceWithInputFile::isEOF() {
	return m_eof;
}

void EventSourceWithInputFile::beginEvent(const G4Event*) {
	readNextEvent();
}

void EventSourceWithInputFile::beginRun(const G4Run*) {
	// attempt to setup filename from mgr if got a new filename
	if (EventSourceManager::instance()->isNewFilename()) {
		setFilename(EventSourceManager::instance()->getFilename());
	}

	// check if filename is setup correctly
	if (!m_fileIsSet) {
		G4Exception("", "", FatalException, "Generator filename was not set.");
	}

	// reset if new file
	if (m_fileIsNew) {

		// close and open (possibly with new file)
		reset();

		/* Set state variables. */
		m_fileIsNew = false;
		m_fileIsOpen = true;
	}

	// get num events to skip from mgr
	int nskip = EventSourceManager::instance()->getSkipEvents();

	if (nskip) {

#ifdef SLIC_LOG
		log() << LOG::okay << "Skipping <" << nskip << "> events ..." << LOG::done;
#endif

		// skip events according to current mgr val
		int actualskipped = skipEvents(nskip);

#ifdef SLIC_LOG
		log() << LOG::okay << "Skipped <" << actualskipped << "> events." << LOG::done;

		if (actualskipped < nskip) {
			log() << LOG::warning << "Ran out of records to skip on event file!" << LOG::done;
		}
#endif

		// reset skip events
		EventSourceManager::instance()->setSkipEvents(0);
	}
}
}
