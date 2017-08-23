// $Id: LogMessenger.cc,v 1.13 2013-06-26 01:57:02 jeremy Exp $

// SLIC
#include "LogMessenger.hh"
#include "LogStream.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// Geant4
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

namespace slic {
LogMessenger::LogMessenger(LogStream* logger) :
        m_logStream(logger) {
    defineCommands();
}

LogMessenger::~LogMessenger() {
    ;
}

void LogMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
    if (cmd == m_setLevelCmd) {
        m_logStream->setOutputLevel((LOG::ELevel) G4UIcmdWithAnInteger::GetNewIntValue(newVals));
    } else if (cmd == m_fileCmd) {
        m_logStream->setOutputFile(newVals);
    }
}

void LogMessenger::defineCommands() {
    m_loggerDir = new G4UIdirectory(std::string("/log/" + m_logStream->getName() + "/").c_str());
    m_loggerDir->SetGuidance(std::string("Commands for the logger " + m_logStream->getName()).c_str());

    m_setLevelCmd = new G4UIcmdWithAnInteger(std::string("/log/" + m_logStream->getName() + "/setLogLevel").c_str(), this);
    m_setLevelCmd->SetParameterName("logLevel", false);
    std::string range = "logLevel >= 0 && logLevel < " + StringUtil::toString(LOG::nlevels);
    m_setLevelCmd->SetRange(range.c_str());
    m_setLevelCmd->SetGuidance("Set the log level.");

    m_fileCmd = new G4UIcmdWithAString(std::string("/log/" + m_logStream->getName() + "/setLogFile").c_str(), this);
    m_fileCmd->SetParameterName("file", false);
    m_fileCmd->SetGuidance("Set the output file for this logger.");
}
}
