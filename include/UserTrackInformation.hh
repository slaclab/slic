#ifndef SLIC_USERTRACKINFORMATION_HH_
#define SLIC_USERTRACKINFORMATION_HH_ 1

// LCDD
#include "lcdd/core/VUserTrackInformation.hh"

// SLIC
#include "TrackSummary.hh"

// Geant4
#include "globals.hh"
#include "G4Allocator.hh"
#include "G4VUserTrackInformation.hh"

namespace slic {

/**
 * @class UserTrackInformation
 * @brief The implementation of the G4VUserTrackInformation class for storing track information within the event
 */
class UserTrackInformation: public VUserTrackInformation {
        //: public G4VUserTrackInformation {

    public:

        /**
         * Class constructor.
         * @param[in] aTrackSummary The TrackSummary attached to this information.
         */
        UserTrackInformation(TrackSummary * aTrackSummary) {
            _trackSummary = aTrackSummary;
        }

        /**
         * Class destructor.
         */
        virtual ~UserTrackInformation() {
        }

        /**
         * New operator.
         */
        inline void *operator new(size_t);

        /**
         * Delete operator.
         */
        inline void operator delete(void *trackInfo);

        /**
         * Get the TrackSummary.
         * @return The TrackSummary.
         */
        TrackSummary* getTrackSummary() const {
            return _trackSummary;
        }

        void setHasTrackerHit() {
            if (_trackSummary != NULL)
                _trackSummary->setHasTrackerHit();
        }

        /**
         * Print out the track information.
         */
        // TODO: Implement this method.
        void Print() const {
            G4cout << "UserTrackInformation::Print is not implemented!!!" << G4endl;
        }

    private:
        TrackSummary* _trackSummary;
};

extern G4Allocator<UserTrackInformation> UserTrackInformationAllocator;

inline void* UserTrackInformation::operator new(size_t) {
    void* trackInfo;
    trackInfo = (void*) UserTrackInformationAllocator.MallocSingle();
    return trackInfo;
}

inline void UserTrackInformation::operator delete(void *trackInfo) {
    UserTrackInformationAllocator.FreeSingle((UserTrackInformation*) trackInfo);
}

}

#endif

