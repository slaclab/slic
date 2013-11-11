// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/TimeUtil.hh,v 1.12 2013-11-06 00:23:36 jeremy Exp $

#ifndef SLIC_TIMEUTIL_HH
#define SLIC_TIMEUTIL_HH 1

// std
#include <string>

/**
 * @class TimeUtil
 * @brief Miscellaneous time-related utilities.
 */
class TimeUtil {

public:

	typedef long long long64;

public:

	/**
	 * Class destructor.
	 */
	virtual ~TimeUtil();

private:

	/**
	 * Class constructor, which is private because functions are all static.
	 */
	TimeUtil();

public:

	/**
	 * Get the current date as a formmating string.
	 * @return The date formatted as "YYYY-MM-DD"
	 */
	static std::string getDate();

	/**
	 * Get the current ASCII time.
	 * @return The full time as an ASCII formatted string.
	 */
	static std::string getAscTime();

	/**
	 * Get the time since the epoch, in nanoseconds, with a one second resolution.
	 * @return time since the epoch in nanoseconds.
	 */
	static TimeUtil::long64 getTimeNS();

private:

	/**
	 * The current timeinfo.
	 * @return The timeinfo processed with the localtime() function.
	 */
	static tm* get_tm_local();
};

#endif
