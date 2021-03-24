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