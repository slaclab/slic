// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/PrimaryGeneratorAction.cc,v 1.38 2013-06-26 01:57:02 jeremy Exp $

// SLIC
#include "PrimaryGeneratorAction.hh"
#include "LcioManager.hh"
#include "RunManager.hh"
#include "SlicApplication.hh"
#include "LcioMcpManager.hh"
#include "LcioMcpFilter.hh"
#include "EventSourceManager.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// LCIO
#include "IMPL/MCParticleImpl.h"
#include "EVENT/LCCollection.h"

// Geant4
#include "G4Types.hh"
#include "G4StateManager.hh"

namespace slic {
PrimaryGeneratorAction::PrimaryGeneratorAction() :
		Module("PrimaryGeneratorAction", false) {
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {

	// Check if run needs to be aborted, in which case we immediately return without executing anything.
	if (RunManager::instance()->isRunAborted()) {
		G4RunManager::GetRunManager()->AbortRun();
		log() << LOG::warning << LOG::name << LOG::sep << "Run was already aborted.  Will not generate events." << LOG::done;
		return;
	}

	EventSourceManager* mgr = EventSourceManager::instance();

	// Beginning of event message that prints the event number.
	// This should show up as the first line in the log messages for this event.
	printBeginEventMessage(anEvent);

	// State of MCP manager needs to be reset for new event.
	// FIXME: This is also called in LcioManager::reset() but not working for some reason!
	LcioMcpManager::instance()->reset();

	// Generator manager begin event hook.
	mgr->beginEvent(anEvent);

	// Generate vertices using the manager.
	mgr->GeneratePrimaryVertex(anEvent);

	// Apply Lorentz transformation to event, used for non-zero beam crossing angle.
	applyLorentzTransformation(anEvent);

	// Apply Z smearing to event vertices.
	zsmear(anEvent);

	// If source has no more events then abort the run.
	if (mgr->isEOF()) {
		SlicApplication::instance()->setAborting(true);
	}
}

void PrimaryGeneratorAction::printBeginEventMessage(G4Event* anEvent) {
	log() << LOG::okay << ">>>> BeginEvent <" << StringUtil::toString(anEvent->GetEventID()) << ">"
			<< LOG::done;
}

void PrimaryGeneratorAction::applyLorentzTransformation(G4Event *evt) {
	const G4double alpha = EventSourceManager::instance()->getLorentzTransformationAngle();

	if (alpha == 0)
		return; // nothing to do

//#ifdef SLIC_LOG
//        log() << LOG::always << "applying Lorentz Transformation angle <" << alpha << LOG::done;
//#endif

	// parameters of the Lorentz transformation matrix
	const G4double gamma = sqrt(1 + sqr(tan(alpha)));
	const G4double betagamma = tan(alpha);

	// scan through all vertices and all valid primary particles
	for (int iVertex = 0; iVertex < evt->GetNumberOfPrimaryVertex(); iVertex++) {
		G4PrimaryVertex *vertex = evt->GetPrimaryVertex(iVertex);
		for (int iParticle = 0; iParticle < vertex->GetNumberOfParticle(); iParticle++) {
			G4PrimaryParticle *particle = vertex->GetPrimary(iParticle);
			// does the particle have a valid particle definition attached?
			if (particle->GetG4code()) {
				// before the transformation
				const G4double px = particle->GetPx();
				const G4double py = particle->GetPy();
				const G4double pz = particle->GetPz();

				//FG: take generator mass ( not necessarily euqal to PDG mass ) 
				const G4double m = particle->GetMass();

				// after the transformation (boost in x-direction)
				const G4double pxPrime = betagamma * sqrt(sqr(px) + sqr(py) + sqr(pz) + sqr(m)) + gamma * px;

				// py and pz remain the same, E changes implicitly with px
				particle->SetMomentum(pxPrime, py, pz);
			}
		}
		// the position of the vertex is not transformed here
	}

	EVENT::LCCollection* col = LcioMcpManager::instance()->getInitialMcpCollection();

	if (col != 0) { // if particle gun is used no collection exists ...

		int nMCP = col->getNumberOfElements();

		for (int i = 0; i < nMCP; ++i) {

			IMPL::MCParticleImpl* mcp = dynamic_cast<IMPL::MCParticleImpl*>(col->getElementAt(i));

			const double* p = mcp->getMomentum();

			// before the transformation
			double pPrime[3];

			const G4double m = mcp->getMass();

			// after the transformation (boost in x-direction)
			pPrime[0] = betagamma * sqrt(sqr(p[0]) + sqr(p[1]) + sqr(p[2]) + sqr(m)) + gamma * p[0];

			pPrime[1] = p[1];
			pPrime[2] = p[2];

			// py and pz remain the same, E changes implicitly with px
			mcp->setMomentum(pPrime);
		}
	}
}

/**
 * Apply a Z smear to the position of every G4PrimaryVertex in an event.
 */
void PrimaryGeneratorAction::zsmear(G4Event* anEvent) {

	//G4cout << "zsmear" << G4endl;

	G4double zParam = EventSourceManager::instance()->getZSmearing();

	//G4cout << "got zParam = " << zParam << G4endl;

	if (zParam == 0)
		return;

	// Generate smeared Z position.
	double zspread = (zParam == 0.0 ? 0.0 : G4RandGauss::shoot(0, zParam/mm));

	//G4cout << "zspread = " << zspread << G4endl;

	// Apply Z smearing to Geant4 event.
	int nVertices = anEvent->GetNumberOfPrimaryVertex();
	//G4cout << "nVertices = " << nVertices << G4endl;
	for (int iVertex = 0; iVertex < nVertices; iVertex++) {
		G4PrimaryVertex* vertex = anEvent->GetPrimaryVertex(iVertex);
		//G4cout << "G4.preZSmear = " << vertex->GetZ0() << G4endl;
		vertex->SetPosition(vertex->GetX0(), vertex->GetY0(), vertex->GetZ0() + zspread);
		//G4cout << "G4.postZSmear = " << vertex->GetZ0() << G4endl;
	}

	// Apply Z smearing to MCParticle collection.
	EVENT::LCCollection* mcpColl = LcioMcpManager::instance()->getInitialMcpCollection();
	if (mcpColl != 0) {
		int nMCP = mcpColl->getNumberOfElements();
		//G4cout << "nMCP = " << nMCP << G4endl;
		for (int i = 0; i < nMCP; ++i) {
			IMPL::MCParticleImpl* mcp = dynamic_cast<IMPL::MCParticleImpl*>(mcpColl->getElementAt(i));
			if (zParam != 0) {
				const double* v = mcp->getVertex();
				double vPrime[3];
				vPrime[0] = v[0];
				vPrime[1] = v[1];
				vPrime[2] = v[2] + zspread;
				//G4cout << "MCP.preZSmear = " << mcp->getVertex()[2] << G4endl;
				mcp->setVertex(vPrime);
				//G4cout << "MCP.postZSmear = " << mcp->getVertex()[2] << G4endl;
			}
		}
	}
}
}
