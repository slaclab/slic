// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/FieldMessenger.cc,v 1.9 2012-11-27 19:32:19 jeremy Exp $
#include "FieldMessenger.hh"

// slic
#include "FieldManager.hh"
#include "G4MagIntegratorStepperFactory.hh"

// geant4
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4FieldManager.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4TransportationManager.hh"
#include "G4ApplicationState.hh"

namespace slic {

FieldMessenger::FieldMessenger(FieldManager* mgr) :
		m_mgr(mgr) {
	defineCommands();
}

FieldMessenger::~FieldMessenger() {
	delete m_fieldDir;
	delete m_selectStepperCmd;
	delete m_setDeltaOneStepCmd;
	delete m_setDeltaIntersectionCmd;
}

void FieldMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
	G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();

	if (cmd == m_selectStepperCmd) {
		G4MagIntegratorStepperFactory::instance()->setupG4MagIntegratorStepper(newVals);
#ifdef SLIC_LOG
		m_mgr->log() << LOG::okay << "mag field integrator <" << newVals << ">" << LOG::done;
#endif
	} else if (cmd == m_setDeltaOneStepCmd) {
		double deltaOneStep = m_setDeltaOneStepCmd->GetNewDoubleValue(newVals);
		fieldMgr->SetDeltaOneStep(deltaOneStep);
		m_mgr->log() << LOG::okay << "delta one step <" << deltaOneStep << ">" << LOG::done;
	} else if (cmd == m_setDeltaIntersectionCmd) {
		double deltaIntersection = m_setDeltaIntersectionCmd->GetNewDoubleValue(newVals);
		fieldMgr->SetDeltaIntersection(deltaIntersection);
		m_mgr->log() << LOG::okay << "delta intersection <" << deltaIntersection << ">" << LOG::done;
	} else {
		G4Exception("", "", JustWarning, "Unknown command.");
	}
}

void FieldMessenger::defineCommands() {
	G4UIparameter *p;

	m_fieldDir = new G4UIdirectory("/field/");
	m_fieldDir->SetGuidance("Magnetic field customization commands. [SLIC]");

	m_selectStepperCmd = new G4UIcommand("/field/selectStepper", this);

	m_selectStepperCmd->SetGuidance(
			"Select the G4MagIntegratorStepper:\nG4ClassicalRK4 G4ExplicitEuler G4HelixExplicitEuler G4HelixHeum G4HelixImplicitEuler G4HelixSimpleRunge G4ImplicitEuler G4CashKarpRKF45 G4SimpleHeum G4SimpleRunge");

	p = new G4UIparameter("stepper", 's', false);
	m_selectStepperCmd->SetParameter(p);
	m_selectStepperCmd->AvailableForStates(G4State_Init, G4State_Idle);

	m_setDeltaOneStepCmd = new G4UIcmdWithADoubleAndUnit("/field/setDeltaOneStep", this);
	m_setDeltaOneStepCmd->SetGuidance("Set the delta of one step.");
	m_setDeltaOneStepCmd->SetDefaultUnit("mm");
	m_setDeltaOneStepCmd->AvailableForStates(G4State_Init, G4State_Idle);

	m_setDeltaIntersectionCmd = new G4UIcmdWithADoubleAndUnit("/field/setDeltaIntersection", this);
	m_setDeltaIntersectionCmd->SetGuidance("Set the delta of intersection.");
	m_setDeltaIntersectionCmd->SetDefaultUnit("mm");
	m_setDeltaIntersectionCmd->AvailableForStates(G4State_Init, G4State_Idle);
}
}
