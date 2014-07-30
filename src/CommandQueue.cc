#include "CommandQueue.hh"

// Geant4
#include "G4UImanager.hh"

namespace slic {
void CommandQueue::printOut() {
	log() << LOG::always << "************************" << LOG::done;
	log() << LOG::always << "* Geant4 Command Queue *" << LOG::done;
	log() << LOG::always << "************************" << LOG::done;
	for (CommandVectorType::iterator iter = cmdsBegin(); iter != cmdsEnd(); iter++) {
		log() << LOG::always << *iter << LOG::done;
	}
	log() << LOG::always << LOG::done;
}

void CommandQueue::execute() {
	for (CommandVectorType::iterator iter = cmdsBegin(); iter != cmdsEnd(); iter++) {
		std::string cmd = *iter;
		G4UImanager::GetUIpointer()->ApplyCommand(cmd);
	}
}

CommandQueue::CommandVectorType::iterator CommandQueue::find(const std::string& str) {
	CommandVectorType::iterator iter = m_commands.begin();
	for (; iter != m_commands.end(); iter++) {
		if ((*iter).find(str) != std::string::npos) {
			break;
		}
	}
	return iter;
}
}
