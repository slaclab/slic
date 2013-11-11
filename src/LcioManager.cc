// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioManager.cc,v 1.96 2013-11-06 00:23:37 jeremy Exp $
#include "LcioManager.hh"

// slic
#include "EventSourceManager.hh"
#include "StdHepEventSource.hh"
#include "FileUtil.hh"
#include "TimeUtil.hh"
#include "PackageInfo.hh"
#include "LcioMessenger.hh"
#include "LcioMcpManager.hh"
#include "LcioFileNamer.hh"
#include "TrajectoryManager.hh"
#include "SlicApplication.hh"
#include "RunManager.hh"

// lcdd
#include "lcdd/hits/CalorimeterHit.hh"
#include "lcdd/hits/TrackerHit.hh"
#include "lcdd/core/LCDDProcessor.hh"
#include "lcdd/util/StringUtil.hh"

// lcio
#include "EVENT/LCIO.h"
#include "IMPL/LCCollectionVec.h"
#include "IOIMPL/LCFactory.h"
#include "IO/LCWriter.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/lStdHep.hh"

// geant4
#include "G4EventManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"

// std
#include <ctime>

using IMPL::LCRunHeaderImpl;
using IMPL::LCEventImpl;
using IMPL::LCCollectionVec;
using EVENT::MCParticle;
using EVENT::LCIO;
using UTIL::LCTOOLS;
using UTIL::lStdHep;

using std::string;

