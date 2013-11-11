// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioEventSource.cc,v 1.10 2013-11-06 00:23:37 jeremy Exp $
#include "LcioEventSource.hh"

// slic
#include "LcioMcpFilter.hh"

namespace slic {

LcioEventSource::LcioEventSource(const std::string& fname) :
		EventSourceWithInputFile("LcioEventSource", fname), m_event(0) {
	m_reader = IOIMPL::LCFactory::getInstance()->createLCReader();

	m_filter = new LcioMcpFilter();
}

LcioEventSource::~LcioEventSource() {
	delete m_reader;
}

// open the current file
void LcioEventSource::open() {
	try {
#ifdef SLIC_LOG
		log() << LOG::verbose << "opening filename <" << m_filename << ">" << LOG::done;
#endif
		m_reader->open(m_filename);
		m_fileIsOpen = true;
	} catch (IO::IOException& ioe) {
		log() << LOG::error << ioe.what() << LOG::done;
		G4Exception("", "", FatalException, "Caught exception while reading LCIO input file.");
	}
}

// close the current file
void LcioEventSource::close() {
	try {
		m_reader->close();
		m_fileIsOpen = false;
	} catch (...) {
	}
}

// read the next event
void LcioEventSource::readNextEvent() {
	m_event = m_reader->readNextEvent();

	if (0 == m_event) {
		m_eof = true;
	}
}

void LcioEventSource::dumpCurrentEvent() {
	log() << LOG::debug << "LcioEventSource - Initial MCParticle Collection" << LOG::done;

	LcioMcpManager::instance()->printMcpCollection("initial");
}

void LcioEventSource::generate(G4Event* anEvent) {
	assert( LcioMcpManager::instance()->getInitialMcpCollection());
	LcioManager::instance()->getGenerator()->generatePrimaryVertexFromMcpCollection(
			LcioMcpManager::instance()->getInitialMcpCollection(), anEvent);
}

void LcioEventSource::beginRun(const G4Run* aRun) {
	// do superclass action
	EventSourceWithInputFile::beginRun(aRun);
}

void LcioEventSource::beginEvent(const G4Event* anEvent) {
	// read an event
	EventSourceWithInputFile::beginEvent(anEvent);

	// set initial collection to filtered coll w/ gen parts only
	LcioMcpManager::instance()->setInitialMcpCollection(
			m_filter->filterSimParticles(getCurrentMcpLCCollection()));
}

EVENT::LCCollection* LcioEventSource::getCurrentMcpLCCollection() {
	EVENT::LCCollection* mcpColl = 0;

	if (m_event) {

		mcpColl = LcioMcpManager::instance()->findParticleCollection(m_event);

		if (mcpColl == 0) {
			G4Exception("", "", FatalException, "No Mcp collection found in LCIO input event.");
		}
	} else {
		log() << LOG::error << "LCEvent is null" << LOG::done;
	}
	return mcpColl;
}
}
