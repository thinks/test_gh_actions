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
add_library(tph::foo ALIAS foo)

install(TARGETS foo EXPORT ${PROJECT_NAME}Targets
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )