#=========
# FFTW
#=========

FetchContent_Declare(
  fftw
  URL  http://fftw.org/fftw-3.3.8.tar.gz
)
FetchContent_GetProperties(fftw)
if(NOT fftw_POPULATED)
  FetchContent_Populate(fftw)

  option(ENABLE_FFTW_BUILD "Enable reconfiguring and rerunning the FFTW build" ON)
  if (ENABLE_FFTW_BUILD)
    set(fftw_CACHE_ARGS         
      "-DCMAKE_INSTALL_PREFIX=${fftw_BINARY_DIR}/install"
      "-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON" # PIC
      #"-DCMAKE_CXX_FLAGS:STRING=-D_GLIBCXX_USE_CXX11_ABI=0" # ABI
      "-DENABLE_FLOAT:BOOL=ON" # Float!
      "-DENABLE_THREADS:BOOL=ON"
      "-DWITH_COMBINED_THREADS:BOOL=ON"
      "-DBUILD_TESTS:BOOL=OFF" # ON
      "-DENABLE_OPENMP:BOOL=OFF"
      "-DBUILD_SHARED_LIBS:BOOL=OFF"
      "-DDISABLE_FORTRAN:BOOL=ON"
      "-DENABLE_SSE:BOOL=ON"
      "-DENABLE_SSE2:BOOL=ON"
      "-DENABLE_AVX:BOOL=ON"
      "-DENABLE_AVX2:BOOL=ON"      
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
      OUTPUT_FILE       ${fftw_BINARY_DIR}/build_output_f.log
      ERROR_FILE        ${fftw_BINARY_DIR}/build_output_f.log
      RESULT_VARIABLE   result_f
    )
    message(STATUS "FFTW<float> build complete")
    if(result_f)
      message(FATAL_ERROR "Failed FFTW<float> build, see build log at:\n"
        "    ${fftw_BINARY_DIR}/build_output_f.log")
    endif()
  endif()
endif()
# Confirm that we can find FFTW.
set(FFTW_ROOT "${fftw_BINARY_DIR}/install")
set(FFTW_USE_STATIC_LIBS TRUE)
find_package(FFTW 
  #QUIET
  REQUIRED 
  COMPONENTS FLOAT_LIB
)
unset(FFTW_USE_STATIC_LIBS)
unset(FFTW_ROOT)
#check_target(FFTW::Float)
