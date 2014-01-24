// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/G4MagIntegratorStepperFactory.cc,v 1.8 2012-11-27 19:32:19 jeremy Exp $
#include "G4MagIntegratorStepperFactory.hh"

// G4
#include "G4Mag_UsualEqRhs.hh"
#include "G4ChordFinder.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ClassicalRK4.hh"
#include "G4ExplicitEuler.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixHeum.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4ImplicitEuler.hh"
//#include "G4RKG3_Stepper.hh"
#include "G4SimpleHeum.hh"
#include "G4SimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SystemOfUnits.hh"

using std::string;

namespace slic {
G4MagIntegratorStepperFactory::G4MagIntegratorStepperFactory() :
		Module("G4MagIntegratorStepperFactory") {
	m_fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
	m_field = static_cast<G4MagneticField*>(const_cast<G4Field*>(m_fieldMgr->GetDetectorField()));
	if (m_field == 0) {
		log() << LOG::error << "Command refused.  Detector has no G4MagneticField." << LOG::done;
	} else {
		m_fieldEquations = new G4Mag_UsualEqRhs(m_field);
	}
}

G4MagIntegratorStepperFactory::~G4MagIntegratorStepperFactory() {
	delete m_fieldEquations;
}

void G4MagIntegratorStepperFactory::setupG4MagIntegratorStepper(std::string name) {
	G4MagIntegratorStepper* stepper = createG4MagIntegratorStepper(name);

	if (stepper != 0) {

		G4ChordFinder* oldChordFinder = m_fieldMgr->GetChordFinder();
		if (oldChordFinder != 0) {
			delete oldChordFinder;
		}

		/* Create new chord finder with default step minimum. */
		m_fieldMgr->SetChordFinder(new G4ChordFinder(m_field, 1E-2 * mm, stepper));
	}
}

G4MagIntegratorStepper* G4MagIntegratorStepperFactory::createG4MagIntegratorStepper(string name) {
	if (name == "G4ClassicalRK4") {
		return new G4ClassicalRK4(m_fieldEquations);
	} else if (name == "G4ExplicitEuler") {
		return new G4ExplicitEuler(m_fieldEquations);
	} else if (name == "G4HelixExplicitEuler") {
		return new G4HelixExplicitEuler(m_fieldEquations);
	} else if (name == "G4HelixHeum") {
		return new G4HelixHeum(m_fieldEquations);
	} else if (name == "G4HelixImplicitEuler") {
		return new G4HelixImplicitEuler(m_fieldEquations);
	} else if (name == "G4HelixSimpleRunge") {
		return new G4HelixSimpleRunge(m_fieldEquations);
	} else if (name == "G4ImplicitEuler") {
		return new G4ImplicitEuler(m_fieldEquations);
	} else if (name == "G4CashKarpRKF45") {
		return new G4CashKarpRKF45(m_fieldEquations);
	} else if (name == "G4SimpleHeum") {
		return new G4SimpleHeum(m_fieldEquations);
	} else if (name == "G4SimpleRunge") {
		return new G4SimpleRunge(m_fieldEquations);
	} else {
		log() << LOG::error << "Unknown G4MagIntegratorStepper <" + name << ">" << LOG::done;
	}

	return 0;
}
}
