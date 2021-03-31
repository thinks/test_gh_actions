add_library(foo)
target_sources(foo
  PRIVATE
    "foo.h"
    "foo.cc"
)
target_include_directories(foo
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${TPH_COMMON_INCLUDE_DIRS}>  
)
target_link_libraries(foo
  PUBLIC
    FFTW3::fftw3f
    #Threads::Threads
    #"-lm"
)
add_library(tph::foo ALIAS foo)

install(TARGETS foo EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(BUILD_TESTING)
  add_executable(foo_test)
  target_sources(foo_test 
    PRIVATE "foo_test.cc"
  )
  target_link_libraries(foo_test 
    PRIVATE
      tph::foo 
      Catch2::Catch2
  )
  add_test(NAME foo_test COMMAND foo_test)

  install(TARGETS foo_test EXPORT ${PROJECT_NAME}Targets
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
endif()