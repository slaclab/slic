// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/GPSEventSource.cc,v 1.9 2012-11-27 19:32:19 jeremy Exp $
#include "GPSEventSource.hh"

// geant4
#include "G4MuonMinus.hh"

namespace slic {
GPSEventSource::GPSEventSource() :
		EventSource("GPSEventSource") {
	m_generator = new G4GeneralParticleSource();

	setupGeneratorDefaults();
}

GPSEventSource::~GPSEventSource() {
	delete m_generator;
}

void GPSEventSource::setupGeneratorDefaults() {
	m_generator->GetCurrentSource()->SetParticleDefinition(G4MuonMinus::MuonMinusDefinition());

	m_generator->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
	m_generator->GetCurrentSource()->GetEneDist()->SetMonoEnergy(5 * GeV);

	m_generator->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
	m_generator->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0, 0, 0));

	m_generator->GetCurrentSource()->GetAngDist()->SetAngDistType("planar");
	m_generator->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0, 1, 0));
}

void GPSEventSource::generate(G4Event* evt) {
	m_generator->GeneratePrimaryVertex(evt);
}

/** This is mostly wrong until after the first event generation. */
void GPSEventSource::dumpCurrentEvent() {
	log() << LOG::okay << "Dumping GPS Event info..." << LOG::done;

	G4ParticleDefinition* pdef = m_generator->GetParticleDefinition();

	log() << LOG::okay << "particle: " << pdef->GetParticleName() << LOG::done;
	log() << LOG::okay << "time: " << m_generator->GetParticleTime() << LOG::done;
	log() << LOG::okay << "nparts: " << m_generator->GetNumberOfParticles() << LOG::done;
	log() << LOG::okay << "pos: " << m_generator->GetParticlePosition() << LOG::done;
	log() << LOG::okay << "mom: " << m_generator->GetParticleMomentumDirection() << LOG::done;
	log() << LOG::okay << "E: " << m_generator->GetParticleEnergy() << LOG::done;
	log() << LOG::okay << LOG::done;
}

G4GeneralParticleSource* GPSEventSource::getGPS() {
	return m_generator;
}
} // namespace slic
