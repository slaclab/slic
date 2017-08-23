#include "GeneratorMessenger.hh"

// SLIC
#include "EventSourceManager.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// Geant4
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"

// CLHEP
#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/Randomize.h"

#include <ctime>

namespace CLHEP {
}
using namespace CLHEP;

namespace slic {

GeneratorMessenger::GeneratorMessenger() {
    defineCommands();
}

GeneratorMessenger::~GeneratorMessenger() {
}

void GeneratorMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
    std::istringstream is((const char*) newVals);

    std::string s;
    is >> s;

    EventSourceManager* mgr = EventSourceManager::instance();

    // select
    if (cmd == m_selectCmd) {
        mgr->setupEventSource(s);
    }
    // reset
    else if (cmd == m_resetCmd) {
        mgr->resetCurrentEventSource();
    }
    // skipEvents
    else if (cmd == m_skipEventsCmd) {
        mgr->setSkipEvents(StringUtil::toInt(s));
    }
    // filename
    else if (cmd == m_filenameCmd) {
        mgr->setFilename(s);
    }
    // dumpCurrentEvent
    else if (cmd == m_enablePrintEventCmd) {
        G4bool enablePrintEvent = StringUtil::toBool(s);
        mgr->enablePrintEvent(enablePrintEvent);
    }
    // printNumEventsGenerated
    else if (cmd == m_printNumEventsGeneratedCmd) {
        mgr->printNumEventsGenerated();
    }
    // set random seed
    else if (cmd == m_randomSeedCmd) {
        G4int seed = 0;

        // seed from user arg
        if (s != std::string("")) {
            seed = StringUtil::toInt(s);
        }
        // seed from time
        // @todo Get MS instead of just seconds, which is not very good quality for a seed.
        else {
            seed = ((unsigned) time(NULL));
        }

        // seed it
        HepRandom::setTheSeed(seed);

        mgr->log() << LOG::okay << "set random seed: " << seed << LOG::done;
    }
    // Lorentz transformation angle
    else if (cmd == m_setLorentzTransformationAngleCmd) {
        EventSourceManager::instance()->setLorentzTransformationAngle(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newVals));
    } else if (cmd == m_setZSmearingParameterCmd) {
        EventSourceManager::instance()->setZSmearing(G4UIcmdWithADouble::GetNewDoubleValue(newVals));
    } else {
        G4Exception("", "", JustWarning, "Unknown command.");
    }
}

void GeneratorMessenger::defineCommands() {
    G4UIparameter *p;

    // generator dir
    m_generatorDir = new G4UIdirectory("/generator/");
    m_generatorDir->SetGuidance("Event generation commands.");

    // select
    m_selectCmd = new G4UIcommand("/generator/select", this);
    m_selectCmd->SetGuidance("Set Generator to use: lcio, stdhep, gps or gun.");
    p = new G4UIparameter("generator", 's', false);
    m_selectCmd->SetParameter(p);

    // reset
    m_resetCmd = new G4UIcommand("/generator/reset", this);
    m_resetCmd->SetGuidance("Reset the generator using the current info.");

    // skipEvents
    m_skipEventsCmd = new G4UIcmdWithAnInteger("/generator/skipEvents", this);
    m_skipEventsCmd->SetGuidance("Skip n events of current generator.");
    m_skipEventsCmd->SetParameterName("NumEvents", false);

    // filename
    m_filenameCmd = new G4UIcommand("/generator/filename", this);
    m_filenameCmd->SetGuidance("Set generator input filename.");
    p = new G4UIparameter("filename", 's', false);
    m_filenameCmd->SetParameter(p);

    // dumpCurrentEvent
    m_enablePrintEventCmd = new G4UIcommand("/generator/printEvent", this);
    m_enablePrintEventCmd->SetGuidance("Dump information about the current generator event.");

    // printNumEventsGenerated
    m_printNumEventsGeneratedCmd = new G4UIcommand("/generator/printNumEventsGenerated", this);
    m_printNumEventsGeneratedCmd->SetGuidance("Print out number of events generated with current generator.");

    // random seed cmd
    m_randomSeedCmd = new G4UIcommand("/random/seed", this);
    m_randomSeedCmd->SetGuidance("Set random seed; no argument seeds with current time.");
    p = new G4UIparameter("seedValue", 'i', true);
    m_randomSeedCmd->SetParameter(p);

    // Lorentz transformation
    m_setLorentzTransformationAngleCmd = new G4UIcmdWithADoubleAndUnit("/generator/setLorentzTransformationAngle", this);
    m_setLorentzTransformationAngleCmd->SetGuidance("Set the Lorentz transformation angle to boost generated events.");

    // Z smearing parameter.
    m_setZSmearingParameterCmd = new G4UIcmdWithADouble("/generator/setZSmearingParameter", this);
    m_setZSmearingParameterCmd->SetGuidance("Set the parameter for smearing vertex Z positions.");
}
}
