// $Id: SlicMain.hh,v 1.3 2013-11-06 00:23:35 jeremy Exp $
#ifndef SLIC_SLICMAIN_HH
#define SLIC_SLICMAIN_HH 1

#include "Module.hh"

namespace slic {

/**
 * @class SlicMain
 * @brief A wrapper for the main method of SLIC.
 */
class SlicMain: public Module {

public:

    /**
     * Class constructor.
     */
    SlicMain();

    /**
     * Class destructor.
     */
    virtual ~SlicMain();

public:

    /**
     * Execute the main method by instantiating the SlicApplication
     * and passing control to it.
     * @param[in] argc The argument count.
     * @param[in] argc The argument values.
     */
    int main(int argc, char** argv);
};
}

#endif
