#include "initialize.hh"
#include "options.hh"
#include "specialization/control.hh"
#include "state.hh"

#ifdef BENCHMARK_MODE
#include <fstream>
#endif

#include <flecsi/flog.hh>

using namespace flecsi;

void
poisson::action::init_mesh(control_policy & cp) {
  flog(info) << "Initializing " << x_extents.value() << "x" << y_extents.value()
             << " mesh" << std::endl;
  flecsi::flog::flush();

#ifdef BENCHMARK_MODE
  // Write out if process 0
  if(flecsi::process() == 0) {
    std::ofstream runtime_file;
    runtime_file.open("result/runtimes.txt", std::ios_base::app);
    if(header.value()) {
      runtime_file << "process;threads;x_extents;y_extents;max_iterations;"
                      "total_runtime;iteration_runtimes;\n";
    }
#if defined(FLECSI_ENABLE_HPX)
    auto threads = hpx::get_os_thread_count();
#else
    auto threads = 0; // omp_get_num_threads();
#endif
    runtime_file << flecsi::processes() << ";" << threads << ";"
                 << x_extents.value() << ";" << y_extents.value() << ";"
                 << max_iterations.value() << ";";
    runtime_file.close();
  }
#endif

  mesh::gcoord axis_extents{x_extents.value(), y_extents.value()};

  mesh::grect geometry;
  geometry[0][0] = 0.0;
  geometry[0][1] = 1.0;
  geometry[1] = geometry[0];

  cp.m.allocate(
    mesh::mpi_coloring(flecsi::processes(), axis_extents), geometry);
} // init_mesh
