#include "ModuleRegistry.hh"

// slic
#include "Module.hh"

// geant4
#include "globals.hh"

namespace slic {
ModuleRegistry::ModuleRegistry() :
		Module("ModuleRegistry", false, false) {
}

ModuleRegistry::~ModuleRegistry() {
}

Module* ModuleRegistry::findModule(const std::string& name) {
	return m_modules[name];
}

void ModuleRegistry::registerModule(Module* module) {
	if (module == 0) {
		log().warning("ModuleRegistry::registerModule - module points to null!");
		return;
	}

	if (m_modules[module->getName()] == 0) {
		m_modules[module->getName()] = module;
	} else {
		log().warning(module->getName() + " is already registered!");
	}
}

void ModuleRegistry::deregisterModule(Module* module) {
	deregisterModule(module->getName());
}

void ModuleRegistry::deregisterModule(const std::string& name) {
	ModuleMap::iterator it = m_modules.find(name);

	if (it != m_modules.end()) {
		m_modules.erase(it);
	}
}

void ModuleRegistry::print() {
	log() << LOG::always << LOG::endl;
	log() << LOG::always << "********************" << LOG::endl;
	log() << LOG::always << "* Module Registry  *" << LOG::endl;
	log() << LOG::always << "********************" << LOG::endl;
	for (ModuleMap::const_iterator it = m_modules.begin(); it != m_modules.end(); it++) {
		log() << LOG::always << it->first << LOG::endl;
	}
}

void ModuleRegistry::deleteModules() {
	for (ModuleMap::iterator it = m_modules.begin(); it != m_modules.end(); it++) {
		// Delete it as long as Module has not
		// been flagged to not delete on exit.
		if (it->second->deleteOnExit()) {
			delete it->second;
		}
	}
}
}
