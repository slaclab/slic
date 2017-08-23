#ifndef SLIC_HITSCOLLECTIONUTIL_HH_
#define SLIC_HITSCOLLECTIONUTIL_HH_ 1

// std
#include <vector>
#include <string>

class SensitiveDetector;

namespace slic {

/**
 * @class HitsCollectionUtil
 * @brief Static utility methods for hits collections.
 */
class HitsCollectionUtil {

    private:

        /**
         * Class constructor.
         */
        HitsCollectionUtil();

    public:

        /**
         * Get the current list of sensitive detectors.
         * @return The list of sensitive detectors.
         */
        static std::vector<SensitiveDetector*> getSensitiveDetectors();

        /**
         * Get the current list of hits collection IDs.
         * @return The list of hits collection IDS.
         */
        static std::vector<int> getHCIDs();

        /**
         * Get the current list of hits collection names.
         * @return The list of hits collection names.
         */
        static std::vector<std::string> getHCNames();
};
}

#endif
