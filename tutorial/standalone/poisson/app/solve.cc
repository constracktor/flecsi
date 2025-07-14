#include "solve.hh"
#include "options.hh"
#include "poisson.hh"
#include "state.hh"
#include "tasks/norm.hh"
#include "tasks/smooth.hh"

#include <flecsi/execution.hh>
#include <flecsi/flog.hh>
#include <flecsi/utilities.hh>

using namespace flecsi;

void
poisson::action::solve(control_policy & cp) {
  util::annotation::rguard<solve_region> guard;
  double err{std::numeric_limits<double>::max()};

  std::size_t sub{50};
  std::size_t ita{0};

  auto f = execute<task::red, default_accelerator>(cp.m, ud(cp.m), fd(cp.m));
  execute<task::black, default_accelerator>(cp.m, ud(cp.m), fd(cp.m));
  f = execute<task::discrete_operator>(cp.m, ud(cp.m), Aud(cp.m));
  f.wait();
  std::chrono::time_point<std::chrono::system_clock> start_time;

  // The tracing utility traces and optimizes loops during a Legion run. In this
  // case a do-while loop will be analysed at every calls to the solve action.
  // The trace is created before the loop; note that the object is static so the
  // identifier of the trace remains the same across the calls to the solver
  // action. The following call to the skip method ensure that the first loop of
  // the do-while loop will not be traced which is required for specific Legion
  // implementation. Inside the loop a guard is created. This creation starts
  // the tracing and its destruction at the end of the do-while loop stops the
  // trace.
  static exec::trace t;
  t.skip();
  std::vector<flecsi::future<double>> runtime_futures;

  start_time = std::chrono::system_clock::now();
  do {
    auto g = t.make_guard();
    // Annotation to time each cycle of the poisson solve
    util::annotation::guard<util::annotation::execution,
      util::annotation::detail::low>
      aguard("poisson-cycle");
    for(std::size_t i{0}; i < sub; ++i) {
      execute<task::red, default_accelerator>(cp.m, ud(cp.m), fd(cp.m));
      execute<task::black, default_accelerator>(cp.m, ud(cp.m), fd(cp.m));
    } // for
    ita += sub;

    f = execute<task::discrete_operator>(cp.m, ud(cp.m), Aud(cp.m));
    auto residual =
      reduce<task::diff, exec::fold::sum>(cp.m, fd(cp.m), Aud(cp.m));

    runtime_futures.push_back(
      execute<task::print_residual>(residual, ita, start_time));
  } while(ita < max_iterations.value());

  // wait for iterations to be finished
  std::vector<double> runtime_sums;
  for(auto runtime : runtime_futures) {
    runtime_sums.push_back(runtime.get() / 1e9);
  }

#ifdef BENCHMARK_MODE
  // write output data into file
  if(flecsi::process() == 0) {
    // store runtimes
    std::string runtimes = "";
    // total runtime
    runtimes = runtimes + std::to_string(runtime_sums.back()) + ";";
    // first iteration
    runtimes = runtimes + std::to_string(runtime_sums[0]) + ";";
    // subsequent iterations
    for(std::size_t i = 1; i < runtime_sums.size(); ++i) {
      runtimes =
        runtimes + std::to_string(runtime_sums[i] - runtime_sums[i - 1]) + ";";
    }
    std::ofstream runtime_file;
    runtime_file.open("result/runtimes.txt", std::ios_base::app);
    runtime_file << runtimes << "\n";
    runtime_file.close();
  }
#else
  if(flecsi::process() == 0) {
    // total runtime
    std::cout << "Total runtime: " << runtime_sums.back() << "s" << std::endl
              << std::flush;
    // first iteration
    std::cout << "Iterations  0 - " << sub << ": " << runtime_sums[0] << "s"
              << std::endl
              << std::flush;
    // subsequent iterations
    for(std::size_t i = 1; i < runtime_sums.size(); ++i) {
      std::cout << "Iterations " << i * sub << " - " << (i + 1) * sub << ": "
                << runtime_sums[i] - runtime_sums[i - 1] << "s" << std::endl
                << std::flush;
    }
  }
#endif
} // solve
