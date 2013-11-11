// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/ParticleGunEventSource.cc,v 1.6 2012-11-27 19:32:19 jeremy Exp $
#include "ParticleGunEventSource.hh"

// geant4
#include "G4ParticleGun.hh"

namespace slic {

ParticleGunEventSource::ParticleGunEventSource() :
		EventSource("ParticleGunEventSource") {
	m_generator = new G4ParticleGun();
}

ParticleGunEventSource::~ParticleGunEventSource() {
	delete m_generator;
}

void ParticleGunEventSource::generate(G4Event* evt) {
	m_generator->GeneratePrimaryVertex(evt);
}

void ParticleGunEventSource::dumpCurrentEvent() {
	log() << LOG::okay << "Dumping ParticleGun Event info..." << LOG::done;

	G4ParticleDefinition* pdef = m_generator->GetParticleDefinition();

	log() << LOG::okay << "particle: " << pdef->GetParticleName() << LOG::done;
	log() << LOG::okay << "time: " << m_generator->GetParticleTime() << LOG::done;
	log() << LOG::okay << "pos: " << m_generator->GetParticlePosition() << LOG::done;
	log() << LOG::okay << "mom: " << m_generator->GetParticleMomentumDirection() << LOG::done;
	log() << LOG::okay << "E: " << m_generator->GetParticleEnergy() << LOG::done;
	log() << LOG::okay << LOG::done;
}

G4ParticleGun* ParticleGunEventSource::getParticleGun() {
	return m_generator;
}
}
