#include "finalize.hh"
#include "state.hh"
#include "tasks/io.hh"

using namespace flecsi;

void
poisson::action::finalize(control_policy & cp) {
#ifndef BENCHMARK_MODE
  execute<task::io, mpi>(cp.m, ud(cp.m), "solution");
#endif
} // finalize
