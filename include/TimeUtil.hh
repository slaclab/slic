#ifndef SLIC_TIMEUTIL_HH
#define SLIC_TIMEUTIL_HH 1

// std
#include <ctime>
#include <cstdio>
#include <iostream>
#include <string>

namespace slic {

/**
 * @class TimeUtil
 * @brief Miscellaneous time-related utilities.
 */
class TimeUtil {

    public:

        typedef long long long64;

        static std::string getAscTime() {
            tm* timeinfo = get_tm_local();

            std::string s = asctime(timeinfo);
            s.erase(s.end() - 1, s.end());
            return s;
        }

        //! \todo FIXME: This has only millisecond accuracy.
        static long64 getTimeNS() {
            long64 t_sec = (long64) time(NULL);
            long64 t_ns = t_sec * ((long64) 1e9);
            return t_ns;
        }

        static std::string getDate() {
            tm* timeinfo = get_tm_local();
            char buffer[11];
            strftime(buffer, 11, "%Y-%m-%d", timeinfo);
            return std::string(buffer);
        }

        static tm* get_tm_local() {
            time_t rawtime;
            struct tm* timeinfo;

            time(&rawtime);
            timeinfo = localtime(&rawtime);

            return timeinfo;
        }
};

} // namespace slic

#endif
