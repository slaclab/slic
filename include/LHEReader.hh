#pragma once

#include <fstream>

//~~ SLIC ~~//
#include "LHEEvent.hh"

namespace slic {

/**
 * @class LHEReader
 * @brief Reads LHE event data into an LHEEvent object
 */
class LHEReader {
 public:
  /**
   * Constructor.
   *
   * @param[in] file_name The path to the input file.
   */
  LHEReader(const std::string& file_name);

  /// Destructor
  ~LHEReader() = default;

  /**
   * Read the next event.
   * @return The next LHE event.
   */
  shared_ptr< LHEEvent > nextEvent();

 private:
  /// The input file stream.
  std::ifstream ifs_;
}; // LHEReader
} // namespace slic
