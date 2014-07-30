#include "FileUtil.hh"

// STL
#include <iostream>
#include <stdio.h>
#include <fstream>

using std::ifstream;
using std::ios;

namespace slic {

int FileUtil::removeFile(const char* filename) {
	int ret = remove(filename);
	return ret;
}

int FileUtil::removeFile(const std::string& filename) {
	return FileUtil::removeFile(filename.c_str());
}

bool FileUtil::fileExists(const char* filename) {
	bool exists = true;

	ifstream fin;
	fin.open(filename, ios::in);

	// either doesn't exist or can't read
	if (fin.fail()) {
		exists = false;
	}
	fin.close();
	return exists;
}

bool FileUtil::fileExists(const std::string& filename) {
	return FileUtil::fileExists(filename.c_str());
}

std::string FileUtil::basename(std::string fn) {
	static std::string delim = "/";

	std::string filename = fn;

	// find Unix/Cygwin delim
	int basename_pos = filename.rfind(delim);

	// may be dealing with a Windows-style path
#if defined(__WIN32__) || defined(__CYGWIN__) || defined(__CYGWIN32__)
	if ( basename_pos == -1 ) {
		basename_pos = filename.rfind("\\");
	}
#endif

	// found a dir delimiter so erase up to it
	if (basename_pos != -1) {
		filename.erase(filename.begin(), filename.begin() + basename_pos + 1);
	}

	return filename;
}

std::string FileUtil::extension(std::string fn) {
	std::string::size_type idx = fn.find_last_of(".");

	std::string ext;
	if (idx != std::string::npos) {
		ext = fn.substr(idx + 1, fn.size() - idx);
	} else {
		ext = "";
	}

	return ext;
}

std::string FileUtil::removeExtension(std::string fn) {
	std::string::size_type idx = fn.find_last_of(".");

	std::string fname;
	if (idx != std::string::npos) {
		fname = fn.substr(0, idx);
	} else {
		fname = fn;
	}
	return fname;
}
}
