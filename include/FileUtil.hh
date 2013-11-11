// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/FileUtil.hh,v 1.12 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_FILEUTIL_HH
#define SLIC_FILEUTIL_HH 1

// STL
#include <string>

namespace slic {

/**
 * @class FileUtil
 * @brief Utilities for file manipulation.
 */
class FileUtil {

public:

    /**
     * Remove a file.
     * @param[in] filename Full path to a file.
     * @return Command status with non-zero indicating an error.
     */
    static int removeFile(const char* filename);

    /**
     * Remove a file.
     * @param[in] filename Full path to a file.
     * @return Command status with non-zero indicating an error.
     */
    static int removeFile(const std::string& filename);

    /**
     * Check if a file exists.
     * @param[in] fileName Full path to a file.
     * @return True if file exists; false if it does not.
     */
    static bool fileExists(const char* filename);

    /**
     * Check if a file exists.
     * @param[in] fileName Full path string to a file.
     * @return True if file exists; false if it does not.
     */
    static bool fileExists(const std::string& filename);

    /**
     * Get the base name of a file path, which has the directory stripped from it.
     * @param[in] filename Full path to a file.
     * @return The base name of the file.
     */
    static std::string basename(std::string filename);

    /**
     * Get the extension of a file.
     * @param[in] filename Full path to a file.
     * @return The extension of the file.
     */
    static std::string extension(std::string filename);

    /**
     * Strip the extension from a file name.
     * @param[in] filename Full path to a file.
     * @return The file name with extension stripped off.
     */
    static std::string removeExtension(std::string filename);
};
}

#endif
