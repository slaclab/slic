// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/Module.cc,v 1.9 2013-06-26 01:57:02 jeremy Exp $

// SLIC
#include "Module.hh"
#include "ModuleRegistry.hh"

namespace slic {
Module::Module(const std::string& name, bool deleteOnExit, bool autoRegister) :
		m_deleteOnExit(deleteOnExit) {
	m_name = name;

	// If requested, register the module with the ModuleRegistry.
	if (autoRegister) {
		ModuleRegistry::instance()->registerModule(this);
	}

	// Create a logger for this module.
	m_log = LogManager::instance()->createLogWithMessenger(m_name, LOG::okay);

	// Init message.
#ifdef SLIC_LOG
	log().verbose( "Initialized module <" + getName() + ">" );
#endif    
}

Module::~Module() {
#ifdef SLIC_LOG
	log().verbose( getName() + " is being deleted." );
#endif
	LogManager::instance()->removeLog(m_log);
	ModuleRegistry::instance()->deregisterModule(this);
}
}
