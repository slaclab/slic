#pragma once

#include <memory>

//~~ Geant4 ~~//
#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
//class G4UIcmdWithAnInteger;
//class G4UIcmdWithADouble;
//class G4UIcmdWithADoubleAndUnit;

namespace slic {

// Forward declare to avoid circular dependency in headers
class PrimaryGeneratorAction; 

/**
 * @class PrimaryGeneratorMessenger
 * @brief Geant4 commands related to event generation.
 */
class PrimaryGeneratorMessenger : public G4UImessenger {
 public:
  /**
   * Constructor.
   * @param[in] pga The primary generator object that will be configured by 
   *  this messenger. 
   */
  PrimaryGeneratorMessenger();

  /// Destructor.
  ~PrimaryGeneratorMessenger() = default;

  /**
   * Process a macro command.
   * @param[in] command The macro command.
   * @param[in] newValues The command arguments.
   */
  void SetNewValue(G4UIcommand* command, G4String new_values);

 private:
 
  /// The generator macro directory. 
  std::unique_ptr<G4UIdirectory> dir_; 

  /// The class name of the PrimaryGenerator to load.
  std::unique_ptr<G4UIcommand> class_name_cmd_; 

  ///  Command used to open a file.
  std::unique_ptr<G4UIcommand> open_cmd_; 
  
  //G4UIcommand* m_randomSeedCmd;
  //G4UIcommand* m_resetCmd;
  //G4UIcmdWithAnInteger* m_skipEventsCmd;
  //G4UIcommand* m_filenameCmd;
  //G4UIcommand* m_enablePrintEventCmd;
  //G4UIcommand* m_printNumEventsGeneratedCmd;
  //G4UIcmdWithADoubleAndUnit* m_setLorentzTransformationAngleCmd;
  //G4UIcmdWithADouble* m_setZSmearingParameterCmd;

}; // PrimaryGeneratorMessenger
}  // namespace slic
