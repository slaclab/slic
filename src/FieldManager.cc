#include "FieldManager.hh"

namespace slic {

FieldManager::FieldManager() :
        Module("FieldManager") {
    m_messenger = new FieldMessenger(this);
}

FieldManager::~FieldManager() {
    delete m_messenger;
}
}
