#ifndef SLIC_FIELDMANAGER_HH_
#define SLIC_FIELDMANAGER_HH_ 1

// SLIC
#include "Module.hh"
#include "FieldMessenger.hh"
#include "Singleton.hh"

namespace slic {

/**
 * @class FieldManager
 * @brief A manager of B-fields that currently just holds the pointer to its associated FieldMessenger.
 */
class FieldManager: public Module, public Singleton<FieldManager> {

public:

    /**
     * Class constructor.
     */
    FieldManager();

    /**
     * Class destructor.
     */
    virtual ~FieldManager();

private:

    FieldMessenger* m_messenger;
};
}

#endif