namespace slic {

string LcioManager::m_defaultFileName = "outfile";

LcioManager::LcioManager() :
		Module("LcioManager"), m_McpFinalColl(0), m_writer(0), m_runHdr(0), m_fileExistsAction(
				LcioManager::eFail), m_runNumber(0), m_enableDumpEvent(false), m_writerIsOpen(false), m_usingAutoname(
				false) {
	// Initialize the Geant4 UI messenger for the LCIO.
	m_messenger = new LcioMessenger(this);

	// Initialize the LCIO-based event generator.
	m_eventGenerator = new LcioPrimaryGenerator(this);

	// Initialize the LCIO MCParticle manager.
	m_mcpManager = LcioMcpManager::instance();

	// Initialize the LCIO HitsCollectionBuilder.
	m_HCBuilder = new LcioHitsCollectionBuilder();

	// Create the LCIO writer.
	createWriter();

	// Create the LCIO file namer.
	m_namer = new LcioFileNamer();

	// Set the default output file name.
	m_filename = m_defaultFileName;
}

LcioManager::~LcioManager() {
	deleteWriter();

	if (m_messenger != 0) {
		delete m_messenger;
	}

}

void LcioManager::openLcioFile() {

	// Get full output path with extension to check its existence.
	string fullFilename = getFullOutputPath(true);

	// Default to writing a new file.
	int writeMode = LCIO::WRITE_NEW;

	// File exists?
	if (FileUtil::fileExists(fullFilename)) {

		// Failure mode is on, so kill the current run.
		if (m_fileExistsAction == eFail) {
			log() << LOG::fatal << LOG::name << LOG::sep << "LCIO file already exists: " << fullFilename << LOG::done;
			RunManager::instance()->abortRun(SlicApplication::OUTPUT_FILE_EXISTS);
		// Deletion mode is on, so try to remove the current file.
		} else if (m_fileExistsAction == eDelete) {
			if (FileUtil::removeFile(fullFilename) != 0) {
				// Fatal error.  File could not be removed.
				log() << LOG::fatal << LOG::head << "Unable to delete old LCIO file: " << fullFilename
						<< LOG::done;
				RunManager::instance()->abortRun(SlicApplication::FAILED_DELETE_LCIO_FILE);
			} else {
				log().okay("Deleted old LCIO file: " + fullFilename);
			}
		}
		// Append mode is on.
		else if (m_fileExistsAction == eAppend) {
			log().okay("Appending to existing LCIO file: " + fullFilename);
			writeMode = LCIO::WRITE_APPEND;
		}
	}
	// Create a new file if doesn't exist.
	else {
		log().okay("Creating new LCIO file: " + fullFilename);
	}

	// Open the file using the writer with the full path.
	if (m_writer) {
		if (!RunManager::instance()->isRunAborted()) {
			m_writer->open(getFullOutputPath(false), writeMode);
			m_writerIsOpen = true;
		}
	} else {
		//G4Exception("", "", FatalException, "LCWriter is null.");
		log() << LOG::fatal << LOG::head << "LCWriter is null." << LOG::done;
		RunManager::instance()->abortRun();
	}
}

LcioManager::EFileExistsAction LcioManager::getFileExistsActionFromString(const string& feaStr) {
	string s = StringUtil::toLower(feaStr);
	EFileExistsAction fea = eInvalid;
	if (s == "fail") {
		fea = eFail;
	} else if (s == "overwrite" || s == "delete") {
		fea = eDelete;
	} else if (s == "append") {
		fea = eAppend;
	}

	return fea;
}

void LcioManager::setRunNumber(RunNumberType rnt) {
	m_runNumber = rnt;

	log().verbose("Set starting run number <" + StringUtil::toString((int) m_runNumber) + ">");
}

void LcioManager::createWriter() {
	m_writer = IOIMPL::LCFactory::getInstance()->createLCWriter();
}

void LcioManager::deleteWriter() {
	if (m_writer != 0) {
		if (m_writerIsOpen) {
			try {
				m_writer->close();
				m_writerIsOpen = false;
			} catch (...) {
			}
		}
		delete m_writer;
		m_writer = 0;
	}
}

string LcioManager::getFullOutputPath(bool withExtension) {
	string fullPath;

	if (m_path.length() > 0) {
		// Append the output directory if one exists.
		fullPath = m_path;
	} else {
		// Default to the current directory if none specified.
		fullPath = ".";
	}

	// Append file name.
	fullPath += "/" + m_filename;

	// Append extension.
	if (withExtension) {
		fullPath += ".slcio";
	}

	return fullPath;
}

void LcioManager::beginRun(const G4Run* aRun) {

	// Automatically create LCIO output file name if option was selected.
	if (m_usingAutoname) {
		makeAutoname();
	}

	// Set the G4Run counter.
	G4RunManager::GetRunManager()->SetRunIDCounter(m_runNumber);

	// Open the LCIO output file for writing.
	openLcioFile();

	//std::cout << "abortCurrentRun = " << m_abortCurrentRun << std::endl;

	// Run aborted, because LCIO output file already exists.
	if (!RunManager::instance()->isRunAborted()) {

		// create new LCRunHeader
		createRunHeader(aRun);

		// write the run header
		m_writer->writeRunHeader(m_runHdr);

		// Incr run number.
		++m_runNumber;
	}
}

void LcioManager::endRun(const G4Run*) {

	// Delete the LCIO run header object.
	deleteRunHeader();

	// Set append mode for subsequent writes if interactive mode and the run was not aborted.
	if (!RunManager::instance()->isRunAborted()) {
		// If run was aborted, writer was never opened.
		if (m_writer != 0) {
			m_writer->close();
			m_writerIsOpen = false;
		}
		// Close the writer to flush it (could be reopened).
		if (SlicApplication::instance()->getMode() == SlicApplication::eInteractive) {
			m_fileExistsAction = eAppend;
		}
	}
}

void LcioManager::createRunHeader(const G4Run*) {
	// create new run header
	m_runHdr = new LCRunHeaderImpl();

	// Write SLIC version into run header.
	m_runHdr->parameters().setValue("SLIC_VERSION", PackageInfo::getVersionString());

	// Write Geant4 version into run header.
	m_runHdr->parameters().setValue("GEANT4_VERSION",
			SlicApplication::instance()->getGeant4VersionString().replace(0, 7, ""));

	// set run number
	m_runHdr->setRunNumber(m_runNumber);

	// set detector name in LCIO header
	setDetectorName();

	// add active subdet to header
	addActiveSubdetectors();
}

void LcioManager::setDetectorName() {
	string det_tag = LCDDProcessor::instance()->getDetectorName();
	m_runHdr->setDetectorName(det_tag);
	log().okay("Detector name set to <" + det_tag + "> in run header.");
}

void LcioManager::deleteRunHeader() {
	if (m_runHdr) {
		delete m_runHdr;
		m_runHdr = 0;
	}
}

void LcioManager::addActiveSubdetectors() {
	LCDDProcessor::SensitiveDetectors::const_iterator iter;
	LCDDProcessor* lcddProc = LCDDProcessor::instance();

	for (iter = lcddProc->getSensitiveDetectorsBegin(); iter != lcddProc->getSensitiveDetectorsEnd();
			iter++) {
		m_runHdr->addActiveSubdetector((iter->second)->GetName());
	}
}

void LcioManager::setPath(const string& path) {
	log().okay("Set output directory to <" + path + ">.");

	m_path = path;
}

void LcioManager::setFilename(const string& filename) {
	m_filename = filename;

	/* If the given filename has an extension, then remove it.
	 The LCIO library will add it automatically. */
	size_t hasExt = filename.find(".slcio");
	if (hasExt != string::npos) {
		m_filename.replace(filename.size() - 6, 6, "");
	}

	log().okay("Set output file name to <" + m_filename + ">.");
}

void LcioManager::setAutonameFields(const std::vector<std::string>& fields) {
	m_usingAutoname = true;
	m_currentAutonameFields.clear();
	for (std::vector<std::string>::const_iterator it = fields.begin(); it != fields.end(); it++) {
		m_currentAutonameFields.push_back(*it);
	}
}

void LcioManager::makeAutoname() {
	std::string autoname = m_namer->makeFileName(m_currentAutonameFields);
	if (autoname.size() == 0 || autoname == "") {
		log().warning(
				"Autonaming returned an empty string.  Using default file name <" + m_defaultFileName + ">");
		setFilename(m_defaultFileName);
	} else {
		setFilename(autoname);
		log().okay("Autoname set file name to <" + autoname + ">.");
	}
}

const string& LcioManager::getPath() const {
	return m_path;
}

const string& LcioManager::getFilename() const {
	return m_filename;
}

LCEventImpl* LcioManager::createLCEvent(const G4Event* anEvent) {
	assert( anEvent);

	LCEventImpl* lcevt = new LCEventImpl();
	lcevt->setEventNumber(anEvent->GetEventID());
	lcevt->setRunNumber(m_runHdr->getRunNumber());
	lcevt->setDetectorName(m_runHdr->getDetectorName());

	// Set the event weight and idrup from the StdHep file.
	EventSourceManager* genMgr = EventSourceManager::instance();
	if (genMgr->getCurrentSourceType() == EventSourceManager::eStdHep) {
		lStdHep* stdhep = ((StdHepEventSource*) (genMgr->getCurrentSource()))->getStdHepReader();
		if (stdhep->isStdHepEv4()) {
			lcevt->parameters().setValue("_weight", (float) stdhep->eventweight());
			lcevt->parameters().setValue("idrup", (int) stdhep->idrup());
		}
	} else {
		lcevt->parameters().setValue("_weight", (float) 1.0);
		lcevt->parameters().setValue("idrup", (float) 0.0);
	}

	// Write Geant4 version into event header.
	lcevt->parameters().setValue("GEANT4_VERSION",
			SlicApplication::instance()->getGeant4VersionString().replace(0, 7, ""));

	// Write SLIC version into event header.
	lcevt->parameters().setValue("SLIC_VERSION", PackageInfo::getVersionString());

	setCurrentLCEvent(lcevt);
	return lcevt;
}

LCEventImpl* LcioManager::createLCEvent() {
	return createLCEvent(G4EventManager::GetEventManager()->GetNonconstCurrentEvent());
}

void LcioManager::endEvent(const G4Event*) {
	if (!RunManager::instance()->isRunAborted()) {

		// create LCEvent
		createLCEvent();

		// create Mcp coll in LcioMcpManager
		m_mcpManager->endEvent(G4EventManager::GetEventManager()->GetNonconstCurrentEvent());

		// create mcp collection from input event
		createFinalMcpCollection();

		// If selected, add the initial MCParticle collection to the event.
		if (LcioMcpManager::instance()->writeInitialMCParticleCollection()) {
			addInitialMCParticleCollection();
		}

		// create HC in current LCEvent from current G4Event using builder
		createHitsCollections();

		// Dump event stats.
		if (m_enableDumpEvent) {
			LCTOOLS::dumpEventDetailed(m_currentLCEvent);
			//LCTOOLS::printMCParticles( LcioMcpManager::instance()->getFinalMcpCollection() );
		}

		// set timestamp
		setEventTimeStamp();

		// write event
		m_writer->writeEvent(m_currentLCEvent);

		// flush writer
		m_writer->flush();

		// delete event's transient objects, including current LCEvent
		reset();
	}
}

void LcioManager::setEventTimeStamp() {
	getCurrentLCEvent()->setTimeStamp(TimeUtil::getTimeNS());
}

void LcioManager::createHitsCollections() {
	m_HCBuilder->createHitCollectionsFromEvent(G4EventManager::GetEventManager()->GetNonconstCurrentEvent(), m_currentLCEvent);
}

void LcioManager::reset() {
	// delete transient event container
	delete m_currentLCEvent;
	m_currentLCEvent = 0;

	// reset mcp mgr, including clearing maps
	m_mcpManager->reset();
}

void LcioManager::createFinalMcpCollection() {
	// add Mcp coll to current event
	getCurrentLCEvent()->addCollection(m_mcpManager->getFinalMcpCollection(), LCIO::MCPARTICLE);
}

void LcioManager::addCollection(EVENT::LCEvent* event, EVENT::LCCollection* collection,
		const std::string& collectionName) {
	event->addCollection(collection, collectionName);
}

void LcioManager::addCollection(EVENT::LCCollection* collection, const std::string& collectionName) {
	getCurrentLCEvent()->addCollection(collection, collectionName);
}

void LcioManager::addInitialMCParticleCollection() {
	const std::string& name = std::string(LCIO::MCPARTICLE) + std::string("Initial");
	addCollection(LcioMcpManager::instance()->getInitialMcpCollection(), name);
}
}
