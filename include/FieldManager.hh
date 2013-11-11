// $Id: FieldManager.hh,v 1.4 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_FIELDMANAGER_HH
#define SLIC_FIELDMANAGER_HH 1

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
