// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/G4MagIntegratorStepperFactory.hh,v 1.7 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_G4MAGINTEGRATORSTEPPERFACTORY_HH
#define SLIC_G4MAGINTEGRATORSTEPPERFACTORY_HH 1

// std
#include <string>

// slic
#include "Module.hh"
#include "Singleton.hh"

class G4MagIntegratorStepper;
class G4Mag_EqRhs;
class G4FieldManager;
class G4MagneticField;

namespace slic {

/**
 * @class G4MagIntegratorStepperFactory
 * @brief Instantiates and sets the G4MagIntegratorStepper based on name.
 */
class G4MagIntegratorStepperFactory: public Singleton<G4MagIntegratorStepperFactory>, public Module {

public:

    /**
     * Class constructor.
     */
    G4MagIntegratorStepperFactory();

    /**
     * Class destructor.
     */
    virtual ~G4MagIntegratorStepperFactory();

public:

    /**
     * Setup an integrator stepper by name.
     * @param[in] name The name of the stepper.
     */
    void setupG4MagIntegratorStepper(std::string name);

private:

    /**
     * Create an integrator stepper.
     * @return The integrator stepper.
     */
    G4MagIntegratorStepper* createG4MagIntegratorStepper(std::string name);

private:
    G4Mag_EqRhs* m_fieldEquations;
    G4FieldManager* m_fieldMgr;
    G4MagneticField* m_field;
};
}

#endif
