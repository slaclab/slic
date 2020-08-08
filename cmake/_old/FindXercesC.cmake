find_path(XercesC_INCLUDE_DIR xercesc/sax/InputSource.hpp xercesc/dom/DOMDocument.hpp xercesc/dom/DOMErrorHandler.hpp ${XercesC_DIR}/include)
find_library(XercesC_LIBRARY xerces-c ${XercesC_DIR}/lib)
# /usr/lib64 /usr/lib /usr/local/lib64 /usr/local/lib

# Find the package
FIND_PACKAGE_HANDLE_STANDARD_ARGS(XercesC DEFAULT_MSG XercesC_LIBRARY XercesC_INCLUDE_DIR)

