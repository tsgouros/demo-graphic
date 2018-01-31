## Use this to find the MinVR package.

####################### LOOKING FOR MinVR
##
## You can set the MinVR location with
## -DMINVR_INSTALL_DIR=/my/location/of/MinVR/install
## Remember, you're looking for the install location, not the repo
## location.  The default install is at MINVR_REPO/build/install.
##
### Look for MinVR
if(MINVR_INSTALL_DIR)
  message("-- Looking for MinVR in: " ${MINVR_INSTALL_DIR})

  find_path(MINVR_INCLUDE_DIR
    NAMES api/MinVR.h
    HINTS
    ${MINVR_INSTALL_DIR}/include
    ENV CPATH
    /usr/local/include)

  find_library(MINVR_LIBRARY
    NAMES MinVR
    HINTS
    ${MINVR_INSTALL_DIR}/lib
    ENV LD_LIBRARY_PATH
    /usr/local/lib)

  find_library(MINVR_LIBRARY_DEBUG
    NAMES MinVRd
    HINTS
    ${MINVR_INSTALL_DIR}/lib
    ENV LD_LIBRARY_PATH
    /usr/local/lib)


else(MINVR_INSTALL_DIR)
  message("MinVR install location not specified.  But we'll still look for it.")
  message("Use  -DMINVR_INSTALL_DIR=/my/location/of/MinVR/install to specify it.")

  find_path(MINVR_INCLUDE_DIR
    NAMES api/MinVR.h
    HINTS
    /usr/local/include
    ${PROJECT_SOURCE_DIR}/../../MinVR/build/install/include   # Wild guess.
    ENV CPATH)

  find_library(MINVR_LIBRARY
    NAMES MinVR
    HINTS
    /usr/local/lib
    ${PROJECT_SOURCE_DIR}/../../MinVR/build/install/lib   # Wild guess.
    ENV LD_LIBRARY_PATH)

  find_library(MINVR_LIBRARY_DEBUG
    NAMES MinVRd
    HINTS
    /usr/local/lib
    ${PROJECT_SOURCE_DIR}/../../MinVR/build/install/lib   # Wild guess.
    ENV LD_LIBRARY_PATH)

endif(MINVR_INSTALL_DIR)

if (MINVR_LIBRARY_DEBUG)
  if (MINVR_LIBRARY)
    # nothing
  else (MINVR_LIBRARY)
    set(MINVR_LIBRARY ${MINVR_LIBRARY_DEBUG})
  endif (MINVR_LIBRARY)
else(MINVR_LIBRARY_DEBUG)
  if (MINVR_LIBRARY)
    set(MINVR_LIBRARY_DEBUG ${MINVR_LIBRARY})
  else (MINVR_LIBRARY)
    # nothing: all will fail
  endif (MINVR_LIBRARY)
endif (MINVR_LIBRARY_DEBUG)

# Handle the QUIETLY and REQUIRED arguments and set MINVR_FOUND to
# TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  MINVR
  DEFAULT_MSG
  MINVR_LIBRARY
  MINVR_INCLUDE_DIR)

# Copy the results to the output variables.
if(MINVR_FOUND)
  get_filename_component(MINVR_INSTALL ${MINVR_INCLUDE_DIR} DIRECTORY)
  set(MINVR_LIBRARIES ${MINVR_LIBRARY})
  set(MINVR_INCLUDE_DIRS ${MINVR_INCLUDE_DIR})
  message("** MinVR found.  Don't forget to set MINVR_ROOT:")
  message("     Type 'export MINVR_ROOT=${MINVR_INSTALL}'.")

else(MINVR_FOUND)
  set(MINVR_LIBRARIES)
  set(MINVR_INCLUDE_DIRS)
endif(MINVR_FOUND)

mark_as_advanced(MINVR_INCLUDE_DIRS MINVR_LIBRARIES)

