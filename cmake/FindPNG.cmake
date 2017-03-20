## Finding the libPNG on the yurt, and perhaps other systems.

# First look for the include files.
find_path(
  PNG_INCLUDE_DIR
  NAMES png.h
  HINTS
  ENV CPATH # For OSCAR modules at Brown/CCV
  /usr/local/include
  /usr/include
  /opt/local/include
  )

find_library(
  PNG_LIBRARY
  NAMES png
  HINTS
  ENV LD_LIBRARY_PATH # For OSCAR modules at Brown/CCV
  /usr/local/lib
  /usr/lib
  /opt/local/lib
  )

# Handle the QUIETLY and REQUIRED arguments and set PNG_FOUND to
# TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  PNG
  DEFAULT_MSG 
  PNG_LIBRARY 
  PNG_INCLUDE_DIR)

# Copy the results to the output variables.
if(PNG_FOUND)
  set(PNG_LIBRARIES ${PNG_LIBRARY})
  set(PNG_INCLUDE_DIRS ${PNG_INCLUDE_DIR})
else(PNG_FOUND)
  message("-- COULD NOT FIND (ADEQUATE) PNG LIBRARY.")
  set(PNG_LIBRARIES)
  set(PNG_INCLUDE_DIRS)
endif(PNG_FOUND)

mark_as_advanced(PNG_INCLUDE_DIRS PNG_LIBRARIES)
