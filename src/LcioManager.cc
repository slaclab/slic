#include "LcioManager.hh"

// SLIC
#include "EventSourceManager.hh"
#include "FileUtil.hh"
#include "LcioFileNamer.hh"
#include "LcioMessenger.hh"
#include "PackageInfo.hh"
#include "RunManager.hh"
#include "SlicApplication.hh"
#include "StdHepEventSource.hh"
#include "TimeUtil.hh"

// LCDD
#include "lcdd/core/LCDDProcessor.hh"
#include "lcdd/hits/CalorimeterHit.hh"
#include "lcdd/hits/TrackerHit.hh"
#include "lcdd/util/StringUtil.hh"

// LCIO
#include "EVENT/LCIO.h"
#include "IMPL/LCCollectionVec.h"
#include "IO/LCWriter.h"
#include "IOIMPL/LCFactory.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/lStdHep.hh"

// Geant4
#include "G4EventManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VHitsCollection.hh"

// STL
#include <ctime>

using EVENT::LCIO;
using EVENT::MCParticle;
using IMPL::LCCollectionVec;
using IMPL::LCEventImpl;
using IMPL::LCRunHeaderImpl;
using UTIL::LCTOOLS;
using UTIL::lStdHep;

using std::string;

namespace slic {

string LcioManager::m_defaultFileName = "slicEvents";

LcioManager::LcioManager()
    : Module("LcioManager"), m_writer(NULL), m_runHdr(NULL),
      m_fileExistsAction(LcioManager::eFail), m_runNumber(0),
      m_enableDumpEvent(false), m_writerIsOpen(false), m_usingAutoname(false) {

  // Initialize the Geant4 UI messenger for the LCIO.
  m_messenger = new LcioMessenger(this);

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
      log() << LOG::fatal << LOG::name << LOG::sep
            << "LCIO file already exists: " << fullFilename << LOG::done;
      RunManager::instance()->abortRun(SlicApplication::OUTPUT_FILE_EXISTS);
      // Deletion mode is on, so try to remove the current file.
    } else if (m_fileExistsAction == eDelete) {
      if (FileUtil::removeFile(fullFilename) != 0) {
        // Fatal error.  File could not be removed.
        log() << LOG::fatal << LOG::head
              << "Unable to delete old LCIO file: " << fullFilename
              << LOG::done;
        RunManager::instance()->abortRun(
            SlicApplication::FAILED_DELETE_LCIO_FILE);
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
    // G4Exception("", "", FatalException, "LCWriter is null.");
    log() << LOG::fatal << LOG::head << "LCWriter is null." << LOG::done;
    RunManager::instance()->abortRun();
  }
}

LcioManager::EFileExistsAction
LcioManager::getFileExistsActionFromString(const string &feaStr) {
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

void LcioManager::setRunNumber(int runNumber) {
  m_runNumber = runNumber;
  log().verbose("Set starting run number <" +
                StringUtil::toString((int)runNumber) + ">");
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

void LcioManager::beginRun(const G4Run *aRun) {

  // Set the G4Run counter.
#if (G4VERSION_NUMBER >= 1000)
  G4RunManager::GetRunManager()->GetNonConstCurrentRun()->SetRunID(m_runNumber);
#else
  // Alternate code for backward compatibility.
  const_cast<G4Run *>(G4RunManager::GetRunManager()->GetCurrentRun())
      ->SetRunID(m_runNumber);
#endif

  // Automatically create LCIO output file name if option was selected.
  if (m_usingAutoname) {
    makeAutoname();
  }

  // Open the LCIO output file for writing.
  openLcioFile();

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

void LcioManager::endRun(const G4Run *) {

  // Delete the LCIO run header object.
  deleteRunHeader();

  // Set append mode for subsequent writes if interactive mode and the run was
  // not aborted.
  if (!RunManager::instance()->isRunAborted()) {
    // If run was aborted, writer was never opened.
    if (m_writer != 0) {
      m_writer->close();
      m_writerIsOpen = false;
    }
    // Close the writer to flush it (could be reopened).
    if (SlicApplication::instance()->getMode() ==
        SlicApplication::eInteractive) {
      m_fileExistsAction = eAppend;
    }
  }
}

void LcioManager::createRunHeader(const G4Run *) {
  // create new run header
  m_runHdr = new LCRunHeaderImpl();

  // Write SLIC version into run header.
  m_runHdr->parameters().setValue("SLIC_VERSION", PackageInfo::getVersion());

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
  LCDDProcessor *lcddProc = LCDDProcessor::instance();

  for (iter = lcddProc->getSensitiveDetectorsBegin();
       iter != lcddProc->getSensitiveDetectorsEnd(); iter++) {
    m_runHdr->addActiveSubdetector((iter->second)->GetName());
  }
}

void LcioManager::setPath(const string &path) {
  log().okay("Set output directory to <" + path + ">.");

  m_path = path;
}

void LcioManager::setFilename(const string &filename) {
  m_filename = filename;

  /* If the given filename has an extension, then remove it.
   The LCIO library will add it automatically. */
  size_t hasExt = filename.find(".slcio");
  if (hasExt != string::npos) {
    m_filename.replace(filename.size() - 6, 6, "");
  }

  log().okay("Set output file name to <" + m_filename + ">.");
}

void LcioManager::setAutonameFields(const std::vector<std::string> &fields) {
  m_usingAutoname = true;
  m_currentAutonameFields.clear();
  for (std::vector<std::string>::const_iterator it = fields.begin();
       it != fields.end(); it++) {
    m_currentAutonameFields.push_back(*it);
  }
}

void LcioManager::makeAutoname() {
  std::string autoname = m_namer->makeFileName(m_currentAutonameFields);
  if (autoname.size() == 0 || autoname == "") {
    log().warning(
        "Autonaming returned an empty string.  Using default file name <" +
        m_defaultFileName + ">");
    setFilename(m_defaultFileName);
  } else {
    setFilename(autoname);
    log().okay("Autoname set file name to <" + autoname + ">.");
  }
}

LCEventImpl *LcioManager::createLCEvent(const G4Event *anEvent) {

  /* Create the LCEvent and set basic parameters. */
  LCEventImpl *lcevt = new LCEventImpl();
  lcevt->setEventNumber(anEvent->GetEventID());
  lcevt->setRunNumber(m_runHdr->getRunNumber());
  lcevt->setDetectorName(m_runHdr->getDetectorName());

  /* Get the current MCParticle collection. */
  LCCollection *mcpColl =
      MCParticleManager::instance()->getMCParticleCollection();

  if (mcpColl != 0) {

    /* Set the event weight. */
    float eventWeight = mcpColl->getParameters().getFloatVal("_weight");
    if (eventWeight != 0.0) {
      lcevt->setWeight(eventWeight);
    }

    /* Set the idrup. */
    int idrup = mcpColl->getParameters().getIntVal("_idrup");
    // G4cout << "idrup = " << idrup << G4endl;
    if (idrup != 0) {
      lcevt->parameters().setValue("_idrup", idrup);
    }
  }

  // Write SLIC version into event header.
  lcevt->parameters().setValue("SLIC_VERSION", PackageInfo::getVersion());

  /* Set the current LCEvent. */
  setCurrentLCEvent(lcevt);

  return lcevt;
}

LCEventImpl *LcioManager::createLCEvent() {
  return createLCEvent(
      G4EventManager::GetEventManager()->GetNonconstCurrentEvent());
}

void LcioManager::endEvent(const G4Event *) {

  // create HC in current LCEvent from current G4Event using builder
  createHitsCollections();

  // Dump event stats.
  if (m_enableDumpEvent) {
    LCTOOLS::dumpEventDetailed(m_currentLCEvent);
  }

  // set timestamp
  setEventTimeStamp();

  // write event
  m_writer->writeEvent(m_currentLCEvent);

  // flush writer
  m_writer->flush();

  // delete event's transient objects, including current LCEvent
  delete m_currentLCEvent;
  m_currentLCEvent = 0;
}

void LcioManager::setEventTimeStamp() {
  getCurrentLCEvent()->setTimeStamp(TimeUtil::getTimeNS());
}

void LcioManager::createHitsCollections() {
  m_HCBuilder->createHitCollectionsFromEvent(
      G4EventManager::GetEventManager()->GetNonconstCurrentEvent(),
      m_currentLCEvent);
}

void LcioManager::addCollection(EVENT::LCEvent *event,
                                EVENT::LCCollection *collection,
                                const std::string &collectionName) {
  event->addCollection(collection, collectionName);
}

void LcioManager::addCollection(EVENT::LCCollection *collection,
                                const std::string &collectionName) {
  getCurrentLCEvent()->addCollection(collection, collectionName);
}

} // namespace slic
