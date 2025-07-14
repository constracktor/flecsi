#ifndef POISSON_TASKS_NORM_HH
#define POISSON_TASKS_NORM_HH

#include "specialization/mesh.hh"

namespace poisson {
namespace task {

double diff(mesh::accessor<ro> m,
  field<double>::accessor<ro, ro> aa,
  field<double>::accessor<ro, ro> ba);

double scale(mesh::accessor<ro> m, double sum);

void discrete_operator(mesh::accessor<ro> m,
  field<double>::accessor<ro, ro> ua,
  field<double>::accessor<rw, ro> Aua);
double print_residual(flecsi::future<double> residual,
  std::size_t ita,
  std::chrono::time_point<std::chrono::system_clock> start);

} // namespace task
} // namespace poisson

#endif
