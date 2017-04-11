FIND_PATH( GDML_DIR G4Binding/G4Processor/G4Processor/GDMLProcessor.h )

FIND_LIBRARY( GDML_LIBRARY gdml ${GDML_DIR}/lib/ )

LIST( APPEND GDML_INCLUDE_DIRS 
  ${GDML_DIR}/Common/Processes
  ${GDML_DIR}/Common/Saxana  
  ${GDML_DIR}/Common/Schema  
  ${GDML_DIR}/Common/Writer
  ${GDML_DIR}/G4Binding/G4Evaluator
  ${GDML_DIR}/G4Binding/G4Processor
  ${GDML_DIR}/G4Binding/G4Subscribers
  ${GDML_DIR}/G4Binding/G4Writer
  ${GDML_DIR}/STEPBinding/STEPWriter
)

INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( GDML DEFAULT_MSG GDML_DIR )
