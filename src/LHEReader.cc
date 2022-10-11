#include "LHEReader.hh"

#include <iostream>
#include <stdexcept>

namespace slic {

LHEReader::LHEReader(const std::string& filename) {
  ifs_.open(filename.c_str(), std::ifstream::in);
}

LHEReader::~LHEReader() { ifs_.close(); }

shared_ptr< LHEEvent > LHEReader::nextEvent() {

  std::string line;
  auto event_found{false};
  while (getline(ifs_, line)) {
    if (line == "<event>") {
      event_found = true;
      break;
    }
  }

  if (!event_found) {
    std::cerr << "WARNING: No next <event> element was found by the LHE reader." 
              << std::endl;
    return NULL;
  }

  getline(ifs_, line);

  auto next_event{std::make_shared<LHEEvent>(line)}; 
  while (getline(ifs_, line)) {
    if (line == "</event>") {
      break;
    }

    auto particle{new LHEParticle(line)};
    next_event->addParticle(particle);
  }

  for (auto& particle : next_event->getParticles()) { 
    if (particle->getMOTHUP(0) != 0) {
      int mother1 = particle->getMOTHUP(0);
      int mother2 = particle->getMOTHUP(1);
      if (mother1 > 0) {
        particle->setMother(0, particles[mother1 - 1]);
      }
      if (mother2 > 0) {
        particle->setMother(1, particles[mother2 - 1]);
      }
    }
  }

  return next_event;
}
}
