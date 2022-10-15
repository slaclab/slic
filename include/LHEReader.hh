#pragma once

#include <fstream>
#include <memory>
#include <string>

//~~ SLIC ~~//
#include "LHEEvent.hh"

namespace slic {

/**
 * @class LHEReader
 * @brief Reads LHE event data into an LHEEvent object
 */
class LHEReader {
 public:
  /// Constructor
  LHEReader() = default;

  /// Destructor
  ~LHEReader();

  /**
   * Open a file for processing.
   * @param[in] Path to the LHE file to process.
   */
  void open(const std::string& filename);

  /**
   * Read the next event.
   * @return The next LHE event.
   */
  std::shared_ptr<LHEEvent> nextEvent();

 private:
  /// The input file stream.
  std::ifstream ifs_;
};  // LHEReader
}  // namespace slic
