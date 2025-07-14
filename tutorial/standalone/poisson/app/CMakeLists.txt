#------------------------------------------------------------------------------#
# Add application headers.
#------------------------------------------------------------------------------#

include_directories(${PROJECT_SOURCE_DIR}/include)

#------------------------------------------------------------------------------#
# Add an executable.
#------------------------------------------------------------------------------#

add_executable(poisson
  poisson.cc
  analyze.cc
  finalize.cc
  initialize.cc
  problem.cc
  solve.cc
  tasks/init.cc
  tasks/io.cc
  tasks/norm.cc
  tasks/smooth.cc
)

if(BENCHMARK_MODE)
  target_compile_definitions(poisson PUBLIC BENCHMARK_MODE)
endif()

#------------------------------------------------------------------------------#
# Add the FleCSI libraries and dependencies.
#------------------------------------------------------------------------------#

target_link_libraries(poisson FleCSI::FleCSI)

#------------------------------------------------------------------------------#
# Add unit test
#------------------------------------------------------------------------------#

add_unit(poisson ARGUMENTS 64 64)
