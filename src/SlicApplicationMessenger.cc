#include "SlicApplicationMessenger.hh"

// slic
#include "SlicApplication.hh"
#include "PackageInfo.hh"

namespace slic {

SlicApplicationMessenger::SlicApplicationMessenger(SlicApplication* app) {
    m_app = app;
    defineCommands();
}

SlicApplicationMessenger::~SlicApplicationMessenger() {
}

void SlicApplicationMessenger::SetNewValue(G4UIcommand* cmd, G4String) {
    if (cmd == m_interactive) {
        m_app->setMode(SlicApplication::eInteractive);
    } else if (cmd == m_slicVersion) {
        m_app->printVersion();
    } else if (cmd == m_slicUsage) {
        m_app->printUsage();
    } else {
        m_app->log() << LOG::error << "Unknown command to SlicApplicationMessenger." << LOG::done;
    }
}

void SlicApplicationMessenger::defineCommands() {
    // interactive mode
    m_interactive = new G4UIcommand("/control/interactive", this);
    m_interactive->SetGuidance("Set interactive mode to G4 UI session. [SLIC]");

    // SLIC dir
    m_slicDir = new G4UIdirectory("/slic/");
    m_slicDir->SetGuidance("SLIC simulator commands. [SLIC]");

    // print SLIC usage
    m_slicUsage = new G4UIcommand("/slic/usage", this);
    m_slicUsage->SetGuidance("Print SLIC usage instructions.");

    // print SLIC version
    m_slicVersion = new G4UIcommand("/slic/version", this);
    m_slicVersion->SetGuidance("Print SLIC version information.");
}
}
