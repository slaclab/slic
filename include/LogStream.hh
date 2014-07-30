#ifndef SLIC_LOGSTREAM_HH_
#define SLIC_LOGSTREAM_HH_ 1

// LCDD
#include "TimeUtil.hh"

// STL
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

using std::ofstream;
using std::string;
using std::ostream;
using std::ios;
using std::ios_base;

namespace LOG {

/**
 * These are ios manipulators for this class.
 */
enum EManip {
    done = 101, head = 102, time = 103, level = 104, name = 105, sep = 106, endl = 107
};

/**
 * Log levels.
 */
enum ELevel {
    none = 0, always = 1, fatal = 2, error = 3, warning = 4, okay = 5, verbose = 6, debug = 7, nlevels = 8
};
}

/**
 * String representations of log levels.
 */
static std::string levelNames[LOG::nlevels] = { "NONE", "ALWAYS", "FATAL", "ERROR", "WARNING", "OKAY", "VERBOSE", "DEBUG" };

namespace slic {

/**
 * @class LogStream
 *
 * @brief A class for streaming messages to a log
 *
 * @note
 * LogStream provides methods for logging messages at a certain level.  The
 * LogStream has a default level, and if a message's log level is greater
 * than the current log level, it is discarded.
 *
 * @note
 * The stream operators are overloaded so that the LogStream can be used
 * similarly to std::cout.
 *
 * @note
 * The manipulator LOG::done should be used to terminate a log message.
 *
 * @note
 * Some of the code, particularly the handling of ios manipulators,
 * is based on Gaudi's
 * <a href="http://isscvs.cern.ch/cgi-bin/cvsweb.cgi/GaudiKernel/GaudiKernel/MsgStream.h?rev=1.41;content-type=text%2Fplain;cvsroot=Gaudi">MsgStream</a>
 * class.
 *
 * @note
 * For more information on the STL IO library see
 * <a href="http://www.cplusplus.com/reference/iolibrary/">this reference page</a>.
 */
class LogStream {

protected:

    /**
     * Class constructor.
     * @param[in] name The name of the LogStream.
     * @param[in] level The level of the logger.
     * @param[in] os The output stream for printing message.
     */
    LogStream(std::string name, LOG::ELevel level, ostream* os = &std::cout) :
            m_name(name), m_outputLevel(level), m_currentMessageLevel(LOG::okay), m_os(os), m_active(true), m_fos(0) {
        ;
    }

    /**
     * Class constructor providing default arguments.
     */
    LogStream() :
            m_name("NONE"), m_outputLevel(LOG::always), m_currentMessageLevel(LOG::okay), m_os(&std::cout), m_active(true), m_fos(0) {
        ;
    }

    /**
     * Class destructor.
     */
    virtual ~LogStream() {
    }

    /**
     * Give the LogManager full access to protected constructors.
     */
    friend class LogManager;

public:

    /**
     * Get the name of the LogStream.
     * @return The name of the LogStream.
     */
    const std::string& getName() const {
        return m_name;
    }

    /**
     * Get whether this LogStream is active or not.
     * @return True if logstream is active; false if not.
     */
    bool isActive() const {
        return m_active;
    }

    /**
     * Get the output level which determines which messages will be shown.
     * @return The output level.
     */
    LOG::ELevel getOutputLevel() const {
        return m_outputLevel;
    }

    /**
     * Set the output level.
     * @param[in] outputLevel The output level.
     */
    void setOutputLevel(const LOG::ELevel& outputLevel) {
        m_outputLevel = outputLevel;
    }

    /**
     * Get the output stream.
     * @return The output stream.
     */
    ostream& getOutputStream() const {
        return *m_os;
    }

    /**
     * Get the current log level for messages.
     * @return The current log level.
     */
    LOG::ELevel getCurrentLevel() const {
        return m_currentMessageLevel;
    }

    /**
     * Set the current log level for messages.
     * @param[in] level The log level.
     */
    void setCurrentLevel(LOG::ELevel level) {
        m_currentMessageLevel = level;
    }

    /**
     * Set the log level via streamer.  Subsequent messages will be placed onto the LogStream if
     * the level is less than or equal to the current output level.  Otherwise they will be
     * discarded.
     * @param[in] level The log level.
     * @return The current LogStream.
     */
    LogStream& operator <<(const LOG::ELevel& level) {

        //std::cout << "setting level: " << level << std::endl;
        //std::cout << "current level: " << getCurrentLevel() << std::endl;

        setCurrentLevel(level);

        //std::cout << "name: " << m_name << std::endl;
        //std::cout << "current level: " << getCurrentLevel() << std::endl;
        //std::cout << "output level: " << getOutputLevel() << std::endl;
        //std::cout << std::endl;

        if (getCurrentLevel() <= getOutputLevel()) {
            // Set active state.
            setActive(true);
        } else {
            // Set inactive state.
            setActive(false);
        }
        return *this;
    }

