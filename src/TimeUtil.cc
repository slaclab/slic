// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/TimeUtil.cc,v 1.10 2012-11-27 19:32:19 jeremy Exp $
#include "TimeUtil.hh"

// std
#include <ctime>
#include <cstdio>
#include <iostream>

std::string TimeUtil::getAscTime() {
	tm* timeinfo = get_tm_local();

	std::string s = asctime(timeinfo);
	s.erase(s.end() - 1, s.end());
	return s;
}

TimeUtil::long64 TimeUtil::getTimeNS() {
	long64 t_sec = (long64) time(NULL);
	long64 t_ns = t_sec * ((long64) 1e9);
	return t_ns;
}

std::string TimeUtil::getDate() {
	tm* timeinfo = get_tm_local();
	char buffer[11];
	strftime(buffer, 11, "%Y-%m-%d", timeinfo);
	return std::string(buffer);
}

tm* TimeUtil::get_tm_local() {
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	return timeinfo;
}
