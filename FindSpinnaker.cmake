# FindSpinnaker.cmake
# Locate Spinnaker library and include directories

# Try to find Spinnaker library and include directories
find_path(Spinnaker_INCLUDE_DIR SpinnakerC.h HINTS /opt/spinnaker/include/spinc)

find_library(Spinnaker_LIBRARIES NAMES Spinnaker HINTS /opt/spinnaker/lib)

# Check if the library and include directories were found
if(Spinnaker_INCLUDE_DIR AND Spinnaker_LIBRARIES)
    set(Spinnaker_FOUND TRUE)
else()
    set(Spinnaker_FOUND FALSE)
endif()

# Provide information to CMake
if(Spinnaker_FOUND)
    if(NOT Spinnaker_FIND_QUIETLY)
        message(STATUS "Found Spinnaker: ${Spinnaker_LIBRARIES}")
    endif()
else()
    if(Spinnaker_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find Spinnaker library")
    else()
        message(STATUS "Spinnaker library not found")
    endif()
endif()

# Set the include directories and libraries
if(Spinnaker_FOUND)
    set(Spinnaker_INCLUDE_DIRS ${Spinnaker_INCLUDE_DIR})
    set(Spinnaker_LIBRARIES ${Spinnaker_LIBRARIES})
endif()

# Export the variables
mark_as_advanced(Spinnaker_INCLUDE_DIR Spinnaker_LIBRARIES)