    /**
     * Handle ios manipulators for the LogStream.
     * @param[in] x The ios manipulator.
     * @return The LogStream.
     */
    LogStream& operator <<(const LOG::EManip& x) {
        if (isActive()) {
            // End of message.
            if (x == LOG::done) {
                getOutputStream() << std::endl;
                // Print full header info.
            } else if (x == LOG::head) {
                getOutputStream() << TimeUtil::getAscTime() << " :: " << levelNames[m_currentMessageLevel] << " :: " << getName() << " :: ";
                // Print current time.
            } else if (x == LOG::time) {
                getOutputStream() << TimeUtil::getAscTime();
                // Print current message level.
            } else if (x == LOG::level) {
                getOutputStream() << levelNames[m_currentMessageLevel];
                // Print logger name.
            } else if (x == LOG::name) {
                getOutputStream() << getName();
            } else if (x == LOG::sep) {
                getOutputStream() << " :: ";
                // Print end of line terminator.
            } else if (x == LOG::endl) {
                getOutputStream() << std::endl;
            }
        }
        return *this;
    }

    /**
     * Redirect the messages to a file.
     * @param[in] file The file name.
     */
    void setOutputFile(const std::string& file) {
        ofstream* m_fos = new ofstream();
        m_fos->open(file.c_str(), ios::app | ios::out);
        m_os = m_fos;
    }

    /**
     * Set the current level to debug.
     * @return The LogStream.
     */
    LogStream& debug() {
        return *this << LOG::debug;
    }

    /**
     * Set the current level to verbose.
     * @return The LogStream.
     */
    LogStream& verbose() {
        return *this << LOG::verbose;
    }

    /**
     * Set the current level to okay.
     * @return The LogStream.
     */
    LogStream& okay() {
        return *this << LOG::okay;
    }

    /**
     * Set the current level to warning.
     * @return The LogStream.
     */
    LogStream& warning() {
        return *this << LOG::warning;
    }

    /**
     * Set the current level to error.
     * @return The LogStream.
     */
    LogStream& error() {
        return *this << LOG::error;
    }

    /**
     * Set the current level to fatal.
     * @return The LogStream.
     */
    LogStream& fatal() {
        return *this << LOG::fatal;
    }

    /**
     * Set the current level to always.
     * @return The LogStream.
     */
    LogStream& always() {
        return *this << LOG::always;
    }

    /**
     * Log a message at a certain level.
     * @param[in] mesg The message contents.
     * @param[in] level The log level.
     */
    void log(const std::string& message, const LOG::ELevel& level) {
        (*this) << level << message << LOG::done;
    }

    /**
     * Log a message at the current level (NOT output level).
     * @param[in] mesg The message.
     */
    void log(const std::string& mesg) {
        log(mesg, getOutputLevel());
    }

    /**
     * Log a message at debug level.
     * @param[in] mesg The message.
     */
    void debug(const std::string& mesg) {
        log(mesg, LOG::debug);
    }

    /**
     * Log a message at verbose level.
     * @param[in] mesg The message.
     */
    void verbose(const std::string& mesg) {
        log(mesg, LOG::verbose);
    }

    /**
     * Log a message at okay level.
     * @param[in] mesg The message.
     */
    void okay(const std::string& mesg) {
        log(mesg, LOG::okay);
    }

    /**
     * Log a message at warning level.
     * @param[in] mesg The message.
     */
    void warning(const std::string& mesg) {
        log(mesg, LOG::warning);
    }

    /**
     * Log a message at error level.
     * @param[in] mesg The message.
     */
    void error(const std::string& mesg) {
        log(mesg, LOG::error);
    }

    /**
     * Log a message at fatal level.
     * @param[in] mesg The message.
     */
    void fatal(const std::string& mesg) {
        log(mesg, LOG::fatal);
    }

    /**
     * Log a message at always level.
     * @param[in] mesg The message.
     */
    void always(const std::string& mesg) {
        log(mesg, LOG::always);
    }

    /**
     * Handle STL Setiosflags manipulator.
     * @param[in] The ios manipulator.
     * @return The LogStream.
     */
    //LogStream& operator <<(const std::_Setiosflags &manip) {
    //    if (isActive()) {
    //        getOutputStream() << manip;
    //    }
    //    return *this;
    //}

    /**
     * Handle STL Resetiosflags manipulator.
     * @param[in] The ios manipulator.
     * @return The LogStream.
     */

    //LogStream& operator <<(const std::_Resetiosflags &manip) {
    //    if (isActive()) {
    //        getOutputStream() << manip;
    //    }
    //    return *this;
    //}

    /**
     * Handle STL Setbase manipulator.
     * @param[in] The ios manipulator.
     * @return The LogStream.
     */
    //LogStream& operator <<(const std::_Setbase &manip) {
    //    if (isActive()) {
    //        getOutputStream() << manip;
    //    }
    //    return *this;
    //}

    /**
     * Handle STL Setprecision manipulator.
     * @param[in] The ios manipulator.
     * @return The LogStream.
     */
    //LogStream& operator <<(const std::_Setprecision &manip) {
    //    if (isActive()) {
    //        getOutputStream() << manip;
    //    }
    //    return *this;
    //}

