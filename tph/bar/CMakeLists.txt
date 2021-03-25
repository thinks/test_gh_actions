add_library(bar)
target_sources(bar
  PRIVATE 
    "bar.h"
    "bar.cc"
)
target_include_directories(bar
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${TPH_COMMON_INCLUDE_DIRS}>  
)
add_library(tph::bar ALIAS bar)

install(
  TARGETS bar EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(BUILD_TESTING)
  add_executable(bar_test)
  target_sources(bar_test 
    PRIVATE "bar_test.cc"
  )
  target_link_libraries(bar_test 
    PRIVATE
      tph::bar 
      Catch2::Catch2
  )
  add_test(NAME bar_test COMMAND bar_test)
endif()