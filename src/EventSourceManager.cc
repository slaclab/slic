#include "EventSourceManager.hh"

// SLIC
#include "FileUtil.hh"
#include "EventAction.hh"
#include "RunManager.hh"
#include "SlicApplication.hh"
#include "GPSEventSource.hh"
#include "LcioEventSource.hh"
#include "ParticleGunEventSource.hh"
#include "StdHepEventSource.hh"
#include "LHEEventSource.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

namespace slic {

std::string EventSourceManager::STDHEP = std::string("stdhep");
std::string EventSourceManager::LCIO = std::string("lcio");
std::string EventSourceManager::GPS = std::string("gps");
std::string EventSourceManager::PARTICLE_GUN = std::string("gun");
std::string EventSourceManager::LHE = std::string("lhe");
std::string EventSourceManager::UNKNOWN = std::string("unknown");

EventSourceManager::EventSourceManager() :
        Module("EventSourceManager"), m_currentEventSource(0), m_filename(""), m_fileIsSet(false),
        m_newFilename(false), m_ngen(0), m_nskip(0), m_newSource(true), m_sourceType(eUnknown),
        m_lorentzTransformationAngle(0.), m_zSmearingParam(0.), m_enablePrintEvent(false) {

    // messenger with generator command macro bindings
    m_messenger = new GeneratorMessenger();

    // Always create a ParticleGun generator.
    m_particleGunSource = new ParticleGunEventSource();

    log().setOutputLevel(LOG::always);
}

EventSourceManager::~EventSourceManager() {
}

const std::string& EventSourceManager::getFilename() {
    return m_filename;
}

void EventSourceManager::setFilename(const std::string& f) {
    m_filename = f;
    m_fileIsSet = true;
    m_newFilename = true;

    /*  Get a file extension from the input file. */
    std::string fext = FileUtil::extension(m_filename);

    /* For known file extensions, get a source type. */
    ESourceType est = getSourceTypeFromFileExtension(fext);

    /* Setup a (possibly) new source. */
    setupEventSource(est);
}

EventSourceManager::ESourceType EventSourceManager::getSourceTypeFromFileExtension(const std::string& fext) {
    ESourceType est = eUnknown;

    /* LCIO file extension */
    if (fext == "slcio") {
        est = eLCIO;
    }
    /* StdHep files, which may have .xdr file ext */
    else if (fext == "stdhep" || fext == "xdr") {
        est = eStdHep;
    /* LHE files */
    } else if (fext == "lhe") {
        est = eLHE;
    } else {
        log() << LOG::error << "WARNING: File <" << m_filename << "> does not have a known file extension." << LOG::done;
    }
    return est;
}

void EventSourceManager::resetCurrentEventSource() {
    m_currentEventSource->reset();
    m_ngen = 0;
}

void EventSourceManager::setSkipEvents(unsigned int s) {
    m_nskip = s;
}

unsigned int EventSourceManager::getSkipEvents() {
    return m_nskip;
}

void EventSourceManager::setSourceType(const std::string& s) {
    setSourceType(getSourceTypeFromName(s));
}

void EventSourceManager::setSourceType(ESourceType egt) {
    m_sourceType = egt;
    m_newSource = true;
}

const std::string& EventSourceManager::getSourceNameFromType(ESourceType egt) const {
    if (egt == eStdHep) {
        return STDHEP;
    } else if (egt == eLCIO) {
        return LCIO;
    } else if (egt == eGPS) {
        return GPS;
    } else if (egt == eParticleGun) {
        return PARTICLE_GUN;
    } else if (egt == eLHE) {
        return LHE;
    }

    return UNKNOWN;
}

EventSourceManager::ESourceType EventSourceManager::getSourceTypeFromName(const std::string& s) const {
    std::string sl = StringUtil::toLower(s);

    ESourceType egt = eUnknown;

    if (sl == STDHEP) {
        egt = eStdHep;
    } else if (sl == GPS) {
        egt = eGPS;
    } else if (sl == PARTICLE_GUN) {
        egt = eParticleGun;
    } else if (sl == LCIO) {
        egt = eLCIO;
    } else if (sl == LHE) {
        egt = eLHE;
    }

    return egt;
}

EventSourceManager::ESourceType EventSourceManager::getCurrentSourceType() {
    return m_sourceType;
}

const std::string& EventSourceManager::getCurrentSourceName() {
    return getSourceNameFromType(m_sourceType);
}

void EventSourceManager::dumpCurrentEvent() {
    m_currentEventSource->printCurrentEvent();
}

void EventSourceManager::printNumEventsGenerated() {
    log() << LOG::okay << LOG::head << "Generated " << m_ngen << " events." << LOG::done;
}

int EventSourceManager::getNumEventsGenerated() {
    return m_ngen;
}

void EventSourceManager::incrNumEventsGenerated() {
    m_ngen += 1;
}

void EventSourceManager::generate(G4Event* evt) {
    if (!isEOF()) {
        m_currentEventSource->generate(evt);
    } else {
        log() << LOG::error << "No more input events from file <" << m_filename << ">." << LOG::done;
    }
    incrNumEventsGenerated();
}

void EventSourceManager::setEventSource(EventSource* es) {
    deleteCurrentEventSource();
    m_currentEventSource = es;
}

void EventSourceManager::deleteCurrentEventSource() {
    if (m_currentEventSource) {
        delete m_currentEventSource;
        m_currentEventSource = 0;
    }
}

EventSource* EventSourceManager::createEventSource(ESourceType st) {
    EventSource* src = 0;
    if (st == eLCIO) {
        src = new LcioEventSource(getFilename());
    } else if (st == eStdHep) {
        src = new StdHepEventSource(getFilename());
    } else if (st == eGPS) {
        src = new GPSEventSource();
    } else if (st == eParticleGun) {
        return m_particleGunSource;
    } else if (st == eLHE) {
        return new slic::LHEEventSource(getFilename());
    } else if (st == eUnknown) {
        log() << LOG::error << "The event source type <" << getSourceNameFromType(st) << " is flagged as unknown!" << LOG::done;
    } else {
        log() << LOG::error << "Invalid ESourceType." << LOG::done;
    }

    return src;
}

void EventSourceManager::setupEventSource(ESourceType st) {
    // Is it a known source type?
    if (st != eUnknown) {
        // It is a new type of source?
        if (isNewSource(st)) {
            // Set new source type.
            m_sourceType = st;

            // Delete current source.
            deleteCurrentEventSource();

            // Create new source.
            m_currentEventSource = createEventSource(m_sourceType);

#ifdef SLIC_LOG
            log().okay("Created event generator <" + getCurrentSourceName() + ">");
#endif
        } else {
            log().warning("Type of new event source is same as old; keeping old source.");
        }
    } else {
        G4Exception("", "", JustWarning, "Event Source type is unknown.");
    }
}

void EventSourceManager::setupEventSource(const std::string& s) {
    ESourceType st = getSourceTypeFromName(s);
    if (st != eUnknown) {
        setupEventSource(st);
    } else {
        log().error("The name <" + s + "> is not a valid event source.  New source was NOT created.");
    }
}

EventSource* EventSourceManager::getCurrentSource() {
    return m_currentEventSource;
}

void EventSourceManager::beginEvent(const G4Event* anEvent) {
    // Start the event timer, as generation occurs before EventAction::BeginOfEventAction().
    EventAction::getEventAction()->startEventTimer();

    // Begin event action of current source.
    m_currentEventSource->beginEvent(anEvent);
}

void EventSourceManager::endEvent(const G4Event* anEvent) {
    m_currentEventSource->endEvent(anEvent);

    if (m_enablePrintEvent) {
        m_currentEventSource->printCurrentEvent();
    }
}

void EventSourceManager::beginRun(const G4Run* aRun) {

#ifdef SLIC_LOG
    log() << LOG::debug << LOG::name << "beginRun" << LOG::done;
#endif

    // Setup a new event source if neccessary.
    setupEventSource();

    // Call the beginRun() function of the current event source.
    if (m_currentEventSource != 0) {
        m_currentEventSource->beginRun(aRun);

        // Filename is now old so source won't reset.
        m_newFilename = false;

        // Reset event counter.
        m_ngen = 0;
    } else {
        log() << LOG::fatal << "No event generator was setup." << LOG::done;
        RunManager::instance()->abortRun(SlicApplication::GENERATOR_NOT_SETUP);
    }
}

void EventSourceManager::setupEventSource() {
    if (m_newSource) {
        m_currentEventSource = createEventSource(m_sourceType);
        m_newSource = false;
    }
}

void EventSourceManager::endRun(const G4Run* aRun) {
    m_currentEventSource->endRun(aRun);
}

bool EventSourceManager::isFileSource() {
    return (m_sourceType == eStdHep || m_sourceType == eLCIO);
}

bool EventSourceManager::isEOF() {
    bool eof = false;
    if (isFileSource()) {
        EventSourceWithInputFile* src = dynamic_cast<EventSourceWithInputFile*>(m_currentEventSource);
        if (src) {
            eof = src->isEOF();
        }
    }
    return eof;
}

bool EventSourceManager::isNewSource() {
    return m_newSource;
}

bool EventSourceManager::isNewSource(ESourceType est) {
    return (est != eUnknown && est != m_sourceType);
}

bool EventSourceManager::isNewFilename() {
    return m_newFilename;
}

EventSourceWithInputFile* EventSourceManager::getFileSource() {
    EventSourceWithInputFile* esif = 0;
    if (isFileSource()) {
        esif = dynamic_cast<EventSourceWithInputFile*>(m_currentEventSource);
    }
    return esif;
}

G4GeneralParticleSource* EventSourceManager::getGPS() {
    GPSEventSource* src = dynamic_cast<GPSEventSource*>(m_currentEventSource);

    G4GeneralParticleSource* gps = 0;
    if (src) {
        gps = src->getGPS();
    }

    return gps;
}

G4ParticleGun* EventSourceManager::getParticleGun() {
    ParticleGunEventSource* src = dynamic_cast<ParticleGunEventSource*>(m_currentEventSource);

    G4ParticleGun* gun = 0;
    if (src) {
        gun = src->getParticleGun();
    }

    return gun;
}

void EventSourceManager::generateNextEvent(G4Event* evt) {
    generate(evt);
}

void EventSourceManager::setLorentzTransformationAngle(const G4double lorentzTransformationAngle) {

    m_lorentzTransformationAngle = lorentzTransformationAngle;
#ifdef SLIC_LOG
    log() << LOG::always << "Set Lorentz transformation angle to " << G4BestUnit(m_lorentzTransformationAngle, "Angle") << LOG::done;
#endif

}

}