    /**
     * Handle STL Setw manipulator.
     * @param[in] The ios manipulator.
     * @return The LogStream.
     */
    //LogStream& operator <<(const std::_Setw &manip) {
    //    if (isActive()) {
    //        getOutputStream() << manip;
    //    }
    //    return *this;
    //}

    /**
     * Handle ios class modifiers.
     * @param[in] manip The ios class modifier.
     * @return The LogStream.
     */
    LogStream& operator <<(ios& (*manip)(std::ios&)) {
        if (isActive()) {
            getOutputStream() << manip;
        }
        return *this;
    }

    /**
     * Handle ios base class modifiers.
     * @param[in] manip The ios base class modifier.
     * @return The LogStream.
     */
    LogStream& operator <<(std::ios_base& (*manip)(std::ios_base&)) {
        if (isActive()) {
            getOutputStream() << manip;
        }
        return *this;
    }

    /**
     * Templated stream operator.
     * @param[in] arg The templated argument.
     * @return The LogStream.
     */
    template<typename T>
    LogStream& operator<<(const T& arg) {
        if (isActive()) {
            getOutputStream() << arg;
        }
        return *this;
    }

    /**
     * Specialization stream operator for vectors.
     * @param[in] v The input vector.
     * @return The LogStream.
     */
    template<typename T>
    LogStream& operator <<(const std::vector<T>& v) {
        if (isActive()) {
            for (typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); ++i) {
                getOutputStream() << *i << " ";
            }
        }
        return *this;
    }

    /**
     * Get the current width setting.
     * @param[in] The width setting.
     */
    int width() const {
        return isActive() ? getOutputStream().width() : 0;
    }

    /**
     * Set the current width.
     * @param[in] The width setting.
     * @return The new width setting.
     */
    int width(int v) {
        return isActive() ? getOutputStream().width(v) : 0;
    }

    /**
     * Get the current set of flags.
     * @return The current set of ios formatting flags.
     */
    long flags() const {
        return isActive() ? getOutputStream().flags() : 0;
    }

    /**
     * Set the formatting flags.
     * @param[in] v The formatting flags.
     * @return The current set of formatting flags.
     */
    long flags(std::ios_base::fmtflags v) {
        return isActive() ? getOutputStream().flags(v) : 0;
    }

    /**
     * Set the formatting flags.
     * @param[in] v The formatting flags.
     * @return The current set of formatting flags.
     */
    long setf(std::ios_base::fmtflags v) {
        return isActive() ? getOutputStream().setf(v) : 0;
    }

    /**
     * Get the fill char.
     * @return The fill char.
     */
    char fill() const {
        return isActive() ? getOutputStream().fill() : -1;
    }

    /**
     * Set the fill char.
     * @param
     * @return The new fill char.
     */
    char fill(char v) {
        return isActive() ? getOutputStream().fill(v) : -1;
    }

    /**
     * Get the precision.
     * @return The precision.
     */
    int precision() const {
        return isActive() ? getOutputStream().precision() : 0;
    }

    /**
     * Set the precision.
     * @param[in] v The precision.
     * @return The new precision.
     */
    int precision(int v) {
        return isActive() ? getOutputStream().precision(v) : 0;
    }

    /**
     * Get the read state.
     * @return The read state.
     */
    int rdstate() const {
        return isActive() ? getOutputStream().rdstate() : std::ios_base::failbit;
    }

    /**
     * Return 0 if no error bits are set.
     * @return The good state.
     */
    int good() const {
        return isActive() ? getOutputStream().good() : 0;
    }

    /**
     * Return zero if EOF has not been reached; otherwise return non-zero.
     * @return The EOF state.
     */
    int eof() const {
        return isActive() ? getOutputStream().eof() : 0;
    }

    /**
     * Return non-zero if error bits are set; otherwise return zero.
     * @return The bad state.
     */
    int bad() const {
        return isActive() ? getOutputStream().bad() : 0;
    }

    /**
     * Set the format flags.
     * @param[in] _f The format flags.
     * @param[in] _m The mask containing the flags to be modified.
     * @return The new format flags.
     */
    long setf(std::ios_base::fmtflags _f, std::ios_base::fmtflags _m) {
        return isActive() ? getOutputStream().setf(_f, _m) : 0;
    }

    /**
     * Clears the format flags selected by the mask.
     * @param[in] _l The mask specifying which format flags to clear.
     */
    void unsetf(std::ios_base::fmtflags _l) {
        if (isActive()) {
            getOutputStream().unsetf(_l);
        }
    }

    /**
     * Set a new value for the internal error state flags.
     * @param[in] _i The new error state flag value.
     */
    void clear(std::ios_base::iostate _i = std::ios_base::failbit) {
        if (isActive()) {
            getOutputStream().clear(_i);
        }
    }

private:

    /**
     * Set whether this LogStream is active.
     * @param[in] active The setting for enabling this LogStream to print messages.
     */
    void setActive(bool active) {
        m_active = active;
    }

private:
    std::string m_name;
    LOG::ELevel m_outputLevel;
    LOG::ELevel m_currentMessageLevel;
    ostream* m_os;
    bool m_active;
    std::ofstream* m_fos;
};
}

#endif
