cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Set BUILD_TESTING to OFF by default.
# This must come before the project() and include(CTest) lines.
option(BUILD_TESTING "Build tests" OFF)

file(STRINGS VERSION.txt tph_version)
project(tph VERSION ${tph_version} LANGUAGES CXX)

# Include current path.
list(APPEND TPH_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.9.2
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
  FetchContent_GetProperties(catch2)
  if(NOT catch2_POPULATED)
    FetchContent_Populate(catch2)

    set(catch2_CACHE_ARGS
      "-DCMAKE_INSTALL_PREFIX:STRING=${catch2_BINARY_DIR}/install" 
      "-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON"
      "-DCMAKE_BUILD_TYPE:STRING=Release"
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
      "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
      "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"      
      "-DCATCH_ENABLE_WERROR=OFF"       
      "-DCATCH_BUILD_TESTING:BOOL=OFF"
      "-DCATCH_INSTALL_DOCS:BOOL=OFF"   
      "-DCATCH_INSTALL_HELPERS:BOOL=OFF"   
    )

    if(CMAKE_GENERATOR_PLATFORM)
      list(APPEND catch2_GENERATOR_ARGS
        --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
      )
    endif()
    if(CMAKE_GENERATOR_TOOLSET)
      list(APPEND catch2_GENERATOR_ARGS
        --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
      )
    endif()

    message(STATUS "Configuring and building Catch2 immediately")
    execute_process(
      COMMAND ${CMAKE_CTEST_COMMAND}
              --build-and-test  ${catch2_SOURCE_DIR} ${catch2_BINARY_DIR}
              --build-generator ${CMAKE_GENERATOR} ${catch2_GENERATOR_ARGS}
              --build-target    install
              --build-noclean
              --build-options   ${catch2_CACHE_ARGS}
      WORKING_DIRECTORY ${catch2_SOURCE_DIR}
      OUTPUT_FILE       ${catch2_BINARY_DIR}/build_output.log
      ERROR_FILE        ${catch2_BINARY_DIR}/build_output.log
      RESULT_VARIABLE   result
    )
    if(result)
      message(FATAL_ERROR "Failed Catch2 build, see build log at:\n"
        "    ${catch2_BINARY_DIR}/build_output.log")
    endif()
    message(STATUS "Catch2 build complete")
  endif()
  find_package(Catch2 
    #QUIET 
    REQUIRED 
    CONFIG
    PATHS "${catch2_BINARY_DIR}/install"
    NO_DEFAULT_PATH 
  ) 
  if (NOT Catch2_FOUND) 
    message(FATAL_ERROR "Catch2 not found")
  endif()

  enable_testing()
endif()

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/tph)

add_subdirectory(tph)


##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(TARGETS foo bar
  EXPORT ${PROJECT_NAME}-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers.
install(
  DIRECTORY ${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
    PATTERN "*.h"
)

# Export the targets to a script.
install(
  EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE tph::
  DESTINATION ${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the config, configversion and custom find modules.
install(
  FILES
    #${CMAKE_CURRENT_LIST_DIR}/cmake/FindRapidJSON.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)

