find_path(GDML_DIR include/G4Processor/GDMLProcessor.h)
find_library(GDML_LIBRARY gdml ${GDML_DIR}/lib/)
set(GDML_INCLUDE_DIR ${GDML_DIR}/include)
find_package_handle_standard_args(GDML DEFAULT_MSG GDML_LIBRARY GDML_DIR)
