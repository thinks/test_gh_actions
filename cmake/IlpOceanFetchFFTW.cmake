#=========
# FFTW
#=========

FetchContent_Declare(
  fftw
  URL  http://fftw.org/fftw-3.3.9.tar.gz
)
FetchContent_GetProperties(fftw)
if(NOT fftw_POPULATED)
  FetchContent_Populate(fftw)

  option(ENABLE_FFTW_BUILD "Enable reconfiguring and rerunning the FFTW build" ON)
  option(ENABLE_FFTW_BUILD_LOG "Print the FFTW build log to the console" OFF)
  if (ENABLE_FFTW_BUILD)
    set(fftw_CACHE_ARGS         
      "-DCMAKE_INSTALL_PREFIX=${fftw_BINARY_DIR}/install"
      "-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON" # PIC

      "-DBUILD_SHARED_LIBS:BOOL=OFF" # static libs
      "-DBUILD_TESTS:BOOL=OFF" # no tests
      
      "-DENABLE_THREADS:BOOL=ON" # Use pthread
      "-DWITH_COMBINED_THREADS:BOOL=ON" # 

      "-DENABLE_FLOAT:BOOL=ON" # <float>

      "-DENABLE_SSE:BOOL=ON"
      "-DENABLE_SSE2:BOOL=ON"
      "-DENABLE_AVX:BOOL=OFF"
      "-DENABLE_AVX2:BOOL=OFF"      

      "-DDISABLE_FORTRAN:BOOL=ON"
    )

    if(CMAKE_TOOLCHAIN_FILE)
      list(APPEND fftw_CACHE_ARGS 
        "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}"
      )
    else()
      list(APPEND fftw_CACHE_ARGS 
        "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
        "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
      )
    endif()    

    get_property(isMulti GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(NOT isMulti)
      list(APPEND fftw_CACHE_ARGS "-DCMAKE_BUILD_TYPE:STRING=Release")
      #list(APPEND fftw_CACHE_ARGS "-DCMAKE_BUILD_TYPE:STRING=Debug")
      #message(STATUS "NOTE: Building FFTW<float> in Debug!")
    endif()

    if(CMAKE_GENERATOR_PLATFORM)
      list(APPEND fftw_GENERATOR_ARGS
        --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
      )
    endif()
    if(CMAKE_GENERATOR_TOOLSET)
      list(APPEND fftw_GENERATOR_ARGS
        --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
      )
    endif()

    message(STATUS "Configuring and building FFTW<float> immediately")
    execute_process(
      COMMAND ${CMAKE_CTEST_COMMAND}
              --build-and-test  ${fftw_SOURCE_DIR} ${fftw_BINARY_DIR}
              --build-generator ${CMAKE_GENERATOR} ${fftw_GENERATOR_ARGS}
              --build-target    install
              --build-noclean
              --build-options   ${fftw_CACHE_ARGS}
      WORKING_DIRECTORY ${fftw_SOURCE_DIR}
      OUTPUT_FILE       ${fftw_BINARY_DIR}/build_output.log
      ERROR_FILE        ${fftw_BINARY_DIR}/build_output.log
      RESULT_VARIABLE   result
    )
    if(result)
      file(READ ${fftw_BINARY_DIR}/build_output.log fftw_log)
      message(FATAL_ERROR "Failed FFTW<float> build, see build log:\n"
        "${fftw_log}")
    endif()
    if(ENABLE_FFTW_BUILD_LOG)
      file(READ ${fftw_BINARY_DIR}/build_output.log fftw_log)
      message(STATUS "FFTW<float> build log:\n"
        "${fftw_log}")
    endif()
    message(STATUS "FFTW<float> build complete")
  endif()
endif()
# Confirm that we can find FFTW.
find_package(FFTW3f
  #QUIET
  REQUIRED 
  #COMPONENTS FLOAT_LIB
  CONFIG
  PATHS "${fftw_BINARY_DIR}/install"
  NO_DEFAULT_PATH 
)
